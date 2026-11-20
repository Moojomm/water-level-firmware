// ทดสอบสูตรคำนวณบนคอม (ไม่ต้องต่อบอร์ด):  pio test -e native

#include <unity.h>

#include <WaterLevel.h>

using namespace waterlevel;

static const Config cfg = {200.0f, 170.0f, 25.0f, 450.0f};

void setUp() {}
void tearDown() {}

void test_echo_to_distance() {
  // 1000 µs ไป-กลับ → 17.15 cm
  TEST_ASSERT_FLOAT_WITHIN(0.01f, 17.15f, echoToDistanceCm(1000));
  TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.0f, echoToDistanceCm(0));
}

void test_level_from_distance() {
  // ติดสูง 200, วัดได้ 150 → น้ำสูง 50 cm = 50/170 = 29%
  Reading r = fromDistance(150.0f, cfg);
  TEST_ASSERT_TRUE(r.valid);
  TEST_ASSERT_FLOAT_WITHIN(0.01f, 50.0f, r.levelCm);
  TEST_ASSERT_FLOAT_WITHIN(0.1f, 29.4f, r.percent);
}

void test_full_tank_clamps_to_100() {
  Reading r = fromDistance(26.0f, cfg);  // น้ำสูง 174 > 170
  TEST_ASSERT_TRUE(r.valid);
  TEST_ASSERT_FLOAT_WITHIN(0.01f, 100.0f, r.percent);
}

void test_distance_beyond_bottom_is_zero_level() {
  Reading r = fromDistance(230.0f, cfg);  // ไกลกว่าก้น (เช่น ก้นเป็นหลุม) → 0 ไม่ติดลบ
  TEST_ASSERT_TRUE(r.valid);
  TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.0f, r.levelCm);
  TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.0f, r.percent);
}

void test_too_close_is_invalid() {
  TEST_ASSERT_FALSE(fromDistance(20.0f, cfg).valid);  // อยู่ใน blind zone
}

void test_no_echo_is_invalid() {
  TEST_ASSERT_FALSE(fromDistance(-1.0f, cfg).valid);
  TEST_ASSERT_FALSE(fromDistance(500.0f, cfg).valid);
}

void test_median_odd_and_even() {
  float a[] = {5, 1, 3};
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 3.0f, median(a, 3));
  float b[] = {4, 1, 3, 2};
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 2.5f, median(b, 4));
}

void test_samples_ignore_spikes_and_misses() {
  // มีค่าหลุด (-1 = ไม่มีเสียงสะท้อน) และค่ากระโดด 400 → ค่ากลางยังเป็น ~150
  float s[] = {150.2f, -1.0f, 149.8f, 400.0f, 150.0f, -1.0f, 150.4f};
  Reading r = fromSamples(s, 7, cfg, 3);
  TEST_ASSERT_TRUE(r.valid);
  TEST_ASSERT_FLOAT_WITHIN(0.01f, 150.2f, r.distanceCm);
}

void test_samples_not_enough_valid() {
  float s[] = {-1.0f, -1.0f, 150.0f, -1.0f, 10.0f};
  TEST_ASSERT_FALSE(fromSamples(s, 5, cfg, 3).valid);
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_echo_to_distance);
  RUN_TEST(test_level_from_distance);
  RUN_TEST(test_full_tank_clamps_to_100);
  RUN_TEST(test_distance_beyond_bottom_is_zero_level);
  RUN_TEST(test_too_close_is_invalid);
  RUN_TEST(test_no_echo_is_invalid);
  RUN_TEST(test_median_odd_and_even);
  RUN_TEST(test_samples_ignore_spikes_and_misses);
  RUN_TEST(test_samples_not_enough_valid);
  return UNITY_END();
}
