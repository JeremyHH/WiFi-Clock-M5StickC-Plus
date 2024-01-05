#include <M5StickCPlus2.h>

#include "WiFiManager.h"
#include "esp32-hal-cpu.h"
#include "prefsManager.h"
#include "timeManager.h"
#include "Pictures/day_pict.h"
#include "Pictures/night_pict.h"

#define IDLE_BRIGHTNESS_LEVEL_PERCENT 5
#define ACTIVE_BRIGHTNESS_LEVEL_PERCENT 100

const rtc_time_t  night_time(20, 00);
const rtc_time_t  day_time(07, 30);

void setup()
{
  // Hardware init
  M5.begin();
  setCpuFrequencyMhz(80);

  // LCD Init
  M5.Lcd.setTextSize(2);
  M5.Lcd.setRotation(1);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.setBrightness(255 * IDLE_BRIGHTNESS_LEVEL_PERCENT / 100);

  // Wifi init
  initWiFi();

  M5.Lcd.setCursor(0, 0, 2);
  M5.Lcd.setCursor(10, 70);
  M5.Lcd.setTextColor(TFT_WHITE);
  M5.Lcd.println("Configuring clock");

  // Set RTC from NTP and then disconnect from wifi 
  timeManagerbegin();
  disconnectWiFi();

  // Reduce CPU speed
  setCpuFrequencyMhz(40);
  Serial.updateBaudRate(115200);
}

void loop()
{
  static rtc_date_t previousDate;
  static rtc_time_t previousTime;

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

    // Are we day or night
    t_t1_t2_compare_res_t t_t1_t2_res = t_t1_t2_compareTime(timeNow, day_time, night_time); 
    
    M5.Lcd.setSwapBytes(true);
    if ((t_t1_t2_res == T_BEFORE_T1) || (t_t1_t2_res == T_AFTER_T2))
    {
        // Display the night picture
        M5.Lcd.pushImage(0, 0, M5.Lcd.width(), M5.Lcd.height() , night_pict);
        Serial.write("[N]");
    }
    else
    {
        // Display the day picture
        M5.Lcd.pushImage(0, 0, M5.Lcd.width(), M5.Lcd.height() , day_pict);
        Serial.write("[D]");

        /*****************/
        /* Screen update */
        /*****************/

        M5.Lcd.fillScreen(TFT_BLACK);
        M5.Lcd.setTextSize(7);
        M5.Lcd.drawString(timeToPrint, 15, 20, 1);
        M5.Lcd.setTextSize(3);
        M5.Lcd.drawString(ddmmyyyy, 30, 80, 1);
    }
    M5.Lcd.setSwapBytes(false);

    Serial.write(" ");
    Serial.write(ddmmyyyy.c_str());
    Serial.write(" ");
    Serial.write(timeToPrint.c_str());
    Serial.write("\n");
  }

  delay(100);
}
