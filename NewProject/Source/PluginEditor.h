/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class NewProjectAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Slider::Listener
{
public:
    NewProjectAudioProcessorEditor (NewProjectAudioProcessor&);
    ~NewProjectAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    juce::TextButton mLeft, mRight, collapse, rPhase, lPhase;
	juce::Slider lFader, rFader;

	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> btnLAttach, btnRAttach, btnSumAttach, btnPhaseRAttach, btnPhaseLAttach;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lSlidAttach, rSlidAttach;

	void sliderValueChanged(juce::Slider *slider) override;
	double getValueFromText(const juce::String& text);
	
	
	void setMute(bool *b);
	//void toggleButtonColor(juce::TextButton *button);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    NewProjectAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewProjectAudioProcessorEditor)
};
