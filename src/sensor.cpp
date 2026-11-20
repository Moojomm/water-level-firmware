#include "sensor.h"

#include <Arduino.h>

#include "config.h"
#include "pins.h"

namespace sensor {

static const waterlevel::Config kCfg = {
    SENSOR_HEIGHT_CM,
    FULL_LEVEL_CM,
    MIN_DISTANCE_CM,
    MAX_DISTANCE_CM,
};

void begin() {
  pinMode(PIN_TRIG, OUTPUT);
  digitalWrite(PIN_TRIG, LOW);
  pinMode(PIN_ECHO, INPUT);
}

// ยิงคลื่น 1 ครั้ง คืนระยะเป็น cm หรือ -1 ถ้าไม่มีเสียงสะท้อน
static float pingOnceCm() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(5);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(20);  // JSN-SR04T ต้องการพัลส์ ≥ 10 µs — ใช้ 20 µs เผื่อรุ่นใหม่
  digitalWrite(PIN_TRIG, LOW);

  unsigned long us = pulseIn(PIN_ECHO, HIGH, ECHO_TIMEOUT_US);
  if (us == 0) return -1.0f;
  return waterlevel::echoToDistanceCm(us);
}

waterlevel::Reading read() {
  float samples[SAMPLES_PER_READING];
  for (int i = 0; i < SAMPLES_PER_READING; i++) {
    samples[i] = pingOnceCm();
    delay(SAMPLE_GAP_MS);
  }
  return waterlevel::fromSamples(samples, SAMPLES_PER_READING, kCfg, MIN_VALID_SAMPLES);
}

}  // namespace sensor
