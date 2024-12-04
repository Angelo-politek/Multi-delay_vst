#include "WaveformDisplay.h"

WaveformDisplay::WaveformDisplay() : lfo(nullptr) {}

void WaveformDisplay::setLFO(LFO* lfoInstance)
{
    lfo = lfoInstance;
    repaint();
}

void WaveformDisplay::resized()
{
    // Ridisegna la forma d'onda quando il componente cambia dimensione
    repaint();
}

void WaveformDisplay::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);

    if (lfo == nullptr)
        return;

    auto bounds = getLocalBounds().toFloat();
    waveformPath.clear();

    constexpr int numPoints = 200; // Numero di punti da calcolare
    const float sampleRate = 44100.0f; // Tasso di campionamento simulato

    float phase = 0.0f;

    for (int i = 0; i <= numPoints; ++i)
    {
        float lfoValue = lfo->getNextValue(sampleRate); // Usa l'LFO

        float x = bounds.getX() + i * bounds.getWidth() / numPoints;
        float y = bounds.getCentreY() - (lfoValue * bounds.getHeight() * 0.4f);

        if (i == 0)
            waveformPath.startNewSubPath(x, y);
        else
            waveformPath.lineTo(x, y);

        // Incrementa manualmente la fase per simulare il ciclo
        phase += (1.0f / numPoints);
        if (phase >= 1.0f)
            phase -= 1.0f;
    }

    g.setColour(juce::Colours::limegreen);
    g.strokePath(waveformPath, juce::PathStrokeType(2.0f));
}

