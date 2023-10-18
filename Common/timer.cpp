#include "timer.h"

#ifndef USE_WINDOWS_API_TIMER

StopWatch::StopWatch()
{
    currTime = std::chrono::high_resolution_clock::now();
    prevTime = std::chrono::high_resolution_clock::now();
}

StopWatch::~StopWatch()
{

}

void StopWatch::startTimer()
{
    prevTime = std::chrono::high_resolution_clock::now();
}

double StopWatch::getElapsedTime()
{
    currTime = std::chrono::high_resolution_clock::now();
    auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(currTime-prevTime).count();
    prevTime = currTime;
    double deltaTime = delta / 1000.0;
    return deltaTime;
}

void StopWatch::waitUntil(long long int time)
{
    Timepoint nowTime = std::chrono::high_resolution_clock::now();
    while (true)
    {
        auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime-prevTime).count();
        if (delta > time)
            break;
        nowTime = std::chrono::high_resolution_clock::now();
    }
}

#else

StopWatch::StopWatch()
{    
    QueryPerformanceFrequency( &frequency ) ;

    #define TARGET_RESOLUTION 1         // 1-millisecond target resolution
    TIMECAPS tc;
    

    if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR) 
    {
        // Error; application can't continue.
    }

    wTimerRes = min(max(tc.wPeriodMin, TARGET_RESOLUTION), tc.wPeriodMax);
    timeBeginPeriod(wTimerRes); 
}

StopWatch::~StopWatch()
{
    timeEndPeriod(wTimerRes);
}

double StopWatch::LIToSecs( LARGE_INTEGER & L) {
     return ((double)L.QuadPart /(double)frequency.QuadPart) ;
 }
 
void StopWatch::startTimer( )
{
    QueryPerformanceCounter(&prevTime) ;
}
 
double StopWatch::getElapsedTime() 
{
    LARGE_INTEGER time;
    QueryPerformanceCounter(&currTime) ;
    time.QuadPart = currTime.QuadPart - prevTime.QuadPart;
    prevTime = currTime;
    return LIToSecs( time) ;
}

void StopWatch::waitUntil(long long time)
{
    LARGE_INTEGER nowTime;
    LONGLONG timeElapsed;
    while (true)
    {
        QueryPerformanceCounter(&nowTime);
        timeElapsed = ((nowTime.QuadPart - prevTime.QuadPart) * 1000 / frequency.QuadPart);
        if (timeElapsed > time)
            return;
        else if (time - timeElapsed > 1)
            Sleep(1);
    }
}

#endif
