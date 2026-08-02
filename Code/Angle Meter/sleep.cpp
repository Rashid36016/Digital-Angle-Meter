#include "sleep.h"
#include "config.h"

extern "C" {
#include "user_interface.h"
#include "gpio.h"
}

PowerManager::PowerManager()
  : _lastActivityTime(0), _currentState(POWER_ACTIVE), _currentContrast(255) {}

void PowerManager::begin() {
  _lastActivityTime = millis();
  _currentState = POWER_ACTIVE;
  _currentContrast = 255;
}

void PowerManager::resetActivity(unsigned long currentMillis) {
  _lastActivityTime = currentMillis;
  _currentState = POWER_ACTIVE;
  _currentContrast = 255;
}

PowerState PowerManager::getState() const {
  return _currentState;
}
bool PowerManager::isSleeping() const {
  return _currentState == POWER_SLEEP;
}

void PowerManager::setOledBrightness(Adafruit_SSD1306& display, uint8_t contrast) {
  if (_currentContrast != contrast) {
    _currentContrast = contrast;
    display.ssd1306_command(SSD1306_SETCONTRAST);
    display.ssd1306_command(contrast);
  }
}

void PowerManager::update(unsigned long currentMillis, Adafruit_SSD1306& display) {
  const unsigned long idleTime = currentMillis - _lastActivityTime;

  if (idleTime < TIMEOUT_DIM_START_MS) {
    if (_currentState != POWER_ACTIVE) {
      _currentState = POWER_ACTIVE;
      setOledBrightness(display, 255);
    }
  } else if (idleTime < TIMEOUT_SLEEP_TOTAL_MS) {
    _currentState = POWER_DIMMING;
    const unsigned long dimElapsed = idleTime - TIMEOUT_DIM_START_MS;

    const float linearProgress = (float)dimElapsed / (float)DURATION_DIM_FADE_MS;
    const float gammaProgress = powf(1.0f - linearProgress, 2.2f);

    uint8_t targetContrast = static_cast<uint8_t>(254.0f * gammaProgress) + 1;
    setOledBrightness(display, targetContrast);
  } else {
    if (_currentState != POWER_SLEEP) {
      _currentState = POWER_SLEEP;
      display.ssd1306_command(SSD1306_DISPLAYOFF);
    }
  }
}

void PowerManager::prepareForSleep() {

}