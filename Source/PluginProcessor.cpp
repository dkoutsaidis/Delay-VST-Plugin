/*
 ==============================================================================
 Author:  Dimitris Koutsaidis
 Contact: dkoutsaidis@gmail.com
 Date:    04/05/2019
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

DelayAudioProcessor::DelayAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor(BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
     : delayPluginParameters(*this, nullptr),
       writeIdx(0),
       latestSampleRate(-1),
       latestWetGainValue(0.f),
       latestDelayFeedbackValue(0.f),
       wetGainValue(1.f),
       delayTimeValue(200.f),
       delayFeedbackValue(std::pow(10.f, (-12.f / 20.f)))
{
    // Add wet gain parameter
    delayPluginParameters.createAndAddParameter("wetGain",
                                                "Wet Gain",
                                                "dB",
                                                NormalisableRange<float>(-80.f, 12.f, 1.f), -6.f,
                                                [](float value) { return String(value, 0); },
                                                [](const String& text)
                                                {
                                                    float valueToConstrain = text.getFloatValue();
                                                    return jlimit(-80.f, 12.f, valueToConstrain);
                                                });
    
    // Add delay time parameter
    delayPluginParameters.createAndAddParameter("delayTime",
                                                "Delay Time",
                                                "msec",
                                                NormalisableRange<float>(0.f, 1000.f, 1.f), 200.f,
                                                [](float value) { return String(value, 0); },
                                                [](const String& text)
                                                {
                                                    float valueToConstrain = text.getFloatValue();
                                                    return jlimit(0.f, 1000.f, valueToConstrain);
                                                });
    
    // Add feedback gain parameter
    delayPluginParameters.createAndAddParameter("delayFeedback",
                                                "Delay Feedback",
                                                "dB",
                                                NormalisableRange<float>(-80.f, 0.f, 1.f), -12.f,
                                                [](float value) { return String(value, 0); },
                                                [](const String& text)
                                                {
                                                    float valueToConstrain = text.getFloatValue();
                                                    return jlimit(-80.f, 0.f, valueToConstrain);
                                                });
    
    delayPluginParameters.state = ValueTree(Identifier("Parameters"));
    delayPluginParameters.addParameterListener("wetGain", this);
    delayPluginParameters.addParameterListener("delayTime", this);
    delayPluginParameters.addParameterListener("delayFeedback", this);
}

DelayAudioProcessor::~DelayAudioProcessor()
{
    
}

//==============================================================================
const String DelayAudioProcessor::getName() const { return JucePlugin_Name; }
bool DelayAudioProcessor::acceptsMidi() const { return false; }
bool DelayAudioProcessor::producesMidi() const { return false; }
bool DelayAudioProcessor::isMidiEffect() const { return false; }
double DelayAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int DelayAudioProcessor::getNumPrograms() { return 1; }
int DelayAudioProcessor::getCurrentProgram() { return 0; }
void DelayAudioProcessor::setCurrentProgram(int index) { }
const String DelayAudioProcessor::getProgramName(int index) { return {}; }
void DelayAudioProcessor::changeProgramName(int index, const String& newName) { }

//==============================================================================
void DelayAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    if (sampleRate != latestSampleRate)
        latestSampleRate = sampleRate;
    
    delayBuffer.setSize(getTotalNumInputChannels(), 2 * (sampleRate + samplesPerBlock));
}

void DelayAudioProcessor::releaseResources()
{

}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DelayAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono() &&
        layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}
#endif

void DelayAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;

    for (auto i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    for (auto channel = 0; channel < getTotalNumInputChannels(); ++channel)
    {
        fillDelayBuffer(buffer, channel);
        getFromDelayBuffer(buffer, channel);
    }
    
    writeIdx += buffer.getNumSamples();
    writeIdx %= delayBuffer.getNumSamples();
    latestWetGainValue = wetGainValue.load();
    latestDelayFeedbackValue = delayFeedbackValue.load();
}

void DelayAudioProcessor::fillDelayBuffer(AudioBuffer<float>& buffer_, const int channel_)
{
    if (delayBuffer.getNumSamples() > buffer_.getNumSamples() + writeIdx)
    {
        delayBuffer.copyFromWithRamp(channel_, writeIdx,
                                     buffer_.getReadPointer(channel_), buffer_.getNumSamples(),
                                     latestWetGainValue, wetGainValue.load());
    }
    else
    {
        const int remainingData = delayBuffer.getNumSamples() - writeIdx;
        const float remainingDataGain = latestWetGainValue +
                                        ((latestWetGainValue - wetGainValue.load()) / buffer_.getNumSamples()) *
                                        (remainingData / buffer_.getNumSamples());
        
        delayBuffer.copyFromWithRamp(channel_, writeIdx,
                                     buffer_.getReadPointer(channel_), remainingData,
                                     latestWetGainValue, remainingDataGain);
        delayBuffer.copyFromWithRamp(channel_, 0,
                                     buffer_.getReadPointer(channel_), buffer_.getNumSamples() - remainingData,
                                     remainingDataGain, wetGainValue.load());
    }
}

void DelayAudioProcessor::getFromDelayBuffer(AudioBuffer<float>& buffer_, const int channel_)
{
    if (latestSampleRate != -1)
    {
        const int readPosition = static_cast<int>(delayBuffer.getNumSamples() + writeIdx -
                                                 (latestSampleRate * delayTimeValue.load() / 1000)) %
                                                 delayBuffer.getNumSamples();
        
        if (delayBuffer.getNumSamples() > buffer_.getNumSamples() + readPosition)
        {
            buffer_.addFrom(channel_, 0, delayBuffer, channel_, readPosition, buffer_.getNumSamples());
        }
        else
        {
            const int remainingData = delayBuffer.getNumSamples() - readPosition;
            
            buffer_.addFrom(channel_, 0, delayBuffer, channel_, readPosition, remainingData);
            buffer_.addFrom(channel_, remainingData, delayBuffer, channel_, 0, buffer_.getNumSamples() - remainingData);
        }
    }
}

//==============================================================================
bool DelayAudioProcessor::hasEditor() const
{
    return true;
}

AudioProcessorEditor* DelayAudioProcessor::createEditor()
{
    return new DelayAudioProcessorEditor(*this);
}

//==============================================================================
void DelayAudioProcessor::getStateInformation(MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DelayAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void DelayAudioProcessor::parameterChanged(const String &parameterID, float newValue)
{
    if (parameterID == "wetGain")
    {
        wetGainValue.store( std::pow(10.f, (newValue / 20.f)) );
    }
    else if (parameterID == "delayTime")
    {
        delayTimeValue.store(newValue);
    }
    else if (parameterID == "delayFeedback")
    {
        delayFeedbackValue.store( std::pow(10.f, (newValue / 20.f)) );
    }
}

AudioProcessorValueTreeState& DelayAudioProcessor::getProcessorState()
{
    return delayPluginParameters;
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DelayAudioProcessor();
}
