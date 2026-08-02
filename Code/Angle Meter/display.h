#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"
#include "battery.h"

class DisplayUI {
public:
  DisplayUI();

  bool begin();
  void clear();
  void render(float targetAngle, bool isLevel, BatteryMonitor& battery);

  Adafruit_SSD1306& getDriver();

private:

  void drawHeaderAngle(float angle);
  void drawAnalogNeedle(int16_t tipX, int16_t tipY);
  void drawFooterStatus(bool isLevel);

  Adafruit_SSD1306 _oled;
  float _interpolatedNeedleAngle;

  // Partial redraw & delta cache state
  float _lastDrawnAngleText;
  float _lastDrawnNeedleAngle;
  int16_t _lastNeedleX;
  int16_t _lastNeedleY;
  bool _lastDrawnLevel;
  uint8_t _lastDrawnBatterySegments;
  bool _firstRender;
};

#endif  // DISPLAY_H