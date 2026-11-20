#pragma once

// ตรรกะคำนวณระดับน้ำ — แยกออกจากฮาร์ดแวร์ทั้งหมด (ไม่ include Arduino.h)
// เพื่อให้ทดสอบบนคอมได้ด้วย `pio test -e native` โดยไม่ต้องต่อบอร์ด

#include <stddef.h>
#include <stdint.h>

namespace waterlevel {

struct Config {
  float sensorHeightCm;  // ระยะจากหน้าเซนเซอร์ถึงก้นถัง/ก้นคลอง (จุดที่ถือว่าน้ำ = 0)
  float fullLevelCm;     // ความสูงน้ำที่ถือว่าเต็ม 100%
  float minDistanceCm;   // ระยะใกล้สุดที่เซนเซอร์วัดได้ (JSN-SR04T ≈ 25 cm)
  float maxDistanceCm;   // ระยะไกลสุดที่ยอมรับ (JSN-SR04T ≈ 600 cm)
};

struct Reading {
  bool valid;         // false = วัดไม่ได้ (ไม่มีเสียงสะท้อน / ใกล้หรือไกลเกิน)
  float distanceCm;   // ระยะจากเซนเซอร์ถึงผิวน้ำ
  float levelCm;      // ความสูงของน้ำจากก้น
  float percent;      // 0–100
};

// ความเร็วเสียง ~343 m/s = 0.0343 cm/µs, หาร 2 เพราะคลื่นวิ่งไป-กลับ
inline float echoToDistanceCm(uint32_t echoUs) {
  return echoUs * 0.0343f / 2.0f;
}

inline bool isValidDistance(float cm, const Config& cfg) {
  return cm >= cfg.minDistanceCm && cm <= cfg.maxDistanceCm;
}

// ค่ากลางของ n ค่า (เรียงลำดับในอาร์เรย์ที่ส่งเข้ามา) — n ต้อง > 0
inline float median(float* v, size_t n) {
  for (size_t i = 1; i < n; i++) {  // insertion sort (n เล็ก ไม่กี่ค่า)
    float x = v[i];
    size_t j = i;
    while (j > 0 && v[j - 1] > x) {
      v[j] = v[j - 1];
      j--;
    }
    v[j] = x;
  }
  return (n % 2) ? v[n / 2] : (v[n / 2 - 1] + v[n / 2]) / 2.0f;
}

inline float clampf(float x, float lo, float hi) {
  return x < lo ? lo : (x > hi ? hi : x);
}

// แปลงระยะที่วัดได้เป็นระดับน้ำ
inline Reading fromDistance(float distanceCm, const Config& cfg) {
  Reading r{};
  r.distanceCm = distanceCm;
  if (!isValidDistance(distanceCm, cfg)) {
    r.valid = false;
    return r;
  }
  r.valid = true;
  r.levelCm = clampf(cfg.sensorHeightCm - distanceCm, 0.0f, cfg.sensorHeightCm);
  float pct = cfg.fullLevelCm > 0 ? r.levelCm / cfg.fullLevelCm * 100.0f : 0.0f;
  r.percent = clampf(pct, 0.0f, 100.0f);
  return r;
}

// รวมหลายค่าที่วัดมา: ทิ้งค่าที่ใช้ไม่ได้ แล้วเอาค่ากลาง
// ต้องมีค่าที่ใช้ได้อย่างน้อย minValid ค่า ไม่งั้นถือว่าวัดไม่ได้
inline Reading fromSamples(const float* samplesCm, size_t n, const Config& cfg, size_t minValid = 3) {
  float ok[16];
  size_t k = 0;
  for (size_t i = 0; i < n && k < 16; i++) {
    if (isValidDistance(samplesCm[i], cfg)) ok[k++] = samplesCm[i];
  }
  if (k == 0 || k < minValid) {
    Reading r{};
    r.valid = false;
    r.distanceCm = 0;
    return r;
  }
  return fromDistance(median(ok, k), cfg);
}

}  // namespace waterlevel
