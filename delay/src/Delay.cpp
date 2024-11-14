#include "Delay.h"

#define INITAL_SAMPLE_RATE 44100

Delay::Delay() : _delay{2, INITAL_SAMPLE_RATE}, _read_i{0}, _write_i_sx{1}, _write_i_dx{1}, _sample_rate{INITAL_SAMPLE_RATE}, _max_delay{INITAL_SAMPLE_RATE}, _dry_wet{0.15}, _feedback{0.5}
{
    _delay.clear();
}

void Delay::reset()
{
    _delay.clear();
}

void Delay::prepare(double sample_rate, int max_num_samples)
{
    _sample_rate = sample_rate;
    _max_delay = juce::jmax((int)sample_rate, max_num_samples);

    _delay.setSize(2, _max_delay);
    _delay.clear();
}

void Delay::process(juce::AudioBuffer<float> &buffer)
{
    float *left_channel = buffer.getWritePointer(0);
    float *right_channel = buffer.getWritePointer(1);

    float *left_delay = _delay.getWritePointer(0);
    float *right_delay = _delay.getWritePointer(1);

    for (int i = 0; i < buffer.getNumSamples(); i++, _read_i = (_read_i + 1) % _max_delay, _write_i_sx = (_write_i_sx + 1) % _max_delay, _write_i_dx = (_write_i_dx + 1) % _max_delay)
    {
        // Get output sample
        float out_left = left_channel[i];
        float out_right = right_channel[i];

        // Get delay
        float out_left_delay = left_delay[_read_i];
        float out_right_delay = right_delay[_read_i];

        // Write out samples
        left_channel[i] = out_left * (1.f - _dry_wet) + out_left_delay * _dry_wet;
        right_channel[i] = out_right * (1.f - _dry_wet) + out_right_delay * _dry_wet;

        // Write delay
        left_delay[_write_i_sx] = out_left + out_left_delay * _feedback;
        right_delay[_write_i_dx] = out_right + out_right_delay * _feedback;

        // Clear read positions
        left_delay[_read_i] = 0.f;
        right_delay[_read_i] = 0.f;
    }
}

void Delay::set_delay_sx_in_ms(float delay_in_ms)
{
    int delay_in_samples = juce::jlimit(1, _max_delay, juce::roundToInt(delay_in_ms * _sample_rate / 1000.f));
    _write_i_sx = (_read_i + delay_in_samples) % _max_delay;
}

void Delay::set_delay_dx_in_ms(float delay_in_ms)
{
    int delay_in_samples = juce::jlimit(1, _max_delay, juce::roundToInt(delay_in_ms * _sample_rate / 1000.f));
    _write_i_dx = (_read_i + delay_in_samples) % _max_delay;
}

void Delay::set_feedback(float feedback)
{
    _feedback = juce::jlimit(0.f, 1.f, feedback);
}

void Delay::set_dry_wet(float dry_wet)
{
    _dry_wet = juce::jlimit(0.f, 1.f, dry_wet);
}