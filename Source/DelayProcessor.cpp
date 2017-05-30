/*

"DelayProcessor" class definition.

This class controls the plugins core audio processing.
Parent class of the "DelayEditor" class.

Date: 29/03/2017
Plugin Name: Delay
Author: Dimitris Koutsaidis

to do:

*/


#include "JuceHeader.h"
#include "DelayEditor.h"
#include <math.h> 


class DelayProcessor : public AudioProcessor
{
public:

	DelayProcessor() : parameters(*this, nullptr)
    {
        parameters.createAndAddParameter ("tDelay", "Delay (s)", String(), NormalisableRange<float> (0, 200, 1), 0, nullptr, nullptr);
        
		parameters.state = ValueTree(Identifier("Delay"));
    }

	~DelayProcessor() {}

    void prepareToPlay (double, int) override {}
    
    void releaseResources() override {}

	void processBlock(AudioSampleBuffer& buffer, MidiBuffer&) override
	{
		// Buffer Parameters
		int numSamples = buffer.getNumSamples();
		double sampleRate = getSampleRate();
		float* const leftChannelDataOut = buffer.getWritePointer(0);


		// Delay Parameters
		tDelay = *parameters.getRawParameterValue("tDelay");
		const float BL = 1.0f;
		const float FB = 0.5f;
		const float FF = 0.25f;


		// Change Delay in Number of Samples
		if (tDelay != previoustDelay) M = int(round((tDelay/1000)*sampleRate));


		// Delay Implementation
		if (M != 0)
		{
			if (tDelay != previoustDelay)
			{
				std::vector<float> Delayline(M, 0);
				previoustDelay = tDelay;
			}

			for (int i = 0; i < numSamples; ++i)
			{
				float leftChannelDataIn = buffer.getReadPointer(0)[i];
				float H = leftChannelDataIn + FB*Delayline[M-2];

				leftChannelDataOut[i] = FF*Delayline[M-2] + BL*H;

				std::rotate(Delayline.rbegin(), Delayline.rbegin() + 1, Delayline.rend());
				Delayline[0] = H;
			}
		}
    }

	AudioProcessorEditor* createEditor() override         { return new DelayEditor(*this, parameters); }
    bool hasEditor() const override                       { return true;   }
	const String getName() const override                 { return "Delay"; }
    bool acceptsMidi() const override                     { return false; }
    bool producesMidi() const override                    { return false; }
    double getTailLengthSeconds() const override          { return 0; }
	int getNumPrograms() override                         { return 1; }
    int getCurrentProgram() override                      { return 0; }
    void setCurrentProgram (int) override                 { }
    const String getProgramName (int) override            { return String(); }
    void changeProgramName (int, const String& ) override { }

    void getStateInformation (MemoryBlock& destData) override
    {
        ScopedPointer<XmlElement> xml (parameters.state.createXml());
        copyXmlToBinary (*xml, destData);
    }
    
    void setStateInformation (const void* data, int sizeInBytes) override
    {
        ScopedPointer<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
        
        if (xmlState != nullptr)
            if (xmlState->hasTagName (parameters.state.getType()))
                parameters.state = ValueTree::fromXml (*xmlState);
    }
       
private:
    AudioProcessorValueTreeState parameters;

	float previoustDelay, tDelay;
    int M;
	std::vector<float> Delayline;
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayProcessor)
};


AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new DelayProcessor();
}