#include "net.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi.h>

#include "config.h"
#include "secrets.h"

namespace net {

static WiFiClient wifiClient;
static PubSubClient mqtt(wifiClient);

static unsigned long lastWifiTry = 0;
static unsigned long lastMqttTry = 0;
static bool attributesSent = false;

static const unsigned long WIFI_RETRY_MS = 10000;
static const unsigned long MQTT_RETRY_MS = 5000;

static const char* TOPIC_TELEMETRY = "v1/devices/me/telemetry";
static const char* TOPIC_ATTRIBUTES = "v1/devices/me/attributes";

void begin() {
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  lastWifiTry = millis();

  mqtt.setServer(TB_HOST, TB_PORT);
  mqtt.setBufferSize(512);
  Serial.printf("[net] connecting Wi-Fi \"%s\"...\n", WIFI_SSID);
}

// ส่งข้อมูลคงที่ของเครื่องครั้งเดียวหลังต่อได้ (เห็นใน ThingsBoard > Attributes)
static void sendAttributes() {
  JsonDocument doc;
  doc["firmware"] = FIRMWARE_VERSION;
  doc["board"] = "ESP32 DevKit V1";
  doc["sensor"] = "JSN-SR04T";
  doc["sensor_height_cm"] = SENSOR_HEIGHT_CM;
  doc["full_level_cm"] = FULL_LEVEL_CM;
  doc["send_interval_s"] = SEND_INTERVAL_MS / 1000;
  char buf[256];
  size_t n = serializeJson(doc, buf, sizeof(buf));
  attributesSent = mqtt.publish(TOPIC_ATTRIBUTES, (const uint8_t*)buf, n, false);
}

void loop() {
  unsigned long now = millis();

  if (WiFi.status() != WL_CONNECTED) {
    if (now - lastWifiTry >= WIFI_RETRY_MS) {
      lastWifiTry = now;
      Serial.println("[net] Wi-Fi not connected, retrying...");
      WiFi.disconnect();
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    }
    return;
  }

  if (!mqtt.connected()) {
    attributesSent = false;
    if (now - lastMqttTry >= MQTT_RETRY_MS) {
      lastMqttTry = now;
      Serial.printf("[net] Wi-Fi OK (%s), connecting ThingsBoard %s...\n",
                    WiFi.localIP().toString().c_str(), TB_HOST);
      // ThingsBoard: username = access token, ไม่ต้องใส่รหัสผ่าน
      if (mqtt.connect("water-level-esp32", TB_DEVICE_TOKEN, nullptr)) {
        Serial.println("[net] ThingsBoard connected");
      } else {
        // state 4/5 = token ผิด, -2 = ต่อเซิร์ฟเวอร์ไม่ได้ (เช็ก TB_HOST)
        Serial.printf("[net] ThingsBoard failed, state=%d\n", mqtt.state());
      }
    }
    return;
  }

  mqtt.loop();
  if (!attributesSent) sendAttributes();
}

bool connected() { return mqtt.connected(); }

bool publish(const waterlevel::Reading& r) {
  if (!mqtt.connected()) return false;

  JsonDocument doc;
  doc[KEY_SENSOR_OK] = r.valid;
  doc[KEY_RSSI] = WiFi.RSSI();
  if (r.valid) {
    // ส่งเฉพาะตอนวัดได้ เพื่อไม่ให้กราฟประวัติมีค่า 0 ปลอม
    doc[KEY_DISTANCE] = roundf(r.distanceCm * 10) / 10;
    doc[KEY_LEVEL_CM] = roundf(r.levelCm * 10) / 10;
    doc[KEY_LEVEL_PERCENT] = roundf(r.percent);
  }
  char buf[256];
  size_t n = serializeJson(doc, buf, sizeof(buf));
  return mqtt.publish(TOPIC_TELEMETRY, (const uint8_t*)buf, n, false);
}

}  // namespace net
