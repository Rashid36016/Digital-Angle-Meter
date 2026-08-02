#include "sensor.h"
#include "config.h"

constexpr uint8_t MPU6050_ADDR = 0x68;
constexpr uint8_t MPU6050_PWR_MGMT_1 = 0x6B;
constexpr uint8_t MPU6050_ACCEL_XOUT_H = 0x3B;
constexpr uint8_t MPU6050_INT_ENABLE = 0x38;
constexpr uint8_t MPU6050_INT_PIN_CFG = 0x37;
constexpr uint8_t MPU6050_MOT_THR = 0x1F;
constexpr uint8_t MPU6050_MOT_DUR = 0x20;

AngleSensor::AngleSensor()
  : _rawAngle(0.0f), _filteredAngle(0.0f), _offsetAx(0), _offsetAy(0), _offsetAz(0) {
}
bool AngleSensor::begin() {

  Serial.println("======================");
  Serial.println("Sensor Begin");

  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
  Serial.println("Wire.begin OK");

  delay(300);

  Wire.setClock(100000);
  Serial.println("I2C Clock = 100kHz");

  delay(300);

  Serial.println();
  Serial.println("===== I2C Scan =====");

  byte deviceCount = 0;

  for (byte addr = 1; addr < 127; addr++) {

    Wire.beginTransmission(addr);

    if (Wire.endTransmission() == 0) {

      Serial.print("Found: 0x");

      if (addr < 16)
        Serial.print("0");

      Serial.println(addr, HEX);

      deviceCount++;
    }
  }

  Serial.print("Devices Found: ");
  Serial.println(deviceCount);
  Serial.println();

  byte err = 4;

  Serial.println("Waking MPU6050...");

  for (byte i = 0; i < 5; i++) {

    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(MPU6050_PWR_MGMT_1);
    Wire.write(0x00);

    err = Wire.endTransmission();

    Serial.print("Try ");
    Serial.print(i + 1);
    Serial.print(" -> Error = ");
    Serial.println(err);

    if (err == 0)
      break;

    delay(100);
  }

  Serial.print("Wake Error Code = ");
  Serial.println(err);

  if (err != 0) {
    Serial.println("ERROR: Cannot communicate with MPU6050");
    return false;
  }

  Serial.println("MPU Wake Success");

  Serial.println("Configuring Interrupt...");

  configureInterrupt();

  Serial.println("Interrupt Config Success");

  Serial.println("Sensor Begin SUCCESS");
  Serial.println("======================");

  delay(100);

  return true;
}

void AngleSensor::configureInterrupt() {
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(MPU6050_INT_PIN_CFG);
  Wire.write(0x20);
  Wire.endTransmission();

  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(MPU6050_MOT_THR);
  Wire.write(0x08);
  Wire.endTransmission();

  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(MPU6050_MOT_DUR);
  Wire.write(0x01);
  Wire.endTransmission();

  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(MPU6050_INT_ENABLE);
  Wire.write(0x40);
  Wire.endTransmission();
}

void AngleSensor::update() {

  static bool firstRun = true;

  if (firstRun) {
    Serial.println("Sensor Update Running");
    firstRun = false;
  }

  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(MPU6050_ACCEL_XOUT_H);
  if (Wire.endTransmission(false) != 0) return;

  Wire.requestFrom(static_cast<uint8_t>(MPU6050_ADDR), static_cast<size_t>(6));
  if (Wire.available() < 6) return;

  const int16_t rawAx = (Wire.read() << 8) | Wire.read();
  const int16_t rawAy = (Wire.read() << 8) | Wire.read();
  const int16_t rawAz = (Wire.read() << 8) | Wire.read();

  const float ax = static_cast<float>(rawAx) / 16384.0f;
  const float ay = static_cast<float>(rawAy) / 16384.0f;
  const float az = static_cast<float>(rawAz) / 16384.0f;



  _rawAngle = calculatePitch(ax, ay, az);






  _filteredAngle = (_filteredAngle * (1.0f - _alpha)) + (_rawAngle * _alpha);
}

float AngleSensor::calculatePitch(float ax, float ay, float az) const {
  return atan2(ay, sqrt(ax * ax + az * az)) * (180.0f / M_PI);
}

float AngleSensor::getRawAngle() const {
  return _rawAngle;
}
float AngleSensor::getFilteredAngle() const {
  return _filteredAngle;
}

bool AngleSensor::isLevel() const {
  return (fabs(_filteredAngle) <= LEVEL_TOLERANCE_DEG);
}