#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Pins
constexpr uint8_t PIN_I2C_SDA = 4;       // D2 (GPIO4)
constexpr uint8_t PIN_I2C_SCL = 5;       // D1 (GPIO5)
constexpr uint8_t PIN_MPU_INT = 14;      // D5 (GPIO14)
constexpr uint8_t PIN_BATTERY_ADC = A0;  // Analog Input A0

// Display
constexpr uint8_t SCREEN_WIDTH = 128;
constexpr uint8_t SCREEN_HEIGHT = 64;
constexpr uint8_t OLED_I2C_ADDRESS = 0x3C;

// Refresh rates & Scheduling (ms)
constexpr uint16_t INTERVAL_NEEDLE_MS = 50;     // 20 FPS
constexpr uint16_t INTERVAL_ANGLE_MS = 100;     // 10 FPS
constexpr uint16_t INTERVAL_BATTERY_MS = 2000;  // 2 seconds
constexpr uint8_t INTERVAL_SENSOR_MS = 10;      // 100 Hz

// Sleep time
constexpr uint32_t TIMEOUT_DIM_START_MS = 3000;    // 3s idle before dimming
constexpr uint32_t DURATION_DIM_FADE_MS = 2000;    // 2s fade
constexpr uint32_t TIMEOUT_SLEEP_TOTAL_MS = 5000;  // 5s total before screen off

// Battery ADC
constexpr float ADC_VOLTAGE_REF = 3.3f;
constexpr float ADC_RESOLUTION = 1024.0f;

// Temporary calibration factor
constexpr float BATTERY_DIVIDER_RATIO = 3.13f;

constexpr float BATTERY_MIN_V = 3.30f;
constexpr float BATTERY_MAX_V = 4.20f;

// Sensitivity
constexpr float LEVEL_TOLERANCE_DEG = 0.35f;


#endif  // CONFIG_H