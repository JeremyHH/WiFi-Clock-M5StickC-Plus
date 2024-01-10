#ifndef TIMEMANAGER
#define TIMEMANAGER

#include <M5Unified.h>

using namespace m5;

typedef enum 
{
  STATE_UNKNOWN,
  STATE_DAY_IDLE,
  STATE_DAY_ACTIVE,
  STATE_NIGHT_IDLE,
  STATE_NIGHT_ACTIVE
}  app_state_t;

typedef enum {
  T_BEFORE_T1,
  T_EQUAL_T1,
  T_BETWEEN_T1_T2,
  T_EQUAL_T2,
  T_AFTER_T2
} t_t1_t2_compare_res_t;

typedef enum {
  T1_BEFORE_T2,
  T1_EQUAL_T2,
  T1_AFTER_T2
} t1_t2_compare_res_t;

void timeManagerbegin();
void syncRTCToNTP();
time_t getTimestampFromRTC();
rtc_time_t getTime();
rtc_date_t getDate();

t1_t2_compare_res_t t1_t2_compareTime(const rtc_time_t& t1,
                                     const rtc_time_t& t2);

t_t1_t2_compare_res_t t_t1_t2_compareTime(const rtc_time_t& t,
                                      const rtc_time_t& t1,
                                      const rtc_time_t& t2);

unsigned int t2_minus_t1(const rtc_time_t &t1, const rtc_time_t &t2);

#endif
