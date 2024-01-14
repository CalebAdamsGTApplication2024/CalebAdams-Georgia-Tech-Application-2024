#include "PluginProcessor.h"
#include "PluginEditor.h"

TimbralInflectorAudioProcessorEditor::TimbralInflectorAudioProcessorEditor(TimbralInflectorAudioProcessor& p, double currentSampleRate)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setOpaque(true);
    setSize (700, 500);

    // Configure wet/dry dial
    addAndMakeVisible(wetSlider);
    wetSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    wetSlider.getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::aqua);
    wetSlider.setRange(0., 1.);
    wetLabel.setText("Wet/Dry", juce::dontSendNotification);
    wetLabel.attachToComponent(&wetSlider, true);
    wetSlider.onValueChange = [this]{ audioProcessor.wetChanged(wetSlider.getValue()); };
    wetSlider.setValue(0.5);

    // Configure gain dial
    addAndMakeVisible(gainSlider);
    gainSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    gainLook.setColour(juce::Slider::thumbColourId, juce::Colours::red);
    gainSlider.setLookAndFeel(&gainLook);
    gainSlider.setRange(0., 5.);
    gainLabel.setText("Gain", juce::dontSendNotification);
    gainLabel.attachToComponent(&gainSlider, true);
    gainSlider.onValueChange = [this]{ audioProcessor.gainChanged(gainSlider.getValue()); };
    gainSlider.setValue(1.0);

    // Configure modulation gain dial
    addAndMakeVisible(modulationGainSlider);
    modulationGainSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    modulationGainLook.setColour(juce::Slider::thumbColourId, juce::Colours::orange);
    modulationGainSlider.setLookAndFeel(&modulationGainLook);
    modulationGainSlider.setRange(0., 5.);
    modulationGainLabel.setText("Modulation Gain", juce::dontSendNotification);
    modulationGainLabel.attachToComponent(&modulationGainSlider, true);
    modulationGainSlider.onValueChange = [this]{ audioProcessor.modulationGainChanged(modulationGainSlider.getValue()); };
    modulationGainSlider.setValue(1.0);

    // Configure Fc dial
    addAndMakeVisible(cutoffSlider);
    cutoffSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    cutoffLook.setColour(juce::Slider::thumbColourId, juce::Colours::fuchsia);
    cutoffSlider.setLookAndFeel(&cutoffLook);
    cutoffSlider.setRange(0., 2. * currentSampleRate / 5. );
    cutoffLabel.setText("Cutoff Frequency", juce::dontSendNotification);
    cutoffLabel.attachToComponent(&cutoffSlider, true);
    cutoffSlider.onValueChange = [this] { audioProcessor.cutoffChanged(cutoffSlider.getValue()); };
    cutoffSlider.setValue(5000.);

    // Configure smMode button
    addAndMakeVisible(smMode);
    smMode.setToggleState(true, juce::dontSendNotification);
    smMode.setClickingTogglesState(true);
    smMode.onClick = [this]{ updateModeState(); };
}

TimbralInflectorAudioProcessorEditor::~TimbralInflectorAudioProcessorEditor()
{
}

void TimbralInflectorAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    g.setOpacity(1.0f);
}

void TimbralInflectorAudioProcessorEditor::resized()
{
    auto border = 4;
    auto area = getLocalBounds();
    auto dialArea = area.removeFromTop(area.getHeight() / 2);
    gainSlider.setBounds(80,5, 200, 200);
    modulationGainSlider.setBounds(420, 5, 200, 200);
    wetSlider.setBounds(420, 170, 200, 200);
    cutoffSlider.setBounds(80, 170, 200, 200);
    smMode.setBounds(262, 360, 200, 60);
}

void TimbralInflectorAudioProcessorEditor::updateModeState()
{
    bool state = smMode.getToggleState();
    juce::String stateString = state ? "Self Modulation (On)" : "Self Modulation (Off)";
    smMode.setButtonText(stateString);
    audioProcessor.modeChanged(state);
}
