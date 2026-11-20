#pragma once

// ค่าที่ต้องปรับตามจุดติดตั้งจริง — แก้ที่นี่ที่เดียว

#define FIRMWARE_VERSION      "0.1.0"

// ---- การติดตั้งเซนเซอร์ (หน่วย cm) ----
// วัดด้วยตลับเมตรจากหน้าหัวเซนเซอร์ลงไปถึงก้นถัง/ก้นคลอง
#define SENSOR_HEIGHT_CM      200.0f
// ความสูงน้ำที่ถือว่า "เต็ม 100%" (ต้องเว้นจากหัวเซนเซอร์อย่างน้อย 25 cm เพราะใกล้กว่านั้นวัดไม่ได้)
#define FULL_LEVEL_CM         170.0f

// ---- ขอบเขตของ JSN-SR04T ----
#define MIN_DISTANCE_CM       25.0f
#define MAX_DISTANCE_CM       450.0f

// ---- การวัด ----
#define SAMPLES_PER_READING   7       // วัดกี่ครั้งต่อ 1 รอบ แล้วเอาค่ากลาง
#define MIN_VALID_SAMPLES     3       // ต้องวัดได้อย่างน้อยกี่ครั้งถึงถือว่าใช้ได้
#define SAMPLE_GAP_MS         70      // เว้นระหว่างการวัดแต่ละครั้ง (เซนเซอร์ต้องการ > 60 ms)
#define ECHO_TIMEOUT_US       30000UL // ~5 m ไป-กลับ ถ้าเกินนี้ถือว่าไม่มีเสียงสะท้อน

// ---- การส่งข้อมูล ----
#define SEND_INTERVAL_MS      10000UL // ส่งขึ้น ThingsBoard ทุก 10 วินาที

// ---- ชื่อ key ที่หน้าเว็บอ่าน (ต้องตรงกับเว็บทุกตัวอักษร) ----
#define KEY_DISTANCE          "distance_cm"
#define KEY_LEVEL_CM          "water_level_cm"
#define KEY_LEVEL_PERCENT     "water_level_percent"
#define KEY_SENSOR_OK         "sensor_ok"
#define KEY_RSSI              "rssi"
