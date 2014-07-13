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

static int libpd_inited = 0;
static int libpd_audioinited = 0;


PluginProcessor::PluginProcessor()
{
    if(!libpd_inited)
    {
        libpd_init();
        libpd_loadcream();
        libpd_inited = 1;
    }
    
    m_scope.enter();
    m_pd = pdinstance_new();
    pd_setinstance(m_pd);
    libpd_start_message(1);
    libpd_add_float(1.f);
    libpd_finish_message("pd", "dsp");
    m_scope.exit();
    
    m_input_pd  = NULL;
    m_output_pd = NULL;
    m_patch     = NULL;
}

PluginProcessor::~PluginProcessor()
{
    m_scope.enter();
    pd_setinstance(m_pd);
    if(m_patch)
        libpd_closefile(m_patch);
    pdinstance_free(m_pd);
    m_scope.exit();
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

int zjzjjz = 0;

t_pd *glob_evalfile2(t_pd *ignore, t_symbol *name, t_symbol *dir)
{
    t_pd *x = 0;
    /* even though binbuf_evalfile appears to take care of dspstate,
     we have to do it again here, because canvas_startdsp() assumes
     that all toplevel canvases are visible.  LATER check if this
     is still necessary -- probably not. */
    
    int dspstate = canvas_suspend_dsp();
    t_pd *boundx = s__X.s_thing;
    s__X.s_thing = 0;       /* don't save #X; we'll need to leave it bound
                             for the caller to grab it. */
    binbuf_evalfile(name, dir);
    while ((x != s__X.s_thing) && s__X.s_thing)
    {
        x = s__X.s_thing;
        vmess(x, gensym("pop"), "i", 1);
    }
    canvas_resume_dsp(dspstate);
    s__X.s_thing = boundx;
    return x;
}

void PluginProcessor::prepareToPlay(double samplerate, int vectorsize)
{
    releaseResources();
    m_vector_size = vectorsize;
    m_input_pd = new float[getNumInputChannels() * m_vector_size];
    m_output_pd = new float[getNumOutputChannels() * m_vector_size];
    
    m_scope.enter();
    pd_setinstance(m_pd);
    if(m_patch)
        libpd_closefile(m_patch);
    if(!zjzjjz)
    {
        m_patch = libpd_openfile("zaza.pd", "/Users/Pierre/Desktop");
    }
    else
    {
       m_patch =  glob_evalfile2(NULL, gensym("zaza.pd"), gensym("/Users/Pierre/Desktop"));
    }
	
    m_scope.exit();
    
    if(!libpd_audioinited)
    {
        libpd_init_audio(getNumInputChannels(), getNumOutputChannels(), samplerate);
        libpd_audioinited = 1;
    }
}

void PluginProcessor::reset()
{
    if(m_input_pd)
    {
        memset(m_input_pd, 0, getNumInputChannels() * m_vector_size * sizeof(float));
    }
    if(m_output_pd)
    {
        memset(m_output_pd, 0, getNumOutputChannels() * m_vector_size * sizeof(float));
    }
}

void PluginProcessor::releaseResources()
{
    if(m_input_pd)
    {
        delete [] m_input_pd;
        m_input_pd = NULL;
    }
    if(m_output_pd)
    {
        delete [] m_output_pd;
        m_output_pd = NULL;
    }
}

void PluginProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    for(int i = 0; i < getNumInputChannels(); i++)
    {
        cblas_scopy(m_vector_size, buffer.getReadPointer(i), 1, m_input_pd+i, getNumInputChannels());
    }
    
    m_scope.enter();
    pd_setinstance(m_pd);
    int ticks = m_vector_size / libpd_blocksize();
	libpd_process_float(ticks, m_input_pd, m_output_pd);
    m_scope.exit();

    for(int i = 0; i < getNumOutputChannels(); i++)
    {
        cblas_scopy(m_vector_size, m_output_pd+i, getNumOutputChannels(), buffer.getWritePointer(i), 1);
    }
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
