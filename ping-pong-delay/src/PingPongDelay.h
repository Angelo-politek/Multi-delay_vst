#ifndef __PING_PONG_DELAY_HPP__
#define __PING_PONG_DELAY_HPP__

#include <juce_audio_basics/juce_audio_basics.h>

class PingPongDelay
{
public:
    enum Mode
    {
        mode_center = 0,
        mode_left = 1,
        mode_right = 2,
    };

private:
    juce::AudioBuffer<float> _delay;
    int _read_i, _write_i;

    double _sample_rate;
    int _max_delay;

    float _feedback;
    float _dry_wet;

    Mode _mode;

public:
    PingPongDelay();

    void prepare(double sample_rate, int max_samples);
    void reset();
    void process(juce::AudioBuffer<float> &buffer);

    void set_delay_in_ms(double delay_in_ms);
    void set_mode(int mode);
    void set_dry_wet(float dry_wet);
    void set_feedback(float feedback);
};

#endif // __PING_PONG_DELAY_HPP__