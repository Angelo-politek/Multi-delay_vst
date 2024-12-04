#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "LFO.h"

class WaveformDisplay : public juce::Component
{
public:
    WaveformDisplay();
    void setLFO(LFO* lfo); // Connetti il LFO al display

    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    LFO* lfo; // Puntatore al LFO per accedere ai suoi valori
    juce::Path waveformPath; // Percorso per disegnare la forma d'onda

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformDisplay)
};
