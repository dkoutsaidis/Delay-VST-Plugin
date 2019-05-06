/*
 ==============================================================================
 Author:  Dimitris Koutsaidis
 Contact: dkoutsaidis@gmail.com
 Date:    04/05/2019
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

DelayAudioProcessorEditor::DelayAudioProcessorEditor(DelayAudioProcessor& p)
    : AudioProcessorEditor(&p),
      processor(p),
      totalWidth(400), totalHeight(400),
      sliderYpos(150), sliderWidth(50), sliderHeight(200)
{
    // slider for adjusting the wet gain of the delay FX
    wetGainSlider.reset(new Slider());
    wetGainSlider->setSliderStyle(Slider::SliderStyle::LinearVertical);
    wetGainSlider->setTextBoxStyle(Slider::TextBoxBelow, true, totalHeight/8, totalHeight/16);
    wetGainSlider->setRange(-80.f, 12.f, 1.f); // dB
    wetGainSlider->setValue(-6.f);
    addAndMakeVisible(wetGainSlider.get());
    
    // slider for adjusting the delay time
    delayTimeSlider.reset(new Slider());
    delayTimeSlider->setSliderStyle(Slider::SliderStyle::LinearVertical);
    delayTimeSlider->setTextBoxStyle(Slider::TextBoxBelow, true, totalHeight/8, totalHeight/16);
    delayTimeSlider->setRange(0.f, 1000.f, 1.f); // msec
    delayTimeSlider->setValue(200.f);
    addAndMakeVisible(delayTimeSlider.get());
    
    // slider for adjusting the feedback gain of the delay FX
    delayFeedbackSlider.reset(new Slider());
    delayFeedbackSlider->setSliderStyle(Slider::SliderStyle::LinearVertical);
    delayFeedbackSlider->setTextBoxStyle(Slider::TextBoxBelow, true, totalHeight/8, totalHeight/16);
    delayFeedbackSlider->setRange(-80.f, 0.f, 1.f);
    delayFeedbackSlider->setValue(-12.f);
    addAndMakeVisible(delayFeedbackSlider.get());
    
    // APVTS attachments
    wetGainSliderAttachment.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.getProcessorState(), "wetGain", *wetGainSlider.get()));
    
    delayTimeSliderAttachment.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.getProcessorState(), "delayTime", *delayTimeSlider.get()));
    
    delayFeedbackSliderAttachment.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.getProcessorState(), "delayFeedback", *delayFeedbackSlider.get()));
    
    // init UI size
    setSize(totalWidth, totalHeight);
}

DelayAudioProcessorEditor::~DelayAudioProcessorEditor()
{
    wetGainSliderAttachment = nullptr;
    delayTimeSliderAttachment = nullptr;
    delayFeedbackSliderAttachment = nullptr;
}

void DelayAudioProcessorEditor::paint(Graphics& g)
{
    // Our component is opaque, so we must completely fill the background with a solid colour.
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
    
    // add title
    {
        g.setColour(Colour(71, 163, 198));
        g.setFont(Font(18.8f, Font::plain).withTypefaceStyle("Bold"));
        g.drawText(TRANS("Delay"), 0, 20, totalWidth, 32, Justification::centred, true);
    }
    
    // add description
    {
        g.setColour(Colour(71, 163, 198));
        g.setFont(Font(14.f, Font::plain).withTypefaceStyle("Bold"));
        g.drawText(TRANS("lay lay lay "), getWidth()/2, 45, 70, 32, Justification::centredLeft, true);
    }
    
    // add labels for the sliders
    {
        int x = 50, width = 100, height = 32;
        g.setColour(Colours::whitesmoke);
        g.setFont(Font (12.00f, Font::plain).withTypefaceStyle("Bold"));
        g.drawText(TRANS("Delay"), x, 100, width, height, Justification::centred, true);
        g.drawText(TRANS("Time"), x, 112, width, height, Justification::centred, true);
        g.drawText(TRANS("(msec)"), x, 124, width, height, Justification::centred, true);
    }
    {
        int x = 150, width = 100, height = 32;
        g.setColour(Colours::whitesmoke);
        g.setFont(Font (12.00f, Font::plain).withTypefaceStyle("Bold"));
        g.drawText(TRANS("Feedback"), x, 100, width, height, Justification::centred, true);
        g.drawText(TRANS("Gain"), x, 112, width, height, Justification::centred, true);
        g.drawText(TRANS("(dB)"), x, 124, width, height, Justification::centred, true);
    }
    {
        int x = 250, width = 100, height = 32;
        g.setColour(Colours::whitesmoke);
        g.setFont(Font (12.00f, Font::plain).withTypefaceStyle("Bold"));
        g.drawText(TRANS("Wet"), x, 100, width, height, Justification::centred, true);
        g.drawText(TRANS("Gain"), x, 112, width, height, Justification::centred, true);
        g.drawText(TRANS("(dB)"), x, 124, width, height, Justification::centred, true);
    }
    
    // add info for the Plug-In
    {
        String text(TRANS("Ver: ") +
                    JucePlugin_VersionString +
                    BUILD_VER_SUFFIX +
                    TRANS(", Build Date: ") +
                    __DATE__);
        g.setColour(Colours::white);
        g.setFont(Font(11.00f, Font::plain));
        g.drawText(text, getWidth()-200, getHeight()-20, 200, 11, Justification::centredLeft, true);
    }
}

void DelayAudioProcessorEditor::resized()
{
    // define UI areas
    auto area = getLocalBounds();
    auto topArea = area.removeFromTop(sliderYpos);
    auto sliderArea = area.removeFromTop(sliderHeight);
    auto bottomArea = area.removeFromTop(totalHeight - sliderYpos - sliderHeight);
    
    // position sliders
    sliderArea.removeFromLeft(1.5*sliderWidth);
    delayTimeSlider->setBounds(sliderArea.removeFromLeft(sliderWidth));
    sliderArea.removeFromLeft(sliderWidth);
    delayFeedbackSlider->setBounds(sliderArea.removeFromLeft(sliderWidth));
    sliderArea.removeFromLeft(sliderWidth);
    wetGainSlider->setBounds(sliderArea.removeFromLeft(sliderWidth));
}
