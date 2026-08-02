#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include <Wire.h>

class AngleSensor {
public:
  AngleSensor();

  bool begin();
  void update();
  void configureInterrupt();

  float getRawAngle() const;
  float getFilteredAngle() const;
  bool isLevel() const;

private:
  void calibrateAccelerometer();

  int32_t _offsetAx;
  int32_t _offsetAy;
  int32_t _offsetAz;
  float calculatePitch(float ax, float ay, float az) const;

  float _rawAngle;
  float _filteredAngle;
  constexpr static float _alpha = 0.08f;
};

#endif  // SENSOR_H