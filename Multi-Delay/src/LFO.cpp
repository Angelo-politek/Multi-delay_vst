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
        {
            // Sawtooth con interpolazione lineare
            float t = _phase;
            if (t < 0.5f)
                value = 2.0f * t;
            else
                value = -2.0f * (1.0f - t);
            break;
        }
        case shape_square:
        {
            // Square con interpolazione lineare
            float t = _phase;
            const float transitionWidth = 0.1f; // Larghezza della transizione
            
            if (t < 0.5f - transitionWidth)
                value = 1.0f;
            else if (t < 0.5f + transitionWidth)
                value = 1.0f - (t - (0.5f - transitionWidth)) * (2.0f / (2.0f * transitionWidth));
            else if (t < 1.0f - transitionWidth)
                value = -1.0f;
            else
                value = -1.0f + (t - (1.0f - transitionWidth)) * (2.0f / (2.0f * transitionWidth));
            break;
        }
    }

    _phase += _rate / sampleRate;
    if (_phase >= 1.0f)
        _phase -= 1.0f;

    return value * _amount;
}
