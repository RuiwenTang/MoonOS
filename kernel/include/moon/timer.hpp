/*******************************************************************************
 * Project: moon-os                                                            *
 * File Created: Monday, 30th November 2020 5:54:36 pm                         *
 * Author: tangruiwen (tangruiwen1989@gmail.com)                               *
 * Copyright - 2020                                                            *
 ******************************************************************************/
#pragma once

#include <stdint.h>

class Timer final {
 public:
  using time_t = long;

  struct TimeVal {
    long seconds;
    long milliseconds;
  };

  struct TimeSpec {
    time_t tv_sec;
    long tv_nsec;
  };

  static Timer* Instance();

  void Initialize(uint32_t freq);

  TimeVal GetSystemUptimeStruct();

  uint64_t GetSystemUptime();
  uint32_t GetTicks();
  uint32_t GetFrequency();

 private:
   static void Handler(void*, void*);

   void AddTicks();
 private:
  uint32_t fFrequency = 0;
  uint32_t fTicks = 0;

  Timer() = default;
  ~Timer() = default;
};

static inline bool operator<(Timer::TimeVal l, Timer::TimeVal r) {
  return (l.seconds < r.seconds) ||
         (l.seconds == r.seconds && l.milliseconds < r.milliseconds);
}