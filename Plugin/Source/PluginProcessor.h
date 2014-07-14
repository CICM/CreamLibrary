/*
 * CicmWrapper
 *
 * A wrapper for Pure Data
 *
 * Copyright (C) 2013 Pierre Guillot, CICM - Université Paris 8
 * All rights reserved.
 *
 * Website  : http://www.mshparisnord.fr/HoaLibrary/
 * Contacts : cicm.mshparisnord@gmail.com
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published
 * by the Free Software Foundation; either version 2 of the License.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#ifndef CREAM_PLUGIN_PROCESSOR
#define CREAM_PLUGIN_PROCESSOR

#include "../JuceLibraryCode/JuceHeader.h"
extern "C"
{
#include "epd_ugen.h"
}
#include "../../c.library.h"

class PluginProcessor  : public AudioProcessor//, public PdReceiver
{
public:
    PluginProcessor();
    ~PluginProcessor();

    void prepareToPlay(double samplerate, int vectorsize) override;
    void releaseResources() override;
    void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages) override;
    void reset() override;

    bool hasEditor() const override                  { return true; }
    AudioProcessorEditor* createEditor() override;

    const String getName() const override            { return JucePlugin_Name; }

    int getNumParameters() override;
    float getParameter(int index) override;
    float getParameterDefaultValue(int index) override;
    void setParameter(int index, float value) override;
    const String getParameterName(int index) override;
    const String getParameterText(int index) override;

    const String getInputChannelName(int index) const override;
    const String getOutputChannelName(int index) const override;
    bool isInputChannelStereoPair(int index) const override;
    bool isOutputChannelStereoPair(int index) const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool silenceInProducesSilenceOut() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override                                               { return 1; }
    int getCurrentProgram() override                                            { return 0; }
    void setCurrentProgram(int index) override                                  {}
    const String getProgramName(int index) override                             { return "Camomile"; }
    void changeProgramName(int index, const String& name) override              {}

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
private:
    t_epd_process*  m_process;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};

#endif
