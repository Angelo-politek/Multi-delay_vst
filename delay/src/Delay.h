#ifndef __DELAY_HPP__
#define __DELAY_HPP__

#include <juce_audio_basics/juce_audio_basics.h>

class Delay
{
public:
    enum Mode_clr
    {
        mode_center = 0,
        mode_left = 1,
        mode_right = 2,
    };

    enum Mode
    {
        mode_feedback = 0,
        mode_pingpong = 1,
    };

private:
    juce::AudioBuffer<float> _delay;
    int _read_i, _write_i_sx, _write_i_dx;

    double _sample_rate;

    int _max_delay;
    float _dry_wet;
    float _feedback;
    bool _sync_enable;
    
    Mode_clr _mode_pingpong;
    Mode _mode_delay;

public:
    Delay();


    void prepare(double sample_rate, int max_num_samples);
    void process(juce::AudioBuffer<float>& samples);
    void reset();

    void set_delay_dx_in_ms(float delay_in_ms);
    void set_delay_sx_in_ms(float delay_in_ms);
    void set_dry_wet(float value);
    void set_feedback(float feedback);
    void enable_sync(bool enable);

    void set_delay_mode(int mode);
    void set_pingpong_mode(int mode);

};

#endif // __DELAY_HPP__