#include <ESP8266WiFi.h>
#include "config.h"
#include "battery.h"
#include "sensor.h"
#include "animation.h"
#include "sleep.h"
#include "display.h"

extern "C" {
#include "user_interface.h"
}

AngleSensor sensor;
DisplayUI gui;
BatteryMonitor battery;
WakeAnimation anim;
PowerManager power;

volatile bool motionDetected = false;

void IRAM_ATTR handleMotionInterrupt() {
  motionDetected = true;
}

void setup() {

  delay(2000);  // ESP8266 Stable হওয়ার সময়

  Serial.begin(115200);
  delay(100);

  Serial.println("==============================");
  Serial.println("1. Setup Start");

  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
  delay(1);

  Serial.println("2. WiFi OFF OK");

  pinMode(PIN_MPU_INT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_MPU_INT), handleMotionInterrupt, FALLING);

  Serial.println("3. Interrupt OK");

  battery.begin();
  Serial.println("4. Battery OK");

  Serial.println("5. Starting GUI...");

  if (!gui.begin()) {
    Serial.println("ERROR: GUI INIT FAILED");
    while (true) {
      delay(100);
    }
  }

  Serial.println("6. GUI OK");

  Serial.println("7. Starting Sensor...");

  if (!sensor.begin()) {
    Serial.println("ERROR: SENSOR INIT FAILED");

    gui.getDriver().clearDisplay();
    gui.getDriver().setTextSize(1);
    gui.getDriver().setCursor(20, 28);
    gui.getDriver().print("SENSOR ERROR");
    gui.getDriver().display();

    while (true) {
      delay(100);
    }
  }

  Serial.println("8. Sensor OK");

  anim.start();
  Serial.println("9. Animation Started");

  power.begin();
  Serial.println("10. Power Manager OK");

  Serial.println("===== SETUP COMPLETE =====");
}

void loop() {
  const unsigned long currentMillis = millis();

  if (motionDetected) {
    Serial.println("MOTION DETECTED");
    motionDetected = false;
    power.resetActivity(currentMillis);

    if (power.isSleeping()) {

      sensor.update();

      static float lastWakeAngle = sensor.getRawAngle();

      if (fabs(sensor.getRawAngle() - lastWakeAngle) >= 1.0f) {

        lastWakeAngle = sensor.getRawAngle();

        power.resetActivity(currentMillis);

        gui.getDriver().ssd1306_command(SSD1306_DISPLAYON);

        anim.start();
      }

      return;
    }
  }

  power.update(currentMillis, gui.getDriver());

  if (power.isSleeping()) {
    power.prepareForSleep();
    //wifi_fpm_do_post_sleep_work();
    return;
  }

  if (!anim.isFinished()) {
    anim.update(currentMillis, gui.getDriver());
    return;
  }

  static unsigned long lastSensorRead = 0;
  if (currentMillis - lastSensorRead >= INTERVAL_SENSOR_MS) {
    lastSensorRead = currentMillis;
    sensor.update();

    static float lastActiveAngle = 0.0f;
    if (fabs(sensor.getRawAngle() - lastActiveAngle) >= 0.1f) {
      lastActiveAngle = sensor.getFilteredAngle();
      power.resetActivity(currentMillis);
    }
  }

  battery.update(currentMillis);

  static unsigned long lastNeedleFrame = 0;
  if (currentMillis - lastNeedleFrame >= INTERVAL_NEEDLE_MS) {
    lastNeedleFrame = currentMillis;
    gui.render(
      sensor.getRawAngle(),
      sensor.isLevel(),
      battery);
  }
}