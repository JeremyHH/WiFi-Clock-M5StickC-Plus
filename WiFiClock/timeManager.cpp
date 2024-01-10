#include <M5StickCPlus2.h>
#include <time.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "timeManager.h"

WiFiUDP ntpUDP;

// By default 'pool.ntp.org' is used with 60 seconds update interval and
// no offset
NTPClient timeClient(ntpUDP);

void timeManagerbegin()
{
  M5.Rtc.begin();

  /* Initialize time zone (France) */
  setenv("TZ", "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00", 1);    // Europe/Paris timezone
  tzset();

  timeClient.begin();

  syncRTCToNTP();
}

void syncRTCToNTP()
{
  timeClient.update();

  time_t now = timeClient.getEpochTime();
  struct tm timeinfo = *localtime(&now);

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

time_t getTimestampFromRTC()
{
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

rtc_time_t getTime()
{
  rtc_time_t timeNow;

  M5.Rtc.getTime(&timeNow);

  return timeNow;
}

rtc_date_t getDate()
{
  rtc_date_t dateNow;

  M5.Rtc.getDate(&dateNow);

  return dateNow;
}

unsigned int seconds_from_midnight(const rtc_time_t &t)
{
  return t.hours*60*60 + t.minutes*60 + t.seconds;
}

unsigned int seconds_upto_midnight(const rtc_time_t &t)
{
  return 24*60*60 - seconds_from_midnight(t);
}

unsigned int t2_minus_t1(const rtc_time_t &t1, const rtc_time_t &t2)
{
  t1_t2_compare_res_t res = t1_t2_compareTime(t1, t2);

  switch(res)
  {
    case T1_BEFORE_T2 :
      return seconds_from_midnight(t2) - seconds_from_midnight(t1);
      break;
    case T1_EQUAL_T2 :
      return 0;
      break;
    case T1_AFTER_T2 :
      return seconds_upto_midnight(t1) + seconds_from_midnight(t2);
      break;
  }
}

t1_t2_compare_res_t t1_t2_compareTime(const rtc_time_t &t1,
                                      const rtc_time_t &t2)
{
  if (t1.hours > t2.hours)
  {
    return T1_AFTER_T2;
  }
  else if (t1.hours < t2.hours)
  {
    return T1_BEFORE_T2;
  }
  else if (t1.minutes > t2.minutes)
  {
    return T1_AFTER_T2;
  }
  else if (t1.minutes < t2.minutes)
  {
    return T1_BEFORE_T2;
  }
  else if (t1.seconds > t2.seconds)
  {
    return T1_AFTER_T2;
  }
  else if (t1.seconds < t2.seconds)
  {
    return T1_BEFORE_T2;
  }

  return T1_EQUAL_T2;
}

t_t1_t2_compare_res_t t_t1_t2_compareTime(const rtc_time_t &t,
                                          const rtc_time_t &t1,
                                          const rtc_time_t &t2)
{
  // Compare T with T1
  t1_t2_compare_res_t res_t1 = t1_t2_compareTime(t, t1);
  t1_t2_compare_res_t res_t2 = t1_t2_compareTime(t, t2);

  if (res_t1 == T1_BEFORE_T2)
  {
    return T_BEFORE_T1;
  }
  else if (res_t1 == T1_EQUAL_T2)
  {
    return T_EQUAL_T1;
  }
  else
  {
    if (res_t2 == T1_BEFORE_T2)
    {
      return T_BETWEEN_T1_T2;
    }
    else if (res_t2 == T1_EQUAL_T2)
    {
      return T_EQUAL_T2;
    }
  }

  return T_AFTER_T2;
}
