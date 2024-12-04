#include "PluginProcessor.h"
#include "LFO.h"

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor() : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                                                                                .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                                                                                .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
),
                                                         parameters{*this, nullptr, juce::Identifier("parameters"), createParameterLayout()}
{
    parameters.addParameterListener("rate", this);
    parameters.addParameterListener("amount", this);
    parameters.addParameterListener("shape", this);
}


AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
    parameters.removeParameterListener("rate", this);
    parameters.removeParameterListener("amount", this);
    parameters.removeParameterListener("shape", this);
}

juce::AudioProcessorValueTreeState::ParameterLayout AudioPluginAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // TODO: add your parameters here
    layout.add(std::make_unique<juce::AudioParameterFloat>(
            "rate", "Rate", juce::NormalisableRange<float>(0.1f, 20.0f, 0.1f), 20.0f, juce::String{}, juce::AudioProcessorParameter::Category::genericParameter, [](float val, int) -> juce::String
            { return juce::String(val) + juce::String(" Hz"); },
            [](juce::String str) -> float
            {
                return str.getFloatValue();
            }));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
            "amount", "Amount", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f, juce::String{}, juce::AudioProcessorParameter::Category::genericParameter, [](float val, int) -> juce::String
            { return juce::String(val * 100) + juce::String(" %"); },
            [](juce::String str) -> float
            {
                return str.getFloatValue();
            }));
    layout.add(std::make_unique<juce::AudioParameterChoice>("shape", "Shape", juce::StringArray({"Sine", "Saw", "Square"}), 0));

    return layout;
}

void AudioPluginAudioProcessor::parameterChanged(const juce::String &id, float newValue)
{
    if (id == "rate")
    {
        lfo.set_rate(newValue);
    }
    else if (id == "amount")
    {
        lfo.set_amount(newValue);
    }
    else if (id == "shape")
    {
        lfo.set_shape(static_cast<int>(newValue));
    }
}
//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
#ifdef PLUGIN_NAME
    return PLUGIN_NAME;
#else
    return "";
#endif
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
              // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String AudioPluginAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName(int index, const juce::String &newName)
{
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::ignoreUnused(sampleRate, samplesPerBlock);

    // TODO: if needed initialize any attribute/dependency which allocates memory
    lfo.set_rate(*parameters.getRawParameterValue("rate"));
    lfo.set_shape(static_cast<int>(*parameters.getRawParameterValue("shape")));
    lfo.set_amount(*parameters.getRawParameterValue("amount"));


}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.

    // TODO: if needed release any attribute/dependency which has memory allocated
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo() && layouts.getMainInputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

        // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}

void AudioPluginAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                             juce::MidiBuffer &midiMessages)
{
    juce::ignoreUnused(midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());


    auto sampleRate = getSampleRate();


    for (int channel = 0; channel < totalNumInputChannels; ++channel){
        auto* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample){
            float lfoValue = (lfo.getNextValue(sampleRate) + 1.0f) * 0.5f; // Normalizza tra 0 e 1
            channelData[sample] *= lfoValue; // Modula il volume
        }
    }
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *AudioPluginAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this); // Use the generic editor
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    juce::MemoryOutputStream mos{destData, true};
    parameters.state.writeToStream(mos);
}

void AudioPluginAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    auto tree = juce::ValueTree::readFromData(data, static_cast<std::size_t>(sizeInBytes));
    if (tree.isValid())
    {
        parameters.replaceState(tree);
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}
