#include "animation.h"

WakeAnimation::WakeAnimation()
  : _currentState(ANIM_IDLE), _stateStartTime(0), _isFinished(true) {}

void WakeAnimation::start() {
  _currentState = ANIM_SLEEPY;
  _stateStartTime = millis();
  _isFinished = false;
}

bool WakeAnimation::isFinished() const {
  return _isFinished;
}

void WakeAnimation::renderFace(Adafruit_SSD1306& display, const char* str) {

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  // Made by-
  display.setTextSize(1);
  display.setCursor(26, 0);
  display.print("Made by-");  //Made by- এর position

  // RASHID
  display.setTextSize(2);
  display.setCursor(26, 15);
  display.print("RASHID");  //RASHID এর position

  // Face
  display.setTextSize(2);

  int16_t x1, y1;
  uint16_t w, h;

  display.getTextBounds(str, 0, 0, &x1, &y1, &w, &h);

  display.setCursor((128 - w) / 2, 42);
  display.print(str);

  display.display();
}

void WakeAnimation::update(unsigned long currentMillis, Adafruit_SSD1306& display) {
  if (_isFinished) return;

  const unsigned long elapsed = currentMillis - _stateStartTime;

  switch (_currentState) {
    case ANIM_SLEEPY:
      renderFace(display, "(-_-)");
      if (elapsed >= 300) {
        _currentState = ANIM_EYES_OPEN;
        _stateStartTime = currentMillis;
      }
      break;

    case ANIM_EYES_OPEN:
      renderFace(display, "(o_o)");
      if (elapsed >= 500) {
        _currentState = ANIM_BLINK;
        _stateStartTime = currentMillis;
      }
      break;

    case ANIM_BLINK:
      renderFace(display, "(o_-)");
      if (elapsed >= 600) {
        _currentState = ANIM_SMILE;
        _stateStartTime = currentMillis;
      }
      break;

    case ANIM_LEFT_OPEN:
      renderFace(display, "(o_o)");
      if (elapsed >= 250) {
        _currentState = ANIM_SMILE;
        _stateStartTime = currentMillis;
      }
      break;

    case ANIM_SMILE:
      renderFace(display, "(^_^)");
      if (elapsed >= 2000) {  /// Intero এর time
        _currentState = ANIM_COMPLETE;
        _isFinished = true;
      }
      break;

    case ANIM_COMPLETE:
    default:
      _isFinished = true;
      break;
  }
}