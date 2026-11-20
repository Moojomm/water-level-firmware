# Water Level Monitor — Firmware

เฟิร์มแวร์เครื่องวัดระดับน้ำ: **ESP32 DevKit V1 (30 ขา)** อ่านระยะจากเซนเซอร์อัลตราโซนิกกันน้ำ **JSN-SR04T** คำนวณระดับน้ำ แล้วส่งขึ้น **ThingsBoard Cloud** ผ่าน MQTT ทุก 10 วินาที ให้หน้าเว็บแสดงผล

```
JSN-SR04T ──TRIG/ECHO──▶ ESP32 ──Wi-Fi/MQTT──▶ ThingsBoard ──▶ Web Dashboard
```

## ฮาร์ดแวร์

| อุปกรณ์ | รุ่น |
|---|---|
| ไมโครคอนโทรลเลอร์ | ESP32 DevKit V1 (DOIT) 30 ขา |
| เซนเซอร์ | JSN-SR04T (วัดได้ ~25–450 cm, ±1 cm) |
| ไฟเลี้ยง | Adapter 5V ≥ 1A เข้าเทอร์มินัล J1 |
| PCB | Carrier board 57.5 × 46 mm (EasyEDA) — มีตัวต้านทานแบ่งแรงดัน ECHO 1k/2k |

การต่อขาดูที่ [`docs/wiring.md`](docs/wiring.md) — สรุป: **TRIG → D26**, **ECHO → (1k/2k) → D27**

## เริ่มใช้งาน

ใช้ [PlatformIO](https://platformio.org/) (ติดตั้งเป็น extension ใน VS Code)

```bash
git clone <repo-url>
cd water-level-firmware

# 1) ทดสอบเซนเซอร์ก่อน (ไม่ต้องมี Wi-Fi) — ดูค่าใน Serial Monitor
pio run -e sensor_test -t upload
pio device monitor

# 2) ใส่รหัส Wi-Fi + token ThingsBoard
cp src/secrets.h.example src/secrets.h     # แล้วแก้ค่าใน secrets.h

# 3) ใช้งานจริง
pio run -e devkit -t upload
pio device monitor

# ทดสอบสูตรคำนวณบนคอม (ไม่ต้องต่อบอร์ด)
pio test -e native
```

> `src/secrets.h` อยู่ใน `.gitignore` — **ห้ามคอมมิตรหัส Wi-Fi หรือ token ขึ้น GitHub**

## ตั้งค่าตามจุดติดตั้ง — `src/config.h`

| ค่า | ความหมาย | ค่าเริ่มต้น |
|---|---|---|
| `SENSOR_HEIGHT_CM` | ระยะจากหน้าเซนเซอร์ถึงก้นถัง/ก้นคลอง | 200 |
| `FULL_LEVEL_CM` | ความสูงน้ำที่ถือว่าเต็ม 100% | 170 |
| `SEND_INTERVAL_MS` | ส่งข้อมูลทุกกี่มิลลิวินาที | 10000 |
| `KEY_*` | ชื่อ key ที่ส่งขึ้น ThingsBoard (ต้องตรงกับหน้าเว็บ) | ดูด้านล่าง |

ติดหัวเซนเซอร์ **หันลงตรงๆ**, สูงกว่าระดับน้ำสูงสุดอย่างน้อย **25 cm** (ใกล้กว่านี้วัดไม่ได้) และไม่ชิดผนัง

## ข้อมูลที่ส่งขึ้น ThingsBoard

**Telemetry** — `v1/devices/me/telemetry` ทุก `SEND_INTERVAL_MS`

```json
{ "sensor_ok": true, "rssi": -61, "distance_cm": 150.2, "water_level_cm": 49.8, "water_level_percent": 29 }
```

| key | หน่วย | หมายเหตุ |
|---|---|---|
| `water_level_percent` | % | 0–100 |
| `water_level_cm` | cm | ความสูงน้ำจากก้น |
| `distance_cm` | cm | ระยะจากเซนเซอร์ถึงผิวน้ำ |
| `sensor_ok` | bool | `false` = วัดไม่ได้รอบนี้ (จะไม่ส่ง 3 ค่าบน เพื่อไม่ให้กราฟมีค่าปลอม) |
| `rssi` | dBm | ความแรง Wi-Fi |

**Attributes** (ส่งครั้งเดียวหลังต่อได้) — `firmware`, `board`, `sensor`, `sensor_height_cm`, `full_level_cm`, `send_interval_s`

การแจ้งเตือน (เช่น DANGER เมื่อน้ำสูง) **คำนวณที่หน้าเว็บ** เฟิร์มแวร์ส่งแค่ค่าวัด

## ไฟสถานะ (LED สีน้ำเงินบนบอร์ด)

| ไฟ | ความหมาย |
|---|---|
| ติดค้าง | ต่อ ThingsBoard ได้ ส่งข้อมูลปกติ |
| กะพริบเร็ว | ยังต่อ Wi-Fi หรือ ThingsBoard ไม่ได้ (ดูสาเหตุใน Serial Monitor) |

## โครงสร้างโปรเจกต์

```
├── platformio.ini            env: devkit / sensor_test / native
├── src/
│   ├── main.cpp              loop หลัก: วัด → ส่ง
│   ├── pins.h                ขาทั้งหมด (อิง PCB)
│   ├── config.h              ค่าติดตั้ง + ชื่อ key
│   ├── secrets.h.example     เทมเพลตรหัส Wi-Fi / token
│   ├── sensor.cpp/.h         อ่าน JSN-SR04T 7 ครั้ง → ค่ากลาง
│   └── net.cpp/.h            Wi-Fi + MQTT ThingsBoard (ต่อใหม่เองถ้าหลุด)
├── lib/waterlevel_core/      สูตรคำนวณ (ไม่ขึ้นกับฮาร์ดแวร์ → ทดสอบบนคอมได้)
├── test/test_waterlevel/     unit test ของสูตรคำนวณ
└── docs/
    ├── wiring.md             ตารางต่อขา + วิธีทดสอบวงจรแบ่งแรงดัน
    └── PROGRESS.md           ความคืบหน้า / บั๊กที่เจอ
```

## แก้ปัญหาเบื้องต้น

| อาการใน Serial Monitor | สาเหตุที่น่าจะเป็น |
|---|---|
| `no valid reading` ตลอด | สาย TRIG/ECHO สลับ, ไม่มีไฟ 5V ที่เซนเซอร์, หรือวัตถุใกล้กว่า 25 cm |
| ค้างที่ `Wi-Fi not connected` | ชื่อ/รหัส Wi-Fi ผิด หรือเป็น Wi-Fi 5 GHz (ESP32 ใช้ได้แค่ 2.4 GHz) |
| `ThingsBoard failed, state=4` หรือ `5` | Access token ผิด |
| `ThingsBoard failed, state=-2` | `TB_HOST` ผิด (US = `thingsboard.cloud`, EU = `eu.thingsboard.cloud`) หรือเน็ตบล็อกพอร์ต 1883 |
| อัปโหลดค้างที่ `Connecting....` | กดปุ่ม **BOOT** บนบอร์ดค้างไว้จนเริ่มอัปโหลด |

## ผู้จัดทำ

- สรภัส ภาวงศ์ (68021896) — Firmware
- นิมิตร มณีโชติ (68023978)
