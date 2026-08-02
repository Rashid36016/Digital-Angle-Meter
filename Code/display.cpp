#include "display.h"
#include <cmath>

namespace {
constexpr float DEG_TO_RAD_FACTOR = 0.017453292519943295f;  // PI / 180.0f

constexpr int16_t CENTER_X = 64;
constexpr int16_t CENTER_Y = 40;       /// Analog angle icon টা উপরে -নিচে করতে
constexpr int16_t NEEDLE_LENGTH = 17;  /// Analog angle icon এর length
constexpr int16_t PIVOT_RADIUS = 2;    /// Analog angle icon এর বল এর আকার

// Sub-pixel movement threshold (in degrees) to trigger needle redraws
constexpr float NEEDLE_ANGLE_THRESHOLD = 0.5f;
}

DisplayUI::DisplayUI()
  : _oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1),
    _interpolatedNeedleAngle(0.0f),
    _lastDrawnAngleText(-999.0f),
    _lastDrawnNeedleAngle(-999.0f),
    _lastNeedleX(-1),
    _lastNeedleY(-1),
    _lastDrawnLevel(false),
    _lastDrawnBatterySegments(255),
    _firstRender(true) {}

bool DisplayUI::begin() {
  if (!_oled.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS)) {
    return false;
  }
  _oled.clearDisplay();
  _oled.setTextColor(SSD1306_WHITE);
  _oled.display();
  _firstRender = true;
  return true;
}

Adafruit_SSD1306& DisplayUI::getDriver() {
  return _oled;
}

void DisplayUI::clear() {
  _oled.clearDisplay();
  _firstRender = true;
}

void DisplayUI::drawHeaderAngle(float angle) {
  const float absAngle = fabsf(angle);
  const int32_t totalTenths = static_cast<int32_t>(roundf(absAngle * 10.0f));
  const int32_t wholePart = totalTenths / 10;
  const int32_t fracPart = totalTenths % 10;

  char formattedStr[12];
  snprintf(formattedStr, sizeof(formattedStr), "%s%ld.%ld", (angle < -0.05f) ? "-" : "", (long)wholePart, (long)fracPart);

  _oled.fillRect(0, 0, 128, 18, SSD1306_BLACK);

  _oled.setTextSize(2);
  int16_t x1, y1;
  uint16_t w, h;
  _oled.getTextBounds(formattedStr, 0, 0, &x1, &y1, &w, &h);

  const int16_t cursorX = static_cast<int16_t>((128 - (w + 8)) / 2);

  _oled.setCursor(cursorX, 2);  //Digital angle -> Y
  _oled.print(formattedStr);

  const int16_t degX = cursorX + static_cast<int16_t>(w) + 3;
  _oled.drawCircle(degX, 4, 2, SSD1306_WHITE);  ///Degree icon -> Y
}

void DisplayUI::drawAnalogNeedle(int16_t tipX, int16_t tipY) {
  // Erase previous needle line FIRST in BLACK
  if (_lastNeedleX != -1 && _lastNeedleY != -1) {
    _oled.drawLine(CENTER_X, CENTER_Y, _lastNeedleX, _lastNeedleY, SSD1306_BLACK);
  }

  _lastNeedleX = tipX;
  _lastNeedleY = tipY;

  // Draw NEW needle vector SECOND in WHITE
  _oled.drawLine(CENTER_X, CENTER_Y, tipX, tipY, SSD1306_WHITE);

  // Redraw fixed center pivot ball THIRD
  _oled.fillCircle(CENTER_X, CENTER_Y, PIVOT_RADIUS, SSD1306_WHITE);
}

void DisplayUI::drawFooterStatus(bool isLevel) {
  _oled.fillRect(0, 52, 105, 12, SSD1306_BLACK);

  _oled.setTextSize(1);
  _oled.setCursor(34, 54);

  if (isLevel) {
    _oled.print("LEVEL ");
    const int16_t cx = _oled.getCursorX();
    const int16_t cy = _oled.getCursorY();
    _oled.drawLine(cx + 1, cy + 4, cx + 3, cy + 6, SSD1306_WHITE);
    _oled.drawLine(cx + 3, cy + 6, cx + 7, cy + 1, SSD1306_WHITE);
  } else {
    _oled.print("LEVEL");
  }
}

void DisplayUI::render(float targetAngle, bool isLevel, BatteryMonitor& battery) {
  constexpr float interpolationFactor = 0.35f;
  _interpolatedNeedleAngle += (targetAngle - _interpolatedNeedleAngle) * interpolationFactor;

  const float roundedDisplayedAngle = roundf(_interpolatedNeedleAngle * 10.0f) / 10.0f;

  const bool needleAngleChanged = (fabsf(_interpolatedNeedleAngle - _lastDrawnNeedleAngle) >= NEEDLE_ANGLE_THRESHOLD);

  const float rad = (_interpolatedNeedleAngle - 90.0f) * DEG_TO_RAD_FACTOR;
  const int16_t nextTipX = CENTER_X + static_cast<int16_t>(cosf(rad) * NEEDLE_LENGTH);
  const int16_t nextTipY = CENTER_Y + static_cast<int16_t>(sinf(rad) * NEEDLE_LENGTH);

  const bool angleTextChanged = (fabsf(roundedDisplayedAngle - _lastDrawnAngleText) >= 0.1f);
  const bool levelStateChanged = (isLevel != _lastDrawnLevel);
  const bool batteryChanged = (battery.getActiveSegments() != _lastDrawnBatterySegments);

  if (!_firstRender && !angleTextChanged && !needleAngleChanged && !levelStateChanged && !batteryChanged) {
    return;
  }

  if (_firstRender) {
    _oled.clearDisplay();
  }

  if (_firstRender || angleTextChanged || batteryChanged) {
    drawHeaderAngle(roundedDisplayedAngle);

    battery.drawIcon(_oled, 108, 4, true);

    _lastDrawnAngleText = roundedDisplayedAngle;
    _lastDrawnBatterySegments = battery.getActiveSegments();
  }

  if (_firstRender || needleAngleChanged) {
    drawAnalogNeedle(nextTipX, nextTipY);
    _lastDrawnNeedleAngle = _interpolatedNeedleAngle;
  }

  if (_firstRender || levelStateChanged) {
    drawFooterStatus(isLevel);
    _lastDrawnLevel = isLevel;
  }



  _firstRender = false;
  _oled.display();
}