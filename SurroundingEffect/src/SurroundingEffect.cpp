#include "SurroundingEffect.h"

/* NOTES:
 * 1. Panning Control: The panningValue variable controls the stereo balance between
 * the left and right channels. The user can adjust this to move the sound between
 * the two channels. A modulation effect is applied to this value.
 * 2. Modulation: The modulationDepth and modulationRate variables allow the user
 * to control how much and how fast the modulation occurs. A sine wave oscillator
 * is used for modulation. The depth and rate affect how the panning shifts over time.
 * 3. JUCE DSP: The plugin uses juce::dsp::Oscillator for the modulation effect
 * and processes the audio block by adjusting the panning on the fly based on
 * the modulation signal.
 * 4. State Management: Methods getStateInformation and setStateInformation
 * are placeholders for saving/loading plugin settings.
*/

// Constructor
SurroundingEffect::SurroundingEffect()
    : panningValue(0.0f), modulationDepth(0.5f), modulationRate(1.0f)
{
    // Set up the modulation oscillator (sine wave by default)
    modulationOscillator.initialise([](float x) { return std::sin(x); }, 256);
}

// Destructor
SurroundingEffect::~SurroundingEffect() {}

void SurroundingEffect::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Initialize modulation oscillator with the sample rate
    modulationOscillator.prepare({ sampleRate, static_cast<uint32>(samplesPerBlock), 2 });
}

void SurroundingEffect::releaseResources() {}

void SurroundingEffect::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // Create a modulation signal for panning effect
    juce::dsp::AudioBlock<float> audioBlock(buffer);
    juce::dsp::ProcessContextReplacing<float> context(audioBlock);

    // Process modulation oscillator
    modulationOscillator.process(context);

    // Apply panning to the left and right channels
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float modulation = modulationOscillator.getNextSample() * modulationDepth;
            float pan = panningValue + modulation;
            pan = juce::jlimit(-1.0f, 1.0f, pan);

            if (channel == 0) // Left channel
            {
                channelData[sample] *= (1.0f - pan); // Apply left panning
            }
            else if (channel == 1) // Right channel
            {
                channelData[sample] *= (1.0f + pan); // Apply right panning
            }
        }
    }
}

juce::AudioProcessorEditor* SurroundingEffect::createEditor()
{
    return nullptr; // Placeholder: You can implement a GUI editor for your plugin
}

bool SurroundingEffect::hasEditor() const
{
    return false; // No GUI editor for now
}

void SurroundingEffect::getStateInformation(juce::MemoryBlock& destData)
{
    // Save plugin state (for future preset saving or loading)
}

void SurroundingEffect::setStateInformation(const void* data, int sizeInBytes)
{
    // Load plugin state from saved preset
}

void SurroundingEffect::setPanning(float pan)
{
    panningValue = juce::jlimit(-1.0f, 1.0f, pan); // Range [-1.0, 1.0]
}

void SurroundingEffect::setModulation(float depth, float rate)
{
    modulationDepth = juce::jlimit(0.0f, 1.0f, depth); // Modulation depth range [0.0, 1.0]
    modulationRate = juce::jlimit(0.1f, 10.0f, rate);  // Modulation rate range [0.1, 10.0]

    // Update oscillator frequency based on modulation rate
    modulationOscillator.setFrequency(modulationRate);
}