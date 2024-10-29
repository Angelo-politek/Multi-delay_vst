#include "PingPongDelay.h"

#define INIT_SAMPLE_RATE 44100

PingPongDelay::PingPongDelay() : _delay{2, INIT_SAMPLE_RATE}, _read_i{0}, _write_i{1}, _sample_rate{INIT_SAMPLE_RATE}, _max_delay{INIT_SAMPLE_RATE}, _feedback{0.5}, _dry_wet{0.15}, _mode{Mode::mode_center} {};

void PingPongDelay::prepare(double sample_rate, int max_samples)
{
    _sample_rate = sample_rate;
    _max_delay = juce::jmax(static_cast<int>(sample_rate), max_samples);

    _delay.setSize(2, _max_delay);
    _delay.clear();
}

void PingPongDelay::reset()
{
    _delay.clear();
}

void PingPongDelay::process(juce::AudioBuffer<float> &buffer)
{
    float *left = buffer.getWritePointer(0);
    float *right = buffer.getWritePointer(1);

    float *left_delay = _delay.getWritePointer(0);
    float *right_delay = _delay.getWritePointer(1);

    for (int i = 0; i < buffer.getNumSamples(); i++, _read_i = (_read_i + 1) % _max_delay, _write_i = (_write_i + 1) % _max_delay)
    {
        float left_sample = left[i];
        float right_sample = right[i];

        float left_delay_sample = left_delay[_read_i];
        float right_delay_sample = right_delay[_read_i];

        left[i] = left_sample * (1.0f - _dry_wet) + left_delay_sample * _dry_wet;
        right[i] = right_sample * (1.0f - _dry_wet) + right_delay_sample * _dry_wet;

        // Ping pong
        switch (_mode)
        {
        case Mode::mode_center:
        {
            left_delay[_write_i] = left_sample + right_delay_sample * _feedback;
            right_delay[_write_i] = right_sample + left_delay_sample * _feedback;
        }
        break;

        case Mode::mode_left:
        {
            left_delay[_write_i] = (left_sample + right_sample) / 2 + right_delay_sample * _feedback;
            right_delay[_write_i] = left_delay_sample * _feedback;
        }
        break;

        case Mode::mode_right:
        {
            left_delay[_write_i] = right_delay_sample * _feedback;
            right_delay[_write_i] = (left_sample + right_sample) / 2 + left_delay_sample * _feedback;
        }
        break;
        }

        // Clear
        left_delay[_read_i] = 0;
        right_delay[_read_i] = 0;
    }
}

void PingPongDelay::set_delay_in_ms(double delay_in_ms)
{
    int delay_in_samples = juce::jlimit(1, _max_delay - 1, static_cast<int>(juce::roundToInt(delay_in_ms * _sample_rate / 1000.)));
    _write_i = (_read_i + delay_in_samples) % _max_delay;
}

void PingPongDelay::set_feedback(float feedback)
{
    _feedback = juce::jlimit(0.0f, 1.0f, feedback);
}

void PingPongDelay::set_dry_wet(float dry_wet)
{
    _dry_wet = juce::jlimit(0.0f, 1.0f, dry_wet);
}

void PingPongDelay::set_mode(int mode)
{
    _mode = static_cast<Mode>(juce::jlimit(0, 2, mode));
}