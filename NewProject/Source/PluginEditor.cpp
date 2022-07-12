/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NewProjectAudioProcessorEditor::NewProjectAudioProcessorEditor (NewProjectAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
    addAndMakeVisible(mLeft);
    addAndMakeVisible(mRight);
    addAndMakeVisible(collapse);

	//Phase buttons
	addAndMakeVisible(rPhase);
	addAndMakeVisible(lPhase);

	addAndMakeVisible(lFader);
	addAndMakeVisible(rFader);

    mLeft.setButtonText("M");
    mRight.setButtonText("M");
    collapse.setButtonText("Mono");

	rPhase.setButtonText("Ø");
	lPhase.setButtonText("Ø");

	rFader.setRange(-48.0f, 1.0f);
	lFader.setRange(-48.0f, 1.0f);


	//Right Fader
	rFader.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
	rFader.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 25);
	rFader.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
	
	rFader.setValue(0.0);
	rFader.addListener(this);

	//Left Fader
	lFader.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
	lFader.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 25);
	lFader.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);

	lFader.setValue(0.0);
	lFader.addListener(this);

	mRight.setClickingTogglesState(true);
	mLeft.setClickingTogglesState(true);
	collapse.setClickingTogglesState(true);
	rPhase.setClickingTogglesState(true);
	lPhase.setClickingTogglesState(true);

	mRight.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::yellow);
	mLeft.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::yellow);
	collapse.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::green);
	lPhase.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::yellow);
	rPhase.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::yellow);

	mLeft.onStateChange = [this]
	{
		
		audioProcessor.lMuted = mLeft.getToggleState();
	};

	mRight.onStateChange = [this]
	{
		/*bool press = mRight.getToggleState();
		mRight.setToggleState(!press, juce::NotificationType::dontSendNotification);
		audioProcessor.rMuted = !audioProcessor.rMuted;*/

		audioProcessor.rMuted = mRight.getToggleState();
	};

	collapse.onStateChange = [this] {

		audioProcessor.monoBtnPressed = collapse.getToggleState();
	};

	rPhase.onStateChange = [this] {

		audioProcessor.phaseR = rPhase.getToggleState();
	};


	lPhase.onStateChange = [this] {

		audioProcessor.phaseL = lPhase.getToggleState();
	};
    
	rSlidAttach = std::make_unique< juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treestate, "RIGHTFADER", rFader);
	lSlidAttach = std::make_unique< juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treestate, "LEFTFADER", lFader);
	btnRAttach = std::make_unique< juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.treestate, "MUTERIGHT", mRight);
	btnLAttach = std::make_unique< juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.treestate, "MUTELEFT", mLeft);
	btnSumAttach = std::make_unique< juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.treestate, "SUM", collapse);
	btnPhaseRAttach = std::make_unique< juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.treestate, "rPhase", rPhase);
	btnPhaseLAttach = std::make_unique< juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.treestate, "lPhase", lPhase);

	setSize (400, 300);
}

NewProjectAudioProcessorEditor::~NewProjectAudioProcessorEditor()
{
}

//==============================================================================
void NewProjectAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

	//Image to memory
	juce::Image bgimg = juce::ImageCache::getFromMemory(BinaryData::Unreached2_png, BinaryData::Unreached2_pngSize);

	//Draw image
	g.drawImageAt(bgimg, 0,0);

    g.setColour (juce::Colours::black);
    g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void NewProjectAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

	mLeft.setBounds(10, 10, 50, 50);

	mRight.setBounds(340, 10, 50, 50);
	collapse.setBounds(getWidth()/2-25, 10, 50, 50);

	lFader.setBounds(10, 70, 50, 200);
	rFader.setBounds(340, 70, 50, 200);

	lPhase.setBounds(70, 250, 25, 25);
	rPhase.setBounds(290, 250, 25, 25);
}

void NewProjectAudioProcessorEditor::setMute(bool *b) {
	*b = !*b;
}

void NewProjectAudioProcessorEditor::sliderValueChanged(juce::Slider *slider)
{
	if (slider == &rFader) {
		audioProcessor.rVolume = slider->getValue();// juce::Decibels::decibelsToGain(slider->getValue());//pow(10, slider->getValue() / 20);
	}
	else if (slider == &lFader) {
		audioProcessor.lVolume = slider->getValue();// juce::Decibels::decibelsToGain(slider->getValue());  //pow(10, slider->getValue() / 20);
	}

	
}

/*void NewProjectAudioProcessorEditor::toggleButtonColor(juce::TextButton *button) 
{

	button->setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::yellow)

}*/

