#include <M5StickCPlus.h>

#include "WiFiManager.h"
#include "esp32-hal-cpu.h"
#include "prefsManager.h"
#include "timeManager.h"

TFT_eSprite tftSprite = TFT_eSprite(&M5.Lcd);

void setup() {
  M5.begin();
  setCpuFrequencyMhz(80);

  M5.Lcd.setTextSize(2);
  M5.Lcd.setRotation(1);
  M5.Axp.ScreenBreath(9);

  initWiFi();

  M5.Lcd.setCursor(10, 70);
  M5.Lcd.println("Configuring clock");
  timeManagerbegin();
  disconnectWiFi();

  setCpuFrequencyMhz(40);

  tftSprite.createSprite(240, 135);
  tftSprite.setTextFont(1);
  tftSprite.setTextColor(TFT_WHITE);
}

void loop() {
  RTC_TimeTypeDef timeNow = getTime();
  RTC_DateTypeDef dateNow = getDate();

  String minutes = String(timeNow.Minutes);
  String hours = String(timeNow.Hours);

  String day = String(dateNow.Date);
  String month = String(dateNow.Month);

  if (timeNow.Hours < 10) {
    hours = "0" + String(timeNow.Hours);
  }

  if (timeNow.Minutes < 10) {
    minutes = "0" + String(timeNow.Minutes);
  }

  if (dateNow.Date < 10) {
    day = "0" + String(dateNow.Date);
  }

  if (dateNow.Month < 10) {
    month = "0" + String(dateNow.Month);
  }

  String ddmmyyyy = day + "/" + month + "/" + String(dateNow.Year);

  String timeToPrint = hours + ":" + minutes;

  tftSprite.fillSprite(TFT_BLACK);
  tftSprite.setTextSize(7);
  tftSprite.drawString(timeToPrint, 15, 20, 1);
  tftSprite.setTextSize(3);
  tftSprite.drawString(ddmmyyyy, 30, 80, 1);
  tftSprite.pushSprite(0, 0);

  delay(1000);
}
