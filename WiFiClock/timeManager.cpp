#include "timeManager.h"

#include <M5StickCPlus2.h>
#include <time.h>

const long gmtOffset_sec = 0;
const int daylightOffset_sec = -3600 * 3;
const char* ntpServer = "pool.ntp.org";

void timeManagerbegin() {
  M5.Rtc.begin();

  // Config ntp server with Europe/Paris timezone
  configTzTime("CET-1CEST,M3.5.0,M10.5.0/3", ntpServer);
  
  syncRTCToNTP();
}

void syncRTCToNTP() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return;
  }

  rtc_time_t currentTime;
  currentTime.hours = timeinfo.tm_hour;
  currentTime.minutes = timeinfo.tm_min;
  currentTime.seconds = timeinfo.tm_sec;

  rtc_date_t currentDate;
  currentDate.year = timeinfo.tm_year + 1900;
  currentDate.month = timeinfo.tm_mon + 1;
  currentDate.date = timeinfo.tm_mday;

  M5.Rtc.setTime(&currentTime);
  M5.Rtc.setDate(&currentDate);
}

time_t getTimestampFromRTC() {
  rtc_time_t currentTime;
  rtc_date_t currentDate;
  M5.Rtc.getTime(&currentTime);
  M5.Rtc.getDate(&currentDate);

  struct tm timeinfo;
  timeinfo.tm_year = currentDate.year + 1900;
  timeinfo.tm_mon = currentDate.month - 1;
  timeinfo.tm_mday = currentDate.date;
  timeinfo.tm_hour = currentTime.hours;
  timeinfo.tm_min = currentTime.minutes;
  timeinfo.tm_sec = currentTime.seconds;

  return mktime(&timeinfo);
}

rtc_time_t getTime() {
  rtc_time_t timeNow;

  M5.Rtc.getTime(&timeNow);

  return timeNow;
}

rtc_date_t getDate() {
  rtc_date_t dateNow;

  M5.Rtc.getDate(&dateNow);

  return dateNow;
}
