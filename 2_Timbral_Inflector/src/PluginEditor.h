#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class TimbralInflectorAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    TimbralInflectorAudioProcessorEditor(TimbralInflectorAudioProcessor&, double currentSampleRate);
    ~TimbralInflectorAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void updateModeState();

private:
    TimbralInflectorAudioProcessor& audioProcessor;

    juce::Slider wetSlider;
    juce::Label wetLabel;

    juce::Slider gainSlider;
    juce::Label gainLabel;
    juce::LookAndFeel_V4 gainLook;

    juce::Slider modulationGainSlider;
    juce::Label modulationGainLabel;
    juce::LookAndFeel_V4 modulationGainLook;

    juce::Slider cutoffSlider;
    juce::Label cutoffLabel;
    juce::LookAndFeel_V4 cutoffLook;
    
    juce::TextButton smMode{"Self Modulation Mode (On)"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TimbralInflectorAudioProcessorEditor)
};
