#ifndef BATTERY_H
#define BATTERY_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>

class BatteryMonitor {
public:
  BatteryMonitor();

  void begin();
  void update(unsigned long currentMillis);

  uint8_t getPercentage() const;
  float getVoltage() const;
  uint8_t getActiveSegments() const;
  bool isLowBattery() const;

  void drawIcon(Adafruit_SSD1306& display, int16_t x, int16_t y, bool forceVisible = false);

private:
  float readVoltage();
  uint8_t calculatePercentage(float voltage) const;

  float _currentVoltage;
  uint8_t _percentage;
  uint8_t _activeSegments;
  unsigned long _lastReadTime;
  unsigned long _lastBlinkTime;
  bool _blinkState;
};

#endif  // BATTERY_H