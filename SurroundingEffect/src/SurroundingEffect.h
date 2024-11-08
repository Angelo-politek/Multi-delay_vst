#ifndef __SURROUNDING_EFFECT_H__
#define __SURROUNDING_EFFECT_H__

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class SurroundingEffect : public juce::AudioProcessor
{
public:
    SurroundingEffect();
    ~SurroundingEffect() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    void setPanning(float pan);
    void setModulation(float depth, float rate);

private:
    float panningValue;
    float modulationDepth;
    float modulationRate;

    juce::dsp::Oscillator<float> modulationOscillator;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SurroundingEffect)
};

#endif // __SURROUNDING_EFFECT_H__