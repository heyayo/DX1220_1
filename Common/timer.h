#ifndef _TIMER_H
#define _TIMER_H

#ifndef USE_WINDOWS_API_TIMER

#include <chrono>

class StopWatch
{
    typedef std::chrono::high_resolution_clock::time_point Timepoint;

    Timepoint prevTime,currTime;

public:
    StopWatch();
    ~StopWatch();
    void startTimer();
    double getElapsedTime();
    void waitUntil(long long time);
};

#else

#include <windows.h>

class StopWatch
{

 private:

    LARGE_INTEGER frequency;
    LARGE_INTEGER prevTime, currTime;
    double LIToSecs( LARGE_INTEGER & L) ;
    UINT     wTimerRes;

 public:
     StopWatch() ;
     ~StopWatch();
     void startTimer();
     double getElapsedTime(); // get time in seconds since the last call to this function
     void waitUntil(long long time);  // wait until this time in milliseconds has passed
 };

#endif

#endif // _TIMER_H