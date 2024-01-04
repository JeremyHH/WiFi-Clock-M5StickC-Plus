#include <M5StickCPlus2.h>

#include "WiFiManager.h"
#include "esp32-hal-cpu.h"
#include "prefsManager.h"
#include "timeManager.h"

#define IDLE_BRIGHTNESS_LEVEL_PERCENT 10
#define ACTIVE_BRIGHTNESS_LEVEL_PERCENT 100

static rtc_date_t previousDate;
static rtc_time_t previousTime;

void setup()
{
  M5.begin();
  setCpuFrequencyMhz(80);

  M5.Lcd.setTextSize(2);
  M5.Lcd.setRotation(1);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);

  M5.Lcd.setBrightness(255 * IDLE_BRIGHTNESS_LEVEL_PERCENT / 100);

  initWiFi();

  M5.Lcd.setCursor(0, 0, 2);
  M5.Lcd.setCursor(10, 70);
  M5.Lcd.println("Configuring clock");
  timeManagerbegin();
  disconnectWiFi();

  setCpuFrequencyMhz(40);

  M5.Lcd.setTextColor(TFT_WHITE);
  Serial.updateBaudRate(115200);
}

void loop()
{

  //@Todo: Increase brightness if main button has been pressed

  /***************/
  /* time update */
  /***************/

  rtc_time_t timeNow = getTime();

  bool newTime = ((timeNow.hours != previousTime.hours) || (timeNow.minutes != previousTime.minutes));

  previousTime = timeNow;

  /***************/
  /* date update */
  /***************/

  rtc_date_t dateNow = getDate();

  bool newDate = ((dateNow.date != previousDate.date)
                  || (dateNow.month != previousDate.month)
                  || (dateNow.year != previousDate.year));
  previousDate = dateNow;

  // Screen update

  if (newTime || newDate)
  {
    M5.Lcd.fillScreen(TFT_BLACK);

    if (newTime)
    {
      String minutes = String(timeNow.minutes);
      String hours = String(timeNow.hours);

      if (timeNow.hours < 10)
      {
        hours = "0" + String(timeNow.hours);
      }

      if (timeNow.minutes < 10)
      {
        minutes = "0" + String(timeNow.minutes);
      }

      String timeToPrint = hours + ":" + minutes;
      M5.Lcd.setTextSize(7);
      M5.Lcd.drawString(timeToPrint, 15, 20, 1);
    }

    if (newDate)
    {
      String day = String(dateNow.date);
      if (dateNow.date < 10)
      {
        // Leading 0
        day = "0" + String(dateNow.date);
      }

      String month = String(dateNow.month);
      if (dateNow.month < 10)
      {
        // Leading 0
        month = "0" + String(dateNow.month);
      }      

      String ddmmyyyy = day + "/" + month + "/" + String(dateNow.year);
      M5.Lcd.setTextSize(3);
      M5.Lcd.drawString(ddmmyyyy, 30, 80, 1);
    }

    Serial.write("Time update\n");
  }

  delay(100);
}
