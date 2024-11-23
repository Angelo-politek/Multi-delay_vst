#ifndef __DELAY_HPP__
#define __DELAY_HPP__

#include <juce_audio_basics/juce_audio_basics.h>

class Delay
{
private:
    juce::AudioBuffer<float> _delay;
    int _read_i, _write_i_sx, _write_i_dx;

    double _sample_rate;

    int _max_delay;
    float _dry_wet;
    float _feedback;


public:
    Delay();

    bool _sync_enable;

    void prepare(double sample_rate, int max_num_samples);
    void process(juce::AudioBuffer<float>& samples);
    void reset();

    void set_delay_dx_in_ms(float delay_in_ms);
    void set_delay_sx_in_ms(float delay_in_ms);
    void set_dry_wet(float value);
    void set_feedback(float feedback);
    void enable_sync(bool enable);
};

#endif // __DELAY_HPP__