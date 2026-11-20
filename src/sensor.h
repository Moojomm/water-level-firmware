#pragma once

#include <WaterLevel.h>

namespace sensor {

void begin();

// วัด SAMPLES_PER_READING ครั้ง แล้วคืนค่าระดับน้ำ (ใช้เวลาราว 0.5 วินาที)
waterlevel::Reading read();

}  // namespace sensor
