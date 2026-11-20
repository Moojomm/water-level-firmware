// Water Level Monitor — ESP32 DevKit V1 + JSN-SR04T → ThingsBoard
//
// build แบบปกติ:          pio run -e devkit -t upload
// ทดสอบเซนเซอร์อย่างเดียว:  pio run -e sensor_test -t upload  (ไม่ต่อ Wi-Fi, ไม่ต้องมี secrets.h)

#include <Arduino.h>

#include "config.h"
#include "pins.h"
#include "sensor.h"

#ifndef SENSOR_TEST_ONLY
#include "net.h"
#endif

static unsigned long lastSend = 0;
static unsigned long lastBlink = 0;

static void printReading(const waterlevel::Reading& r) {
  if (r.valid) {
    Serial.printf("[sensor] distance=%.1f cm  level=%.1f cm  %.0f%%\n",
                  r.distanceCm, r.levelCm, r.percent);
  } else {
    Serial.println("[sensor] no valid reading (check wiring / too close < 25 cm / no echo)");
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.printf("\n=== Water Level Monitor v%s ===\n", FIRMWARE_VERSION);

  pinMode(PIN_STATUS_LED, OUTPUT);
  sensor::begin();

#ifndef SENSOR_TEST_ONLY
  net::begin();
#else
  Serial.println("SENSOR TEST MODE (no Wi-Fi)");
#endif
}

void loop() {
#ifdef SENSOR_TEST_ONLY
  printReading(sensor::read());
  digitalWrite(PIN_STATUS_LED, !digitalRead(PIN_STATUS_LED));
  delay(500);
#else
  unsigned long now = millis();
  net::loop();

  // ไฟสถานะ: ติดค้าง = ต่อ ThingsBoard ได้, กะพริบ = ยังไม่ได้
  if (net::connected()) {
    digitalWrite(PIN_STATUS_LED, HIGH);
  } else if (now - lastBlink >= 300) {
    lastBlink = now;
    digitalWrite(PIN_STATUS_LED, !digitalRead(PIN_STATUS_LED));
  }

  if (lastSend == 0 || now - lastSend >= SEND_INTERVAL_MS) {
    lastSend = now;
    waterlevel::Reading r = sensor::read();
    printReading(r);
    if (net::publish(r)) {
      Serial.println("[net] sent");
    } else {
      Serial.println("[net] not sent (offline)");
    }
  }
#endif
}
