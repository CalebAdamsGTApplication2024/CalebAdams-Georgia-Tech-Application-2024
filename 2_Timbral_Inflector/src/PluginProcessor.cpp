#include "PluginProcessor.h"
#include "PluginEditor.h"

TimbralInflectorAudioProcessor::TimbralInflectorAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

TimbralInflectorAudioProcessor::~TimbralInflectorAudioProcessor()
{
}

const juce::String TimbralInflectorAudioProcessor::getName() const
{
    return "Timbral Inflector";
}

bool TimbralInflectorAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TimbralInflectorAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TimbralInflectorAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TimbralInflectorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TimbralInflectorAudioProcessor::getNumPrograms()
{
    return 1;
}

int TimbralInflectorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TimbralInflectorAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String TimbralInflectorAudioProcessor::getProgramName (int index)
{
    return {};
}

void TimbralInflectorAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

// Method is called before playback
void TimbralInflectorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    selfModulatorMode = true;
    currentSampleRate = sampleRate;
    modulatorCurrentSampleRate = sampleRate;
    wetLevel = 0.5;
    cutoffFrequency = 5000.;
    gain = 1.0;
    modulationGain = 1.0;
    currentSampleRate = sampleRate;

    // Calculate coefficients for 2nd Order High Pass Butterworth Filter
    double C = 1 / tan(juce::MathConstants<double>::pi * cutoffFrequency / currentSampleRate);
    double a1 = 1 / (1 + sqrt(2) * C + C * C);
    double a2 = -2 * a1;
    double b1 = 2 * a1 * (C * C - 1);
    double b2 = a1 * (1 - sqrt(2) * C + C * C);

    highPass.setCoefficients(juce::IIRCoefficients::IIRCoefficients(1., a1, a2, 1., b1, b2));

    // Caculate coefficients for Smith-Angell Resonator
    double q = 100.,
        bw = cutoffFrequency / q,
        theta = juce::MathConstants<double>::twoPi * cutoffFrequency / sampleRate;

    b2 = pow(juce::MathConstants<double>::euler, -juce::MathConstants<double>::twoPi * bw / sampleRate);
    b1 = -4 * b2 * cos(theta) / (1 + b2);
    a1 = 1 - sqrt(b2);
    a2 = -a1;

    resonator.setCoefficients(juce::IIRCoefficients::IIRCoefficients(1., a1, a2, 1., b1, b2));

    updateAngleDelta();
}

void TimbralInflectorAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TimbralInflectorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

// Buffer Processing
void TimbralInflectorAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    int numSamples = buffer.getNumSamples();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, numSamples);

    auto* inLeftBuffer = buffer.getReadPointer(0);
    auto* inRightBuffer = buffer.getReadPointer(1);
    auto* outLeftBuffer = buffer.getWritePointer(0);
    auto* outRightBuffer = buffer.getWritePointer(1);

    float* wetComponentL = new float[numSamples];
    float* wetComponentR = new float[numSamples];
    float* modulatorCurrentDataL = new float[numSamples];
    float* modulatorCurrentDataR = new float[numSamples];

    // Intialize Filters with incoming buffer
    for (auto sample = 0; sample < numSamples; ++sample)
    {
        wetComponentL[sample] = inLeftBuffer[sample];
        wetComponentR[sample] = inRightBuffer[sample];
        modulatorCurrentDataL[sample] = inLeftBuffer[sample];
        modulatorCurrentDataR[sample] = inRightBuffer[sample];
    }

    if (selfModulatorMode == true)
    {
        resonator.processSamples(modulatorCurrentDataL, numSamples);
        resonator.processSamples(modulatorCurrentDataR, numSamples);
    }

    // Create wet component of final buffer
    for (auto sample = 0; sample < numSamples; ++sample)
    {        
        if (selfModulatorMode == true)
        {
            wetComponentL[sample] = (inLeftBuffer[sample] * modulatorCurrentDataL[sample] * wetLevel) * modulationGain; 
            wetComponentR[sample] = (inRightBuffer[sample] * modulatorCurrentDataR[sample] * wetLevel) * modulationGain; 
        }
        else 
        {
            auto modulatorCurrentSample = (float)std::sin(modulatorCurrentAngle);
            modulatorCurrentAngle += modulatorAngleDelta;

            wetComponentL[sample] = (inLeftBuffer[sample] * modulatorCurrentSample * wetLevel) * modulationGain;
            wetComponentR[sample] = (inRightBuffer[sample] * modulatorCurrentSample * wetLevel) * modulationGain;
        }
    }   

    highPass.processSamples(wetComponentL, numSamples);
    highPass.processSamples(wetComponentR, numSamples);

    // Combine wet and dry components into the final output buffer
    for (auto sample = 0; sample < numSamples; ++sample)
    {
        outLeftBuffer[sample] = (wetComponentL[sample] + (inLeftBuffer[sample] * (1.0 - wetLevel))) * gain;
        outRightBuffer[sample] = (wetComponentR[sample] + (inRightBuffer[sample] * (1.0 - wetLevel))) * gain;
    }
}

bool TimbralInflectorAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* TimbralInflectorAudioProcessor::createEditor()
{
    return new TimbralInflectorAudioProcessorEditor(*this, currentSampleRate);
}

void TimbralInflectorAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
}

void TimbralInflectorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TimbralInflectorAudioProcessor();
}

// Generate next angle delta for the modulating signal when smMode is off
void TimbralInflectorAudioProcessor::updateAngleDelta()
{
    auto modulatorCyclesPerSample = 5000. / modulatorCurrentSampleRate;
    modulatorAngleDelta = modulatorCyclesPerSample * 2.0 * juce::MathConstants<double>::pi;
}

void TimbralInflectorAudioProcessor::wetChanged(float newWet)
{
    wetLevel = newWet;
}

void TimbralInflectorAudioProcessor::gainChanged(float newGain)
{
    gain = newGain;
}

void TimbralInflectorAudioProcessor::modulationGainChanged(float newModulationGain)
{
    modulationGain = newModulationGain;
}

void TimbralInflectorAudioProcessor::modeChanged(bool newMode)
{
    selfModulatorMode = newMode;
}

void TimbralInflectorAudioProcessor::cutoffChanged(double newCutoff)
{
    cutoffFrequency = newCutoff;
    
    // Recalculate filter coefficients according to new Fc
    double C = 1 / tan(juce::MathConstants<double>::pi * cutoffFrequency / currentSampleRate);
    double a1 = 1 / (1 + sqrt(2) * C + C * C);
    double a2 = -2 * a1;
    double b1 = 2 * a1 * (C * C - 1);
    double b2 = a1 * (1 - sqrt(2) * C + C * C);

    highPass.setCoefficients(juce::IIRCoefficients::IIRCoefficients(1., a1, a2, 1., b1, b2));

    double q = 100.,
        bw = cutoffFrequency / q,
        theta = juce::MathConstants<double>::twoPi * cutoffFrequency / currentSampleRate;

    b2 = pow(juce::MathConstants<double>::euler, -juce::MathConstants<double>::twoPi * bw / currentSampleRate);
    b1 = -4 * b2 * cos(theta) / (1 + b2);
    a1 = 1 - sqrt(b2);
    a2 = -a1;

    resonator.setCoefficients(juce::IIRCoefficients::IIRCoefficients(1., a1, a2, 1., b1, b2));
}