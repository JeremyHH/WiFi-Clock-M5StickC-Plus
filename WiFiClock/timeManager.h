#ifndef TIMEMANAGER
#define TIMEMANAGER

#include <M5Unified.h>

using namespace m5;

void timeManagerbegin();
void syncRTCToNTP();
time_t getTimestampFromRTC();
rtc_time_t getTime();
rtc_date_t getDate();

#endif
