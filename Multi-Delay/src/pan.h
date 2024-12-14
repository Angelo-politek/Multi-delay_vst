#ifndef __PAN_HPP__
#define __PAN_HPP__

#include <juce_audio_basics/juce_audio_basics.h>

class Pan
{
private:
    float _pan;            // -1.0 = full left, 0.0 = center, +1.0 = full right
    double _sample_rate;   // Audio sample rate
    int _max_num_samples;  // Maximum number of samples for processing

public:
    Pan();

    void prepare(double sample_rate, int max_num_samples);
    void process(juce::AudioBuffer<float>& buffer);
    void reset();

    void set_pan(float pan);
};

#endif // __PAN_HPP__