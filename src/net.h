#pragma once

#include <WaterLevel.h>

// Wi-Fi + ThingsBoard (MQTT) — ต่อใหม่เองอัตโนมัติถ้าหลุด
namespace net {

void begin();

// เรียกทุกรอบใน loop() — ดูแลการต่อ Wi-Fi/MQTT แบบไม่ค้าง (non-blocking)
void loop();

bool connected();  // true เมื่อต่อ ThingsBoard ได้

// ส่งค่าระดับน้ำ 1 ชุดขึ้น ThingsBoard (คืน false ถ้ายังไม่ได้ต่อ)
bool publish(const waterlevel::Reading& r);

}  // namespace net
