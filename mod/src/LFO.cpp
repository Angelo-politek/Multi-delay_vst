#include "LFO.h"

LFO::LFO() : _amount(0.0f), _rate(1.0f), _shape(shape_sine), _phase(0.0f) {}

void LFO::set_rate(float rateHz)
{
    _rate = juce::jlimit(0.1f, 20.0f, rateHz);
}

void LFO::set_shape(int shape)
{
    _shape = static_cast<Shape>(juce::jlimit(0, 2, shape));
}

void LFO::set_amount(float amount)
{
    _amount = juce::jlimit(0.0f, 1.0f, amount);
}

float LFO::getNextValue(double sampleRate)
{
    float value = 0.0f;

    switch (_shape)
    {
        case shape_sine:
            value = std::sin(2.0 * juce::MathConstants<double>::pi * _phase);
            break;
        case shape_saw:
            value = 2.0f * (_phase - std::floor(_phase + 0.5f)); // Sawtooth classico
            break;
        case shape_square:
            value = (_phase < 0.5f) ? 1.0f : -1.0f;
            break;
    }

    // Incremento della fase basato sulla frequenza
    _phase += _rate / sampleRate;
    if (_phase >= 1.0f)
        _phase -= 1.0f;

    return value * _amount;
}
