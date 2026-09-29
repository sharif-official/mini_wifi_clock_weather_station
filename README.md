# Mini Wi-Fi Clock & Weather Station

ESP32 DevKit V1 + 1.3-inch I2C OLED + Wi-Fi + Power Bank.

## Features

- Wi-Fi connection
- Bangladesh/Dhaka local time using NTP
- Online weather using Open-Meteo
- Temperature
- Weather condition
- Wind speed
- 1.3-inch 128x64 I2C OLED
- Portable power-bank operation

## Wiring

| OLED | ESP32 DevKit V1 |
|---|---|
| VCC | 3.3V |
| GND | GND |
| SDA | GPIO 21 |
| SCL | GPIO 22 |

```text
ESP32                 OLED 1.3"
3.3V  ---------------- VCC
GND   ---------------- GND
GPIO21 -------------- SDA
GPIO22 -------------- SCL

Power Bank --USB--> ESP32
```

## Required Arduino libraries

- Adafruit GFX Library
- Adafruit SH110X
- ArduinoJson

## Setup

1. Open `src/Mini_WiFi_Clock_Weather_Station.ino`.
2. Change:
   - `YOUR_WIFI_NAME`
   - `YOUR_WIFI_PASSWORD`
3. If needed, change `LATITUDE` and `LONGITUDE`.
4. Select **ESP32 Dev Module**.
5. Upload.

## Display

```text
┌────────────────────────┐
│ 11:35:42          PM   │
│       29 Sep 2026      │
├────────────────────────┤
│ Temp: 29.4°C           │
│ Clear Sky       8km/h  │
└────────────────────────┘
```

## Notes

This version gets outdoor weather from the internet. It does not measure room temperature locally.

## License

MIT
