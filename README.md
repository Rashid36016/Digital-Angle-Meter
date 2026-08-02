# Digital Angle Meter

A portable digital angle meter built using the **ESP8266 NodeMCU V3**, **MPU6050** and **SSD1306 OLED Display**. The device provides smooth and accurate angle measurement with a modern user interface, battery monitoring, automatic power-saving mode, and an animated startup screen.

---

## Features

- Real-time angle measurement
- Smooth needle animation
- Digital angle display
- LEVEL indication
- Battery level indicator
- Automatic screen dimming
- Automatic screen off after inactivity
- Wake-up on movement
- Boot animation
- Low power consumption
- Portable battery-powered design

---

## Hardware Used

- ESP8266 NodeMCU V3
- MPU6050 6-Axis Accelerometer & Gyroscope
- SSD1306 128×64 I2C OLED Display
- 18650 Li-ion Battery
- Battery Spring Contact
- Slide Switch
- 10kΩ Resistor
- 22kΩ Resistor
- Connecting Wires
- 3D Printed Case

---

## Pin Connections

| Device        | ESP8266 Pin |
|---------------|-------------|
| MPU6050 SDA   | D2 (GPIO4)  |
| MPU6050 SCL   | D1 (GPIO5)  |
| MPU6050 INT   | D5 (GPIO14) |
| OLED SDA      | D2 (GPIO4)  |
| OLED SCL      | D1 (GPIO5)  |
| Battery (+ve) | VIN         |
| Battery (-ve) | GND         |
| 22kΩ Resistor | VIN to A0   |
| 10kΩ Resistor | GND to A0   |

---

## Battery Monitoring

The battery voltage is measured through a voltage divider.

- R1 = 10kΩ
- R2 = 22kΩ

---

## Project Structure

```
Digital-Angle-Meter
│
├── Code
├── Circuit
├── Components
├── Project_Photos
├── 3D_Print
├── README.md
└── LICENSE
```

---

## Software and Libraries

- Arduino IDE
- ESP8266 Board Package
- Adafruit SSD1306 Library
- Adafruit GFX Library
- Wire Library

---

## Power Saving

- OLED automatically dims after inactivity.
- Display turns off to save battery.
- Motion automatically wakes the display.

---

## 3D Printed Case

A custom-designed 3D printed Case is included in this repository.

---

## Future Improvements

- Calibration mode
- Buzzer feedback
- Wi-Fi firmware update
- Auto display rotation
- OLED themes

---

## Author

**Rashid Ahmmed**

---

## License

This project is released under the **MIT License**.
