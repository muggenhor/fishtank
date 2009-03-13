/* vim: set et sts=4 sw=4: */

#ifndef __INCLUDED_FRAMERATE_HPP__
#define __INCLUDED_FRAMERATE_HPP__

class FrameRateManager
{
    public:
        FrameRateManager(unsigned int rate);

        void targetRate(unsigned int rate);
        unsigned int targetRate() const;

        unsigned int frameCount() const;
        float countTime() const;
        float avgFrameRate() const;
        float recentAvgFrameRate() const;

        void frameRateDelay();

    private:
        int computeFrameRateDelay();

    private:
        unsigned int        _totalFrameCount;
        unsigned int        _avgFrameCount;
        unsigned int        _targetRate;
        float               _targetTime;
        unsigned int        _lastFrameTicks;
        const unsigned int  _avgFrameTicks;
};

#endif // __INCLUDED_FRAMERATE_HPP__
