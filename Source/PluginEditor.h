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
#include <memory>

#define BUILD_VER_SUFFIX " beta" // String to be added before the version name on the GUI (e.g. beta, alpha etc..)

class DelayAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    DelayAudioProcessorEditor(DelayAudioProcessor&);
    ~DelayAudioProcessorEditor();

    void paint (Graphics&) override;
    void resized() override;

private:
    DelayAudioProcessor &processor; /*!< A reference to the processor instance. */
    
    // sliders
    std::unique_ptr<Slider> wetGainSlider;
    std::unique_ptr<Slider> delayTimeSlider;
    std::unique_ptr<Slider> delayFeedbackSlider;
    
    // APVTS attachments
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> wetGainSliderAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> delayTimeSliderAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> delayFeedbackSliderAttachment;
    
    // UI size and sliders' position variables
    const int totalWidth, totalHeight, sliderYpos, sliderWidth, sliderHeight;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayAudioProcessorEditor)
};
