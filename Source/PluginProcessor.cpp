#include "PluginProcessor.h"
#include "PluginEditor.h"

TB303AudioProcessor::TB303AudioProcessor()
    : AudioProcessor(BusesProperties()
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "Parameters", createParameterLayout())
{
    waveformParam   = apvts_.getRawParameterValue("waveform");
    tuningParam     = apvts_.getRawParameterValue("tuning");
    cutoffParam     = apvts_.getRawParameterValue("cutoff");
    resonanceParam  = apvts_.getRawParameterValue("resonance");
    envModParam     = apvts_.getRawParameterValue("envMod");
    decayParam      = apvts_.getRawParameterValue("decay");
    accentParam     = apvts_.getRawParameterValue("accent");
    volumeParam     = apvts_.getRawParameterValue("volume");
    masterTuneParam = apvts_.getRawParameterValue("masterTune");
    driveTypeParam  = apvts_.getRawParameterValue("driveType");
    driveToneParam  = apvts_.getRawParameterValue("driveTone");
    driveDepthParam = apvts_.getRawParameterValue("driveDepth");
    delayTypeParam  = apvts_.getRawParameterValue("delayType");
    delayTimeParam  = apvts_.getRawParameterValue("delayTime");
    delayLevelParam = apvts_.getRawParameterValue("delayLevel");
    delayFeedbackParam = apvts_.getRawParameterValue("delayFeedback");
    tempoSyncParam  = apvts_.getRawParameterValue("tempoSync");
    shuffleParam    = apvts_.getRawParameterValue("shuffle");
    scaleParam      = apvts_.getRawParameterValue("scale");
    playModeParam   = apvts_.getRawParameterValue("playMode");
    tempoParam      = apvts_.getRawParameterValue("tempo");
}

TB303AudioProcessor::~TB303AudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout TB303AudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "waveform", "Waveform", juce::StringArray{ "Sawtooth", "Square" }, 0));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "tuning", "Tuning", juce::NormalisableRange<float>(-12.0f, 12.0f, 0.01f), 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "cutoff", "Cut Off Freq",
        juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.3f), 1000.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "resonance", "Resonance", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.3f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "envMod", "Env Mod", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "decay", "Decay", juce::NormalisableRange<float>(30.0f, 3000.0f, 1.0f, 0.4f), 200.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "accent", "Accent", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "volume", "Volume", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.7f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "masterTune", "Master Tune", juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f), 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "driveType", "Drive Type", juce::StringArray{ "Soft", "Hard", "Tube" }, 0));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "driveTone", "Drive Tone", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "driveDepth", "Drive Depth", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "delayType", "Delay Type", juce::StringArray{ "Digital", "Tape", "Ping Pong" }, 0));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "delayTime", "Delay Time", juce::NormalisableRange<float>(1.0f, 2000.0f, 1.0f, 0.5f), 250.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "delayLevel", "Delay Level", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "delayFeedback", "Delay Feedback", juce::NormalisableRange<float>(0.0f, 0.95f, 0.01f), 0.3f));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "tempoSync", "Tempo Sync", false));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "shuffle", "Shuffle", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "scale", "Scale", juce::StringArray{ "1/16", "1/16T", "1/32" }, 0));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "playMode", "Play Mode", juce::StringArray{ "Forward", "Reverse", "FWD&REV", "Invert", "Random" }, 0));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "tempo", "Tempo", juce::NormalisableRange<float>(20.0f, 300.0f, 0.1f), 120.0f));

    return { params.begin(), params.end() };
}

void TB303AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    voice_.prepare(sampleRate);
    drive_.prepare(sampleRate);
    delay_.prepare(sampleRate, samplesPerBlock);
    sequencer_.prepare(sampleRate);
}

void TB303AudioProcessor::releaseResources() {}

bool TB303AudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    return true;
}

void TB303AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    buffer.clear();

    // Update voice parameters
    voice_.setWaveform(static_cast<int>(*waveformParam) == 0
                       ? TB303Oscillator::Waveform::Sawtooth
                       : TB303Oscillator::Waveform::Square);
    voice_.setTuning(*tuningParam + *masterTuneParam / 100.0f);
    voice_.setCutoff(*cutoffParam);
    voice_.setResonance(*resonanceParam);
    voice_.setEnvMod(*envModParam);
    voice_.setDecay(*decayParam);
    voice_.setAccentAmount(*accentParam);

    // Update drive
    drive_.setDriveType(static_cast<DriveEffect::DriveType>(static_cast<int>(*driveTypeParam)));
    drive_.setTone(*driveToneParam);
    drive_.setDepth(*driveDepthParam);

    // Update delay
    delay_.setDelayType(static_cast<DelayEffect::DelayType>(static_cast<int>(*delayTypeParam)));
    delay_.setTime(*delayTimeParam);
    delay_.setLevel(*delayLevelParam);
    delay_.setFeedback(*delayFeedbackParam);
    delay_.setTempoSync(*tempoSyncParam > 0.5f);

    // Update sequencer
    sequencer_.setTempo(static_cast<double>(*tempoParam));
    sequencer_.setPlayMode(static_cast<StepSequencer::PlayMode>(static_cast<int>(*playModeParam)));
    sequencer_.setScale(static_cast<StepSequencer::Scale>(static_cast<int>(*scaleParam)));
    sequencer_.setShuffle(*shuffleParam);

    // Host sync
    if (auto* playHead = getPlayHead())
    {
        if (auto position = playHead->getPosition())
        {
            if (auto bpm = position->getBpm())
                delay_.setBPM(*bpm);
            if (auto ppq = position->getPpqPosition())
            {
                bool hostPlaying = position->getIsPlaying();
                sequencer_.setHostPosition(*ppq, position->getBpm().orFallback(120.0), hostPlaying);
            }
        }
    }

    // Process MIDI for live keyboard play
    for (const auto metadata : midiMessages)
    {
        auto msg = metadata.getMessage();
        if (msg.isNoteOn())
            voice_.noteOn(msg.getNoteNumber(), msg.getVelocity() > 100, false);
        else if (msg.isNoteOff())
            voice_.noteOff();
    }

    int numSamples = buffer.getNumSamples();

    // Process sequencer (triggers notes on the voice)
    sequencer_.processBlock(numSamples, voice_);

    // Render voice into buffer
    auto* channelL = buffer.getWritePointer(0);
    float volume = *volumeParam;

    for (int i = 0; i < numSamples; ++i)
    {
        float sample = voice_.process();
        sample = drive_.process(sample);
        channelL[i] = sample * volume;
    }

    // Process delay (stereo)
    float* channelR = (totalNumOutputChannels > 1) ? buffer.getWritePointer(1) : nullptr;
    if (channelR != nullptr)
        std::copy(channelL, channelL + numSamples, channelR);

    delay_.process(channelL, channelR, numSamples);
}

juce::AudioProcessorEditor* TB303AudioProcessor::createEditor()
{
    return new TB303AudioProcessorEditor(*this);
}

bool TB303AudioProcessor::hasEditor() const { return true; }
const juce::String TB303AudioProcessor::getName() const { return JucePlugin_Name; }
bool TB303AudioProcessor::acceptsMidi() const { return true; }
bool TB303AudioProcessor::producesMidi() const { return false; }
bool TB303AudioProcessor::isMidiEffect() const { return false; }
double TB303AudioProcessor::getTailLengthSeconds() const { return 2.0; }

int TB303AudioProcessor::getNumPrograms() { return presetManager_.getNumPatches(); }
int TB303AudioProcessor::getCurrentProgram() { return 0; }
void TB303AudioProcessor::setCurrentProgram(int index) { loadPatch(index); }
const juce::String TB303AudioProcessor::getProgramName(int index)
{
    return presetManager_.getPatch(index).name;
}
void TB303AudioProcessor::changeProgramName(int /*index*/, const juce::String& /*newName*/) {}

void TB303AudioProcessor::loadPatch(int index)
{
    auto& patch = presetManager_.getPatch(index);
    apvts_.getParameter("cutoff")->setValueNotifyingHost(
        apvts_.getParameterRange("cutoff").convertTo0to1(patch.cutoff));
    apvts_.getParameter("resonance")->setValueNotifyingHost(patch.resonance);
    apvts_.getParameter("envMod")->setValueNotifyingHost(patch.envMod);
    apvts_.getParameter("decay")->setValueNotifyingHost(
        apvts_.getParameterRange("decay").convertTo0to1(patch.decay));
    apvts_.getParameter("accent")->setValueNotifyingHost(patch.accent);
    apvts_.getParameter("tuning")->setValueNotifyingHost(
        apvts_.getParameterRange("tuning").convertTo0to1(patch.tuning));
    apvts_.getParameter("waveform")->setValueNotifyingHost(
        static_cast<float>(patch.waveform));
    apvts_.getParameter("driveDepth")->setValueNotifyingHost(patch.driveDepth);
    apvts_.getParameter("driveType")->setValueNotifyingHost(
        static_cast<float>(patch.driveType) / 2.0f);
    apvts_.getParameter("driveTone")->setValueNotifyingHost(patch.driveTone);
    apvts_.getParameter("delayTime")->setValueNotifyingHost(
        apvts_.getParameterRange("delayTime").convertTo0to1(patch.delayTime));
    apvts_.getParameter("delayLevel")->setValueNotifyingHost(patch.delayLevel);
    apvts_.getParameter("delayFeedback")->setValueNotifyingHost(
        apvts_.getParameterRange("delayFeedback").convertTo0to1(patch.delayFeedback));
    apvts_.getParameter("delayType")->setValueNotifyingHost(
        static_cast<float>(patch.delayType) / 2.0f);
    apvts_.getParameter("tempoSync")->setValueNotifyingHost(patch.tempoSync ? 1.0f : 0.0f);
}

void TB303AudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    juce::ValueTree state("TB303State");
    state.addChild(apvts_.copyState(), 0, nullptr);
    state.addChild(sequencer_.getPatternData().toValueTree(), 1, nullptr);
    state.addChild(presetManager_.toValueTree(), 2, nullptr);

    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void TB303AudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml != nullptr)
    {
        auto state = juce::ValueTree::fromXml(*xml);
        if (state.isValid())
        {
            if (state.getNumChildren() > 0)
                apvts_.replaceState(state.getChild(0));
            if (state.getNumChildren() > 1)
                sequencer_.getPatternData().fromValueTree(state.getChild(1));
            if (state.getNumChildren() > 2)
                presetManager_.fromValueTree(state.getChild(2));
        }
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TB303AudioProcessor();
}
