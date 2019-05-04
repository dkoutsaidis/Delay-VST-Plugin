/*
  ==============================================================================
  Author:  Dimitris Koutsaidis
  Contact: dkoutsaidis@gmail.com
  Date:    04/05/2019
  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

class DelayAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    DelayAudioProcessorEditor(DelayAudioProcessor&);
    ~DelayAudioProcessorEditor();

    void paint (Graphics&) override;
    void resized() override;

private:
    DelayAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayAudioProcessorEditor)
};
