#ifndef SLEEP_H
#define SLEEP_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>

enum PowerState {
  POWER_ACTIVE,
  POWER_DIMMING,
  POWER_SLEEP
};

class PowerManager {
public:
  PowerManager();

  void begin();
  void resetActivity(unsigned long currentMillis);
  void update(unsigned long currentMillis, Adafruit_SSD1306& display);

  PowerState getState() const;
  bool isSleeping() const;

  void prepareForSleep();

private:
  void setOledBrightness(Adafruit_SSD1306& display, uint8_t contrast);

  unsigned long _lastActivityTime;
  PowerState _currentState;
  uint8_t _currentContrast;
};

#endif  // SLEEP_H