#ifndef TEMPLATE_LFO_H
#define TEMPLATE_LFO_H

#include <juce_audio_basics/juce_audio_basics.h>

class LFO
{
public:
    enum Shape
    {
        shape_sine = 0,
        shape_saw = 1,
        shape_square = 2,
    };

private:
    float _rate;   // Frequenza in Hz
    float _amount; // Ampiezza della modulazione
    double _phase;
    Shape _shape;


public:
    LFO();

    void set_rate(float rateHz);
    void set_shape(int shape);
    void set_amount(float amount);
    float getNextValue(double sampleRate);
};

#endif
