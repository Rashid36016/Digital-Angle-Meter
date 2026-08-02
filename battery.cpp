#include "battery.h"
#include "config.h"

BatteryMonitor::BatteryMonitor()
  : _currentVoltage(4.2f),
    _percentage(100),
    _activeSegments(4),
    _lastReadTime(0),
    _lastBlinkTime(0),
    _blinkState(true) {}

void BatteryMonitor::begin() {
  pinMode(PIN_BATTERY_ADC, INPUT);

  _currentVoltage = readVoltage();
  _percentage = calculatePercentage(_currentVoltage);
  _activeSegments = (_percentage + 19) / 25;

  Serial.print("Voltage = ");
  Serial.print(_currentVoltage, 2);

  Serial.print("V  Percentage = ");
  Serial.print(_percentage);

  Serial.print("%  Segments = ");
  Serial.println(_activeSegments);
}

float BatteryMonitor::readVoltage() {
  uint32_t rawSum = 0;
  constexpr uint8_t samples = 16;
  for (uint8_t i = 0; i < samples; i++) {
    rawSum += analogRead(PIN_BATTERY_ADC);
  }
  const float rawAvg = static_cast<float>(rawSum) / static_cast<float>(samples);
  Serial.print("ADC = ");
  Serial.println(rawAvg);
  const float pinVoltage = (rawAvg / ADC_RESOLUTION) * ADC_VOLTAGE_REF;
  float batteryVoltage = pinVoltage * BATTERY_DIVIDER_RATIO;
  Serial.print("Ratio = ");
  Serial.println(BATTERY_DIVIDER_RATIO, 3);

  Serial.print("Pin Voltage = ");
  Serial.print(pinVoltage, 3);

  Serial.print("   Battery = ");
  Serial.println(batteryVoltage, 3);

  return batteryVoltage;
}

uint8_t BatteryMonitor::calculatePercentage(float voltage) const {
  if (voltage >= 4.15f) return 100;
  if (voltage <= 3.30f) return 0;

  if (voltage > 4.00f) return 80 + static_cast<uint8_t>((voltage - 4.00f) / 0.15f * 20.0f);
  if (voltage > 3.80f) return 50 + static_cast<uint8_t>((voltage - 3.80f) / 0.20f * 30.0f);
  if (voltage > 3.65f) return 20 + static_cast<uint8_t>((voltage - 3.65f) / 0.15f * 30.0f);
  return static_cast<uint8_t>((voltage - 3.30f) / 0.35f * 20.0f);
}

void BatteryMonitor::update(unsigned long currentMillis) {
  if (currentMillis - _lastReadTime >= INTERVAL_BATTERY_MS) {
    _lastReadTime = currentMillis;
    const float rawV = readVoltage();
    _currentVoltage = (_currentVoltage * 0.8f) + (rawV * 0.2f);
    _percentage = calculatePercentage(_currentVoltage);

    if (_percentage >= 80) _activeSegments = 4;
    else if (_percentage >= 60) _activeSegments = 3;
    else if (_percentage >= 40) _activeSegments = 2;
    else if (_percentage >= 20) _activeSegments = 1;
    else _activeSegments = 0;
  }

  if (_activeSegments == 0) {
    if (currentMillis - _lastBlinkTime >= 500) {
      _lastBlinkTime = currentMillis;
      _blinkState = !_blinkState;
    }
  } else {
    _blinkState = true;
  }
}

uint8_t BatteryMonitor::getPercentage() const {
  return _percentage;
}
float BatteryMonitor::getVoltage() const {
  return _currentVoltage;
}
uint8_t BatteryMonitor::getActiveSegments() const {
  return _activeSegments;
}
bool BatteryMonitor::isLowBattery() const {
  return _activeSegments == 0;
}

void BatteryMonitor::drawIcon(Adafruit_SSD1306& display, int16_t x, int16_t y, bool forceVisible) {
  constexpr int16_t w = 16;
  constexpr int16_t h = 8;

  if (_activeSegments == 0 && !_blinkState && !forceVisible) {
    // Low battery হলে পুরো icon disappear হবে
    display.fillRect(x, y, w + 3, h + 1, SSD1306_BLACK);
    return;
  }

  // অন্য সব অবস্থায় icon clear করে redraw হবে
  display.fillRect(x, y, w + 3, h + 1, SSD1306_BLACK);

  display.drawRect(x, y, w, h, SSD1306_WHITE);
  display.drawFastVLine(x + w, y + 2, 4, SSD1306_WHITE);

  for (uint8_t i = 0; i < _activeSegments; i++) {
    const int16_t segX = x + 2 + (i * 3);
    display.fillRect(segX, y + 2, 2, h - 4, SSD1306_WHITE);
  }
}