/*

"DelayEditor" class definition.

This class controls the plugins GUI.
Child class of the "AudioProcessor" class.

Date: 29/03/2017
Plugin Name: Delay
Author: Dimitris Koutsaidis

to do:

*/


class DelayEditor : public AudioProcessorEditor
{
public:
    enum
    {
        paramControlHeight = 40,
        paramLabelWidth    = 100,
        paramSliderWidth   = 300
    };

    typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    
	DelayEditor(AudioProcessor& parent, AudioProcessorValueTreeState& vts) : AudioProcessorEditor(parent), valueTreeState(vts)
    {
		tDelayLabel.setText("Delay (s)", dontSendNotification);
		addAndMakeVisible(tDelayLabel);
        
		addAndMakeVisible(tDelaySlider);
		tDelayAttachment = new SliderAttachment(valueTreeState, "tDelay", tDelaySlider);
        
        setSize (paramSliderWidth + paramLabelWidth, paramControlHeight * 2);
    }

	~DelayEditor() {}

    void resized() override
    {
        Rectangle<int> r = getLocalBounds();
        {
			Rectangle<int> tDelayRect = r.removeFromTop(paramControlHeight);
			tDelayLabel.setBounds(tDelayRect.removeFromLeft(paramLabelWidth));
			tDelaySlider.setBounds(tDelayRect);
        }
    }

    void paint (Graphics& g) override
    {
        g.setColour (Colours::white);
        g.fillRect (getLocalBounds());
    }
    
private:
    AudioProcessorValueTreeState& valueTreeState;
    
	Label tDelayLabel;
	Slider tDelaySlider;
	ScopedPointer<SliderAttachment> tDelayAttachment;
};