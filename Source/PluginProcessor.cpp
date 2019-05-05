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
     : writeIdx(0),
       latestSampleRate(-1)
{
    
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
void DelayAudioProcessor::setCurrentProgram (int index) { }
const String DelayAudioProcessor::getProgramName (int index) { return {}; }
void DelayAudioProcessor::changeProgramName (int index, const String& newName) { }

//==============================================================================
void DelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    if (sampleRate != latestSampleRate)
        latestSampleRate = sampleRate;
    
    delayBuffer.setSize(getTotalNumInputChannels(), 2 * (sampleRate + samplesPerBlock));
}

void DelayAudioProcessor::releaseResources()
{

}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void DelayAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;

    for (auto i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    const int bufferLength = buffer.getNumSamples();
    const int delayBufferLength = delayBuffer.getNumSamples();
    
    for (auto channel = 0; channel <= getTotalNumInputChannels(); ++channel)
    {
        const float* bufferData = buffer.getReadPointer(channel);
        const float* delayBufferData = delayBuffer.getReadPointer(channel);
        
        fillDelayBuffer(channel, bufferLength, delayBufferLength, bufferData, delayBufferData);
        getFromDelayBuffer(buffer, channel, bufferLength, delayBufferLength, bufferData, delayBufferData);
    }
    
    writeIdx += bufferLength;
    writeIdx %= delayBufferLength;
}

void DelayAudioProcessor::fillDelayBuffer(const int channel_,
                                          const int bufferLength_, const int delayBufferLength_,
                                          const float* bufferData_, const float* delayBufferData_)
{
    if (delayBufferLength_ > bufferLength_ + writeIdx)
    {
        delayBuffer.copyFromWithRamp(channel_, writeIdx, bufferData_, bufferLength_, 0.8, 0.8);
    }
    else
    {
        const int remainingData = delayBufferLength_ - writeIdx;
        
        delayBuffer.copyFromWithRamp(channel_, writeIdx, bufferData_, remainingData, 0.8, 0.8);
        delayBuffer.copyFromWithRamp(channel_, 0, bufferData_, bufferLength_ - remainingData, 0.8, 0.8);
    }
}

void DelayAudioProcessor::getFromDelayBuffer(AudioBuffer<float>& buffer_, const int channel_,
                                             const int bufferLength_, const int delayBufferLength_,
                                             const float* bufferData_, const float* delayBufferData_)
{
    if (latestSampleRate != -1)
    {
        int delayTime = 500;
        
        const int readPosition = static_cast<int>(delayBufferLength_ + writeIdx - (latestSampleRate * delayTime / 1000)) % delayBufferLength_;
        
        if (delayBufferLength_ > bufferLength_ + readPosition)
        {
            buffer_.addFrom(channel_, 0, delayBufferData_, readPosition, bufferLength_);
        }
        else
        {
            const int remainingData = delayBufferLength_ - readPosition;
            
            buffer_.addFrom(channel_, 0, delayBufferData_, readPosition, remainingData);
            buffer_.addFrom(channel_, remainingData, delayBufferData_, 0, bufferLength_ - remainingData);
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
void DelayAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DelayAudioProcessor();
}
