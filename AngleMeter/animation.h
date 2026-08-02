#ifndef ANIMATION_H
#define ANIMATION_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>

enum AnimState {
  ANIM_IDLE,
  ANIM_SLEEPY,
  ANIM_EYES_OPEN,
  ANIM_BLINK,
  ANIM_LEFT_OPEN,
  ANIM_SMILE,
  ANIM_COMPLETE
};

class WakeAnimation {
public:
  WakeAnimation();

  void start();
  void update(unsigned long currentMillis, Adafruit_SSD1306& display);
  bool isFinished() const;

private:
  void renderFace(Adafruit_SSD1306& display, const char* str);

  AnimState _currentState;
  unsigned long _stateStartTime;
  bool _isFinished;
};

#endif  // ANIMATION_H