#include "PluginProcessor.h"

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
{   // Parametri dell'interfaccia grafica
    // Delay Parameters
    parameters.addParameterListener("delay-sx", this);
    parameters.addParameterListener("delay-dx", this);
    parameters.addParameterListener("feedback", this);
    parameters.addParameterListener("dry-wet", this);
    parameters.addParameterListener("sync-enable", this);
    parameters.addParameterListener("delay-mode", this);
    parameters.addParameterListener("pingpong-mode", this);
    // Pan Parameters
    parameters.addParameterListener("pan", this);
    // LFO Parameters
    parameters.addParameterListener("rate", this);
    parameters.addParameterListener("amount", this);
    parameters.addParameterListener("shape", this);
}


AudioPluginAudioProcessor::~AudioPluginAudioProcessor()    // Distruttore dell'oggetto AudioPluginAudioProcessor
{   // Rimozione dei parametri
    parameters.removeParameterListener("delay-sx", this);
    parameters.removeParameterListener("delay-dx", this);
    parameters.removeParameterListener("feedback", this);
    parameters.removeParameterListener("dry-wet", this);
    parameters.removeParameterListener("sync-enable", this);
    parameters.removeParameterListener("delay-mode", this);
    parameters.removeParameterListener("pingpong-mode", this);
    parameters.removeParameterListener("pan", this);
    parameters.removeParameterListener("rate", this);
    parameters.removeParameterListener("amount", this);
    parameters.removeParameterListener("shape", this);
}

juce::AudioProcessorValueTreeState::ParameterLayout AudioPluginAudioProcessor::createParameterLayout()  // Metodo per creare il layout dei parametri
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    // Delay
    layout.add(std::make_unique<juce::AudioParameterChoice>("delay-mode", "Delay Mode", juce::StringArray({ "feedback", "pingpong"}), 0));
    layout.add(std::make_unique<juce::AudioParameterChoice>("pingpong-mode", "Pingpong Mode", juce::StringArray({ "center", "left", "right" }), 0));
    layout.add(std::make_unique<juce::AudioParameterBool>("sync-enable", "Sync", false));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
            "delay-sx", "Delay Sx/Master", juce::NormalisableRange<float>(0.0f, 500.0f, 0.1f), 150.0f, juce::String{}, juce::AudioProcessorParameter::Category::genericParameter, [](float val, int) -> juce::String
            { return juce::String(val) + juce::String(" ms"); },
            [](juce::String str) -> float
            {
                return str.getFloatValue();
            }));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
            "delay-dx", "Delay Dx", juce::NormalisableRange<float>(0.0f, 500.0f, 0.1f), 100.0f, juce::String{}, juce::AudioProcessorParameter::Category::genericParameter, [](float val, int) -> juce::String
            { return juce::String(val) + juce::String(" ms"); },
            [](juce::String str) -> float
            {
                return str.getFloatValue();
            }));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
            "feedback", "Feedback", juce::NormalisableRange<float>(0.0f, juce::Decibels::decibelsToGain<float>(0.0f), 0.0005f, 0.4f), 0.1f, juce::String{}, juce::AudioProcessorParameter::Category::genericParameter, [](float val, int) -> juce::String
            { return juce::String(juce::Decibels::gainToDecibels(val)) + juce::String(" dB"); },
            [](juce::String val)
            {
                return juce::Decibels::decibelsToGain<float>(val.getFloatValue());
            }));
    layout.add(std::make_unique<juce::AudioParameterInt>(
            "dry-wet", "Dry/Wet", 0, 100, 15, juce::String{}, [](int val, int)
            { return juce::String(static_cast<int>(val)) + juce::String(" %"); },
            [](juce::String val)
            {
                return val.getIntValue();
            }));


    // LFO
    layout.add(std::make_unique<juce::AudioParameterFloat>(
            "rate", "Rate", juce::NormalisableRange<float>(0.1f, 5.0f, 0.05f), 1.0f, juce::String{}, juce::AudioProcessorParameter::Category::genericParameter, [](float val, int) -> juce::String
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

    // Pan
    layout.add(std::make_unique<juce::AudioParameterFloat>(
            "pan", "Pan", juce::NormalisableRange<float>(-1.0f, 1.0f, 0.1f), 0.0f, juce::String{}, juce::AudioProcessorParameter::Category::genericParameter, [](float val, int) -> juce::String
            { return juce::String(val) + juce::String(" "); },
            [](juce::String str) -> float
            {
                return str.getFloatValue();
            }));

    return layout;
}

void AudioPluginAudioProcessor::parameterChanged(const juce::String &id, float newValue)  // Metodo per gestire i cambiamenti dei parametri
{
    juce::ignoreUnused(id, newValue);

    if (id == "delay-sx")
    {
        delay.set_delay_sx_in_ms(newValue);
    }
    else if (id == "delay-dx")
    {
        delay.set_delay_dx_in_ms(newValue);
    }
    else if (id == "sync-enable")
    {
        delay.enable_sync(newValue >= 0.5f); // Correzione qui: convertiamo correttamente a bool
    }
    else if (id == "delay-mode")
    {
        delay.set_delay_mode(static_cast<int>(newValue));
    }
    else if (id == "pingpong-mode")
    {
        delay.set_pingpong_mode(static_cast<int>(newValue));
    }
    else if (id == "feedback")
    {
        delay.set_feedback(newValue);
    }
    else if (id == "dry-wet")
    {
        delay.set_dry_wet(newValue / 100.f);
    }

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

    delay.prepare(sampleRate, samplesPerBlock);

    delay.enable_sync(static_cast<int>(*parameters.getRawParameterValue("sync-enable")));
    delay.set_delay_mode(static_cast<int>(*parameters.getRawParameterValue("delay-mode")));
    delay.set_pingpong_mode(static_cast<int>(*parameters.getRawParameterValue("pingpong-mode")));

    delay.set_delay_dx_in_ms(*parameters.getRawParameterValue("delay-dx"));
    delay.set_delay_sx_in_ms(*parameters.getRawParameterValue("delay-sx"));

    delay.set_feedback(*parameters.getRawParameterValue("feedback"));
    delay.set_dry_wet(*parameters.getRawParameterValue("dry-wet") / 100);

    lfo.set_rate(*parameters.getRawParameterValue("rate"));
    lfo.set_shape(static_cast<int>(*parameters.getRawParameterValue("shape")));
    lfo.set_amount(*parameters.getRawParameterValue("amount"));



}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.

    // TODO: if needed release any attribute/dependency which has memory allocated
    pan.reset();
    delay.reset();
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

    // TODO: add your logic
    auto sampleRate = getSampleRate();                                                      // Ottiene il sample rate
    for (int channel = 0; channel < totalNumInputChannels; ++channel)                       // Per ogni canale
    {
        auto *channelData = buffer.getWritePointer(channel);                                // Ottiene il puntatore al canale

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)                     // Per ogni campione
        {
            float lfoValue = (lfo.getNextValue(sampleRate));                                // Ottiene il valore successivo dell'LFO
            pan.set_pan(*parameters.getRawParameterValue("pan") + lfoValue);                // Imposta il panning in base al parametro pan e al valore dell'LFO
        }
    }
    delay.process(buffer);                                                                  // Applica l'effetto delay al buffer
    pan.process(buffer);                                                                    // Applica il panning al buffer
    
    

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
