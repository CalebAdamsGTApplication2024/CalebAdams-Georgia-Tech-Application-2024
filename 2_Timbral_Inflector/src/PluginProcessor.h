#pragma once
#include <JuceHeader.h>
#include <algorithm>

class TimbralInflectorAudioProcessor  : public juce::AudioProcessor  
{
public:
    TimbralInflectorAudioProcessor();
    ~TimbralInflectorAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void updateAngleDelta();
    void wetChanged(float newWet);
    void gainChanged(float newGain);
    void modulationGainChanged(float newModulationGain);
    void modeChanged(bool newMode);
    void cutoffChanged(double newCutoff);

private:
    juce::Random random;

    juce::IIRFilter highPass;
    juce::IIRFilter resonator;
    
    double currentSampleRate = 0.0;
    double cutoffFrequency;

    double modulatorCurrentSampleRate = 0.0, modulatorCurrentAngle = 0.0, modulatorAngleDelta = 0.0;

    bool selfModulatorMode;
    float wetLevel;
    float gain;
    float modulationGain;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TimbralInflectorAudioProcessor)
};

