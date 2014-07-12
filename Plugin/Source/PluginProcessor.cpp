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

#include "PluginProcessor.h"
#include "PluginEditor.h"

AudioProcessor* JUCE_CALLTYPE createPluginFilter();

PluginProcessor::PluginProcessor()
{
    ;
}

PluginProcessor::~PluginProcessor()
{
    m_pd.clear();
}

int PluginProcessor::getNumParameters()
{
    return 0;
}

float PluginProcessor::getParameter(int index)
{
    return 0;
}

void PluginProcessor::setParameter(int index, float value)
{
    ;
}

float PluginProcessor::getParameterDefaultValue(int index)
{
    return 0;
}

const String PluginProcessor::getParameterName(int index)
{
    return String::empty;
}

const String PluginProcessor::getParameterText(int index)
{
    return String((float)0, 2);
}

void PluginProcessor::prepareToPlay(double samplerate, int vectorsize)
{
    m_pd.init(0, 2, samplerate);
    Patch p1 = m_pd.openPatch("zaza.pd", "/Users/Pierre/Desktop");
   if(p1.isValid())
       std::cout << "yes ... p1 is opened" << std::endl;
    else
        std::cout << "aww ... p1 couldn't be opened" << std::endl;
    
    m_juceIn = new float*[getNumInputChannels()];
    for(int i = 0; i < getNumInputChannels(); ++i)
        m_juceIn[i] = new float[vectorsize];
    m_juceOut = new float*[getNumOutputChannels()];
    for(int i = 0; i < getNumOutputChannels(); ++i)
        m_juceOut[i] = new float[vectorsize];
    m_pdIn = new float[getNumInputChannels() * vectorsize];
    m_pdOut = new float[getNumOutputChannels() * vectorsize];
    
    m_vector_size = vectorsize;
    m_ticks = vectorsize / m_pd.blockSize();
}

void PluginProcessor::releaseResources()
{
    for(int i = 0; i < getNumInputChannels(); ++i)
        delete [] m_juceIn[i];
    delete [] m_juceIn;
    for(int i = 0; i < getNumOutputChannels(); ++i)
        delete [] m_juceOut[i];
    delete [] m_juceOut;
    delete [] m_pdIn;
    delete [] m_pdOut;
    m_juceIn = NULL;
    m_juceOut = NULL;
    m_pdIn = NULL;
    m_pdOut = NULL;
}

void PluginProcessor::reset()
{
    // Use this method as the place to clear any delay lines, buffers, etc, as it
    // means there's been a break in the audio's continuity.
}

void PluginProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    m_pd.processFloat(m_ticks, m_pdIn, m_pdOut);
    for(int i = 0; i < m_vector_size * getNumOutputChannels(); ++i)
    {
        for (int channel = 0; channel < getNumOutputChannels(); ++channel)
            m_juceOut[channel][i / getNumOutputChannels()] = m_pdOut[i];
    }
    for(int channel = 0; channel < getNumOutputChannels(); ++channel)
        buffer.copyFrom(channel, 0, m_juceOut[channel], m_vector_size);
    
    m_pd.computeAudio(1);
}

AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor (this);
}

void PluginProcessor::getStateInformation(MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // Here's an example of how you can use XML to make it easy and more robust:

    // Create an outer XML element..
    XmlElement xml ("MYPLUGINSETTINGS");
    // add some attributes to it..
    xml.setAttribute ("attrname", 0);
    // then use this helper function to stuff it into the binary blob and return it..
    copyXmlToBinary (xml, destData);
}

void PluginProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    // This getXmlFromBinary() helper function retrieves our XML from the binary blob..
    ScopedPointer<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    int zaza;
    if (xmlState != nullptr)
    {
        // make sure that it's actually our type of XML object..
        if(xmlState->hasTagName ("MYPLUGINSETTINGS"))
        {
            // ok, now pull out our parameters..
            zaza  = xmlState->getIntAttribute ("uiWidth", zaza);
        }
    }
}

const String PluginProcessor::getInputChannelName(const int index) const
{
    return String(index + 1);
}

const String PluginProcessor::getOutputChannelName(const int index) const
{
    return String(index + 1);
}

bool PluginProcessor::isInputChannelStereoPair(int index) const
{
    return false;
}

bool PluginProcessor::isOutputChannelStereoPair(int index) const
{
    return false;
}

bool PluginProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::silenceInProducesSilenceOut() const
{
    return false;
}

double PluginProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}
