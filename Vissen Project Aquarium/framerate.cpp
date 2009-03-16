/* vim: set et sts=4 sw=4: */

#include "framerate.hpp"
#ifdef WIN32
# include <windows.h>
#else
# include <unistd.h>
# include <sys/times.h>

static inline void Sleep(unsigned int msecs)
{
    usleep(msecs * 1000);
}

static inline unsigned int GetTickCount()
{
    tms tm;
    clock_t ticks = times(&tm);

    return ticks * 1000 / sysconf(_SC_CLK_TCK);
}
#endif

FrameRateManager::FrameRateManager(unsigned int rate) :
    _totalFrameCount(0),
    _avgFrameCount(0),
    _targetRate(rate),
    _targetTime(1000.f / static_cast<float>(_targetRate)),
    _lastFrameTicks(::GetTickCount()),
    _avgFrameTicks(_lastFrameTicks)
{
}

void FrameRateManager::reset()
{
    _totalFrameCount = 0;
    _avgFrameTicks = 0;
    _lastFrameTicks = ::GetTickCount();
    _avgFrameTicks = _lastFrameTicks;
}

void FrameRateManager::targetRate(unsigned int rate)
{
    _avgFrameCount = 0;
    _targetRate = rate;
    _targetTime = 1000.f / static_cast<float>(_targetRate);
    _lastFrameTicks = ::GetTickCount();
}

unsigned FrameRateManager::targetRate() const
{
    return _targetRate;
}

unsigned int FrameRateManager::frameCount() const
{
    return _totalFrameCount;
}

float FrameRateManager::countTime() const
{
    // Compute the total time elapsed in seconds
    return static_cast<float>(::GetTickCount() - _avgFrameTicks) / 1000.f;
}

float FrameRateManager::avgFrameRate() const
{
    // Compute the amount of frames per seconds (i.e. framerate expressed in Hz)
    return static_cast<float>(_totalFrameCount) / countTime();
}

float FrameRateManager::recentAvgFrameRate() const
{
    // Compute the amount of frames per seconds (i.e. framerate expressed in Hz)
    return static_cast<float>(_avgFrameCount) / static_cast<float>(::GetTickCount() - _lastFrameTicks) * 1000.f;
}

int FrameRateManager::computeFrameRateDelay()
{
    // First move to the next frame
    ++_totalFrameCount;
    ++_avgFrameCount;

    // Retrieve the current time/tick count
    const int current_ticks = ::GetTickCount();

    // Compute the time point at which this frame should end
    const int target_ticks = _lastFrameTicks + static_cast<unsigned int>(static_cast<float>(_avgFrameCount) * _targetTime);

    // Check if we're not passed this point in time yet
    if (current_ticks > target_ticks)
    {
        /* If we're past that time already, reset the frame and tick counters.
         * This way we won't be eating all CPU time after a short laps in our
         * frame rate.
         */
        _avgFrameCount = 0;
        _lastFrameTicks = ::GetTickCount();
        return -1;
    }

    return target_ticks - current_ticks;
}

void FrameRateManager::frameRateDelay()
{
    // Compute the delay
    int const delay = computeFrameRateDelay();

    // Don't delay if we are short on processing time already
    if (delay < 0)
        return;

    // Yield processing time to other threads and processes
    ::Sleep(delay);
}
