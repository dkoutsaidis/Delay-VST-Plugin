/*
 ==============================================================================
 Author:  Dimitris Koutsaidis
 Contact: dkoutsaidis@gmail.com
 Date:    04/05/2019
 ==============================================================================
 */

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class DelayAudioProcessor  : public AudioProcessor
{
public:
    DelayAudioProcessor();
    ~DelayAudioProcessor();

    // main public methods
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(AudioBuffer<float>&, MidiBuffer&) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
   #endif

    // other public methods
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    const String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const String getProgramName(int index) override;
    void changeProgramName(int index, const String& newName) override;
    void getStateInformation(MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // user public methods
    
    // user public variables
    
private:
    // user private methods
    
    // user private variables
    AudioBuffer<float> delayBuffer;
    
    int writeIdx;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayAudioProcessor)
};
