/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NewProjectAudioProcessor::NewProjectAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
	), treestate(*this, nullptr, "PARAMETERS", {
												std::make_unique<juce::AudioParameterFloat>("RIGHTFADER", "rightfader", -48.0f, 1.0f, 0.0f),
												std::make_unique<juce::AudioParameterFloat>("LEFTFADER", "leftfader", -48.0f, 1.0f, 0.0f),
												std::make_unique<juce::AudioParameterBool>("MUTELEFT", "Mute Left", false , "LeftFaderParam", nullptr, nullptr),
												std::make_unique<juce::AudioParameterBool>("MUTERIGHT", "muteright", false, "RightFaderParam", nullptr, nullptr),
												std::make_unique<juce::AudioParameterBool>("SUM", "sum", false, "SummingParam", nullptr, nullptr),
												std::make_unique<juce::AudioParameterBool>("rPhase", "PhaseRight", false, "PhaseR", nullptr, nullptr),
												std::make_unique<juce::AudioParameterBool>("lPhase", "PhaseLeft", false, "PhaseR", nullptr, nullptr)
												})
#endif
{
	lVolume = 0.0;
	rVolume = 0.0;
	
	monoBtnPressed = false;
	rMuted = false;
	lMuted = false;
	phaseR = false;
	phaseL = false;
	
	//treestate.state = juce::ValueTree("SaveData");
	
}

NewProjectAudioProcessor::~NewProjectAudioProcessor()
{
}

//==============================================================================
const juce::String NewProjectAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool NewProjectAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool NewProjectAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool NewProjectAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double NewProjectAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int NewProjectAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int NewProjectAudioProcessor::getCurrentProgram()
{
    return 0;
}

void NewProjectAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String NewProjectAudioProcessor::getProgramName (int index)
{
    return {};
}

void NewProjectAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void NewProjectAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void NewProjectAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool NewProjectAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void NewProjectAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

	

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.

	float* channelDataL = buffer.getWritePointer(0);
	float* channelDataR = buffer.getWritePointer(1);

	

	if (lMuted) buffer.clear(0, 0, buffer.getNumSamples());
	if (rMuted) buffer.clear(1,0,buffer.getNumSamples());

	if (phaseL)
		buffer.applyGain(0, 0, buffer.getNumSamples(), -1.0);// channelDataL[sample] * -1.0;
	if (phaseR)
		buffer.applyGain(1, 0, buffer.getNumSamples(), -1.0);

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {

		

		for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
		{




			if (!lMuted) {
				
				channelDataL[sample] = channelDataL[sample] * juce::Decibels::decibelsToGain(lVolume);
				if (phaseL) buffer.applyGain(0, buffer.getNumSamples(), -1);
			}
			if (!rMuted) {
				channelDataR[sample] = channelDataR[sample] * juce::Decibels::decibelsToGain(rVolume);
			}
			


			if (monoBtnPressed) {

				int div;

				if(lMuted || rMuted)
					div = 1;
				else div = 2;

				float tmpMono = (channelDataL[sample] + channelDataR[sample])/div;//Sum channels
				channelDataL[sample] = tmpMono;
				channelDataR[sample] = tmpMono;

				//buffer.applyGain(-2.0);
			}

			
		}

		
    }
}

//==============================================================================
bool NewProjectAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* NewProjectAudioProcessor::createEditor()
{
    return new NewProjectAudioProcessorEditor (*this);
}

//==============================================================================
void NewProjectAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    std::unique_ptr<juce::XmlElement> xml = treestate.state.createXml();
    xml->setAttribute("rSlider", rVolume);
    xml->setAttribute("lSlider", lVolume);
    xml->setAttribute("MuteL", lMuted);
    xml->setAttribute("MuteR", rMuted);
    xml->setAttribute("Sum", monoBtnPressed);
	xml->setAttribute("phaseR", phaseR);
	xml->setAttribute("phaseL", phaseL);
    copyXmlToBinary(*xml, destData);
}

void NewProjectAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    std::unique_ptr<juce::XmlElement> newParams = getXmlFromBinary(data, sizeInBytes);
    
    if (newParams != nullptr) {
        if (newParams->hasTagName(treestate.state.getType())) {
            treestate.state = juce::ValueTree::fromXml(*newParams);
            rVolume = newParams->getDoubleAttribute("rSlider", -1.0);
            lVolume = newParams->getDoubleAttribute("lSlider", -1.0);
            lMuted = newParams->getBoolAttribute("MuteL", false);
            rMuted = newParams->getBoolAttribute("MuteR", false);
            monoBtnPressed = newParams->getBoolAttribute("Sum", false);
			phaseL = newParams->getBoolAttribute("phaseL", false);
			phaseR = newParams->getBoolAttribute("phaseR", false);
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NewProjectAudioProcessor();
}

