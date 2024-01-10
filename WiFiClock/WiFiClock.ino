#include <M5StickCPlus2.h>

#include "WiFiManager.h"
#include "esp32-hal-cpu.h"
#include "prefsManager.h"
#include "timeManager.h"
#include "Pictures/day_pict.h"
#include "Pictures/night_pict.h"

#define IDLE_BRIGHTNESS_LEVEL_PERCENT 5
#define ACTIVE_BRIGHTNESS_LEVEL_PERCENT 80
#define ACTIVE_DURATION_S 10
#undef SHOW_DAY_TIME

const rtc_time_t night_time(20, 00);
const rtc_time_t day_time(07, 30);

static app_state_t appState = STATE_UNKNOWN;
static unsigned long buttonPressedTimestamp = 0;
static bool active = false;
static bool night = false;
static rtc_date_t currentDate;
static rtc_time_t currentTime;

void displayDayPicture()
{
  M5.Lcd.setSwapBytes(true);
  M5.Lcd.pushImage(0, 0, M5.Lcd.width(), M5.Lcd.height(), day_pict);
  M5.Lcd.setSwapBytes(false);
}

void displayNightPicture()
{
  M5.Lcd.setSwapBytes(true);
  M5.Lcd.pushImage(0, 0, M5.Lcd.width(), M5.Lcd.height(), night_pict);
  M5.Lcd.setSwapBytes(false);
}

String dateToString(const rtc_date_t &newDate)
{
  String day = String(newDate.date);
  if (newDate.date < 10)
  {
    // Leading 0
    day = "0" + String(newDate.date);
  }

  String month = String(newDate.month);
  if (newDate.month < 10)
  {
    // Leading 0
    month = "0" + String(newDate.month);
  }

  String ddmmyyyy = day + "/" + month + "/" + String(newDate.year);

  return ddmmyyyy;
}

String timeToString(const rtc_time_t &newTime)
{
  String minutes = String(newTime.minutes);
  String hours = String(newTime.hours);

  if (newTime.hours < 10)
  {
    hours = "0" + String(newTime.hours);
  }

  if (newTime.minutes < 10)
  {
    minutes = "0" + String(newTime.minutes);
  }

  String timeToPrint = hours + ":" + minutes;

  return timeToPrint;
}

void logDateAndTime(const rtc_date_t &newDate, const rtc_time_t &newTime)
{
  // Serial output
  Serial.write(" ");
  Serial.write(dateToString(newDate).c_str());
  Serial.write(" ");
  Serial.write(timeToString(newTime).c_str());
  Serial.write("\n");
}

void displayDateAndTime(const rtc_date_t &newDate, const rtc_time_t &newTime)
{
  // M5.Lcd.fillScreen(lgfx::color888(0xFF, 0xB2, 0x1D));
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setTextSize(7);
  M5.Lcd.drawString(timeToString(newTime), 15, 20, 1);
  M5.Lcd.setTextSize(3);
  M5.Lcd.drawString(dateToString(newDate), 30, 80, 1);
}

void logNewState(const app_state_t newState)
{
  // log new state
  Serial.write("New state : ");
  switch (newState)
  {
  case STATE_DAY_IDLE:
    Serial.write("STATE_DAY_IDLE");
    break;
  case STATE_DAY_ACTIVE:
    Serial.write("STATE_DAY_ACTIVE");
    break;
  case STATE_NIGHT_IDLE:
    Serial.write("STATE_NIGHT_IDLE");
    break;
  case STATE_NIGHT_ACTIVE:
    Serial.write("STATE_NIGHT_ACTIVE");
    break;
  }
  Serial.write("\n");
}

void setIdleBrightness()
{
  M5.Lcd.setBrightness(255 * IDLE_BRIGHTNESS_LEVEL_PERCENT / 100);
}

void setActiveBrightness()
{
  M5.Lcd.setBrightness(255 * ACTIVE_BRIGHTNESS_LEVEL_PERCENT / 100);
}

void handleStateChange(bool force = false)
{
  // Determine the new state according to the night and active info
  app_state_t newState;

  if (night)
  {
    if (active)
    {
      newState = STATE_NIGHT_ACTIVE;
    }
    else
    {
      newState = STATE_NIGHT_IDLE;
    }
  }
  else
  {
    if (active)
    {
      newState = STATE_DAY_ACTIVE;
    }
    else
    {
      newState = STATE_DAY_IDLE;
    }
  }

  if ((newState == STATE_UNKNOWN) || ((newState == appState) && !force))
  {
    return;
  }

  switch (newState)
  {
  case STATE_DAY_IDLE:
    displayDayPicture();
    setActiveBrightness();
    if (appState != STATE_DAY_ACTIVE)
    {
      M5.Power.setBatteryCharge(true);
    }
    break;
  case STATE_DAY_ACTIVE:
    displayDateAndTime(currentDate, currentTime);
    setActiveBrightness();
    if (appState != STATE_DAY_IDLE)
    {
      M5.Power.setBatteryCharge(true);
    }
    break;
  case STATE_NIGHT_IDLE:
    // Display the night picture
    if (appState != STATE_NIGHT_ACTIVE)
    {
      M5.Power.setBatteryCharge(false);
      setIdleBrightness();
    }
    displayNightPicture();
    break;
  case STATE_NIGHT_ACTIVE:
    if (appState != STATE_NIGHT_IDLE)
    {
      M5.Power.setBatteryCharge(false);
      setIdleBrightness();
    }
    displayDateAndTime(currentDate, currentTime);
    break;
  }

  logNewState(newState);
  appState = newState;
}

bool handleTimeUpdate()
{
  app_state_t newState;

  logDateAndTime(currentDate, currentTime);

  // Are we day or night
  t_t1_t2_compare_res_t t_t1_t2_res = t_t1_t2_compareTime(currentTime, day_time, night_time);

  if ((t_t1_t2_res == T_BEFORE_T1) || (t_t1_t2_res == T_AFTER_T2))
  {
    if (!night)
    {
      night = true;
      handleStateChange();
      return true;
    }
  }
  else
  {
    if (night)
    {
      night = false;
      handleStateChange();
      return true;
    }
  }

  return false;
}

bool handleButton()
{
  static unsigned long buttonPressedTimestamp = 0;

  if (M5.BtnA.isPressed())
  {
    // update button pressed last timestamp
    buttonPressedTimestamp = millis();
    if (!active)
    {
      active = true;
      handleStateChange();
      return true;
    }
  }
  else
  {
    unsigned long currentTimestamp = millis();
    if ((currentTimestamp - buttonPressedTimestamp) > (ACTIVE_DURATION_S * 1000))
    {
      if (active)
      {
        active = false;
        handleStateChange();
        return true;
      }
    }
  }

  return false;
}

bool handleSystemEvents()
{
  static rtc_date_t previousDate;
  static rtc_time_t previousTime;
  bool stateChange = false;

  // Handle button pressed
  stateChange |= handleButton();

  // Update date and time
  currentDate = getDate();
  currentTime = getTime();

  bool newTime = ((currentTime.hours != previousTime.hours) || (currentTime.minutes != previousTime.minutes));
  bool newDate = ((currentDate.date != previousDate.date) || (currentDate.month != previousDate.month) || (currentDate.year != previousDate.year));

  // Handle time change
  if (newDate || newTime)
  {
    stateChange |= handleTimeUpdate();
    previousDate = currentDate;
    previousTime = currentTime;
  }

  return stateChange;
}

void setup()
{
  // Hardware init
  M5.begin();
  setCpuFrequencyMhz(80);

  // LCD Init
  M5.Lcd.setTextSize(2);
  M5.Lcd.setRotation(3);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  setIdleBrightness();

  // Wifi init
  initWiFi();

  M5.Lcd.setCursor(0, 0, 2);
  M5.Lcd.setCursor(10, 70);
  M5.Lcd.setTextColor(TFT_WHITE);

  // Set RTC from NTP and then disconnect from wifi
  timeManagerbegin();
  disconnectWiFi();

  // Reduce CPU speed
  setCpuFrequencyMhz(40);
  Serial.updateBaudRate(115200);

  // Initial setting of current state
  if (!handleSystemEvents())
  {
    handleStateChange(true);
  }
}

void loop()
{
  // Update button readings
  M5.update();

  handleSystemEvents();

  // Check if button has been pressed
  delay(20);
}
