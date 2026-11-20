#pragma once

// ขาทั้งหมดอยู่ที่นี่ที่เดียว — อ้างอิงจาก PCB (EasyEDA: Sheet_1 / PCB_rrrrr)
// บอร์ด: ESP32 DevKit V1 (DOIT) 30 ขา, เสียบบน Female Header J3 (แถวซ้าย) / J4 (แถวขวา)
//
//   J2 (เซนเซอร์)  →  ESP32
//   VCC            →  VIN (+5V)
//   TRIG           →  D26              ต่อตรง (3.3V จาก ESP32 สั่งเซนเซอร์ได้)
//   ECHO           →  R1 1k → D27      R2 2k ลง GND — แบ่ง 5V เหลือ ~3.3V
//   GND            →  GND
//
// D26/D27 ไม่ใช่ขา strapping และไม่ใช่ขา input-only — ใช้ได้ปลอดภัย

#define PIN_TRIG        26
#define PIN_ECHO        27

// LED บนบอร์ด DevKit V1 (สีน้ำเงิน) ใช้เป็นไฟสถานะ
// ติดค้าง = ต่อ ThingsBoard ได้, กะพริบ = ยังต่อไม่ได้
#define PIN_STATUS_LED  2
