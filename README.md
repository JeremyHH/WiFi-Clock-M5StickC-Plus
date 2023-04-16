# WiFi Clock M5StickC Plus

This repository contains the code for a simple WiFi-enabled clock using the M5StickC Plus development board. The project connects to the internet to synchronize time using the NTP (Network Time Protocol) and displays the current time and date on the M5StickC Plus screen. The device connects to WiFi only to obtain the time, then disconnects from WiFi and reduces the CPU clock speed to save power. It also stores the WiFi SSID and password in the persistent storage of the ESP32 to facilitate future connections.

## Features

- Connects to WiFi for time synchronization, then disconnects to save power
- Reduces CPU clock speed after time synchronization to save energy
- Persistent storage of WiFi SSID and password for easy reconnection
- Displays current time in HH:mm format
- Displays current date in dd/mm/yyyy format
- Timezone: GMT-3

## Repository Structure

- `/WiFiClock`: Main project folder
  - `WiFiClock.ino`: Main sketch file for the project
  - `WiFiManager.cpp`: WiFi management functions, including SmartConfig for WiFi setup
  - `prefsManager.cpp`: Preferences manager functions for handling persistent storage of WiFi data
  - `timeManager.cpp`: Time management functions
- `.gitignore`: Git ignore file
- `LICENSE`: Unlicense file

## Getting Started

1. Install the [M5StickC Plus library](https://github.com/m5stack/M5StickC-Plus) for Arduino IDE or PlatformIO.
2. Upload the `WiFiClock.ino` sketch to your M5StickC Plus device.
3. The device will connect to WiFi using stored SSID and password or via SmartConfig, and synchronize time using the NTP server.
4. The device will then disconnect from WiFi and reduce the CPU clock speed to save power.
5. The current time and date will be displayed on the screen.

## License

This project is released under the [Unlicense](https://unlicense.org/), which means it is free and open-source software, and you can use, modify and distribute it without any restrictions.
