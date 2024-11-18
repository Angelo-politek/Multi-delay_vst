#include "Pan.h"

#define INITIAL_SAMPLE_RATE 44100

Pan::Pan() : _pan{0.0f} {}

void Pan::reset()
{
    _pan = 0.0f; // Reset pan to center
}

void Pan::prepare(double sample_rate, int max_num_samples)
{
    _sample_rate = sample_rate; // Store sample rate, though not directly used
    _max_num_samples = max_num_samples;
}

void Pan::process(juce::AudioBuffer<float>& buffer)
{
    auto numSamples = buffer.getNumSamples();
    auto* leftChannel = buffer.getWritePointer(0);
    auto* rightChannel = buffer.getWritePointer(1);

    // Calculate the pan gain for each channel
    float leftGain = (_pan <= 0.0f) ? 1.0f : (1.0f - _pan);
    float rightGain = (_pan >= 0.0f) ? 1.0f : (1.0f + _pan);

    for (int i = 0; i < numSamples; ++i)
    {
        leftChannel[i] *= leftGain;
        rightChannel[i] *= rightGain;
    }
}

void Pan::set_pan(float pan)
{
    _pan = juce::jlimit(-1.0f, 1.0f, pan); // Clamp pan value to range [-1, 1]
}