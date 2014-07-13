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

typedef struct _pdinstance_list
{
    t_pdinstance* current;
    _pdinstance_list* next;
} t_pdinstance_list;

static t_pdinstance_list* pdinstance_list = NULL;
static int libpd_inited = 0;
static int libpd_audioinited = 0;

void pdinstance_list_add(t_pdinstance* x)
{
    if(!pdinstance_list)
    {
        pdinstance_list = (t_pdinstance_list *)malloc(sizeof(t_pdinstance_list));
        pdinstance_list->current = x;
        pdinstance_list->next    = NULL;
    }
    else
    {
        t_pdinstance_list* pdilist = pdinstance_list;
        while(pdilist)
        {
            if(!pdilist->next)
            {
                pdilist->next = (t_pdinstance_list *)malloc(sizeof(t_pdinstance_list));
                pdilist->next->current = x;
                pdilist->next->next = NULL;
                return;
            }
            pdilist = pdilist->next;
        }
    }
}

void pdinstance_list_remove(t_pdinstance* x)
{
    t_pdinstance_list* temp;
    t_pdinstance_list* pdilist = pdinstance_list;
    if(pdinstance_list->current == x)
    {
        temp = pdinstance_list->next;
        free(pdinstance_list);
        pdinstance_list = temp;
        return;
    }
    while(pdilist)
    {
        if(pdilist->current == x)
        {
            temp = pdilist;
            pdilist = pdilist->next;
            free(temp);
            return;
        }
        pdilist = pdilist->next;
    }
}

void pdinstance_list_startdsp()
{
    t_pdinstance_list* temp;
    t_pdinstance_list* pdilist = pdinstance_list;
    while(pdilist)
    {
        pd_setinstance(pdilist->current);
        libpd_start_message(1);
        libpd_add_float(1.f);
        libpd_finish_message("pd", "dsp");
        pdilist = pdilist->next;
    }
}

void pdinstance_list_stopdsp()
{
    t_pdinstance_list* temp;
    t_pdinstance_list* pdilist = pdinstance_list;
    while(pdilist)
    {
        pd_setinstance(pdilist->current);
        libpd_start_message(1);
        libpd_add_float(0.f);
        libpd_finish_message("pd", "dsp");
        pdilist = pdilist->next;
    }
}

PluginProcessor::PluginProcessor()
{
    m_pd = pdinstance_new();
    
    pdinstance_list_stopdsp();
    pdinstance_list_add(m_pd);
    
    sys_lock();
    if(!libpd_inited)
    {
        libpd_init();
        libpd_loadcream();
        libpd_inited = 1;
    }
    sys_unlock();
    
    m_input_pd  = NULL;
    m_output_pd = NULL;
    m_patch     = NULL;
}

PluginProcessor::~PluginProcessor()
{
    pdinstance_list_stopdsp();
    
    sys_lock();
    pd_setinstance(m_pd);
    if(m_patch)
        libpd_closefile(m_patch);
    sys_unlock();
    
    pdinstance_list_remove(m_pd);
    pdinstance_free(m_pd);
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
    releaseResources();
    m_vector_size = vectorsize;
    m_input_pd = new float[getNumInputChannels() * m_vector_size];
    m_output_pd = new float[getNumOutputChannels() * m_vector_size];
    
    sys_lock();
    pd_setinstance(m_pd);
    if(m_patch)
        libpd_closefile(m_patch);
	m_patch = libpd_openfile("zaza.pd", "/Users/Pierre/Desktop");
    
    if(!libpd_audioinited)
    {
        libpd_init_audio(getNumInputChannels(), getNumOutputChannels(), samplerate);
        libpd_audioinited = 1;
    }
    
    libpd_start_message(1);
    libpd_add_float(1.f);
    libpd_finish_message("pd", "dsp");
    
    sys_unlock();
    
    //pdinstance_list_startdsp();
}

void PluginProcessor::reset()
{
    /*
    if(m_input_pd)
    {
        memset(m_input_pd, 0, getNumInputChannels() * m_vector_size * sizeof(float));
    }
    if(m_output_pd)
    {
        memset(m_output_pd, 0, getNumOutputChannels() * m_vector_size * sizeof(float));
    }*/
}

void PluginProcessor::releaseResources()
{
    //pdinstance_list_stopdsp();
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
    pd_setinstance(m_pd);
	int ticks = m_vector_size / libpd_blocksize();
    for(int i = 0; i < getNumInputChannels(); i++)
    {
        cblas_scopy(m_vector_size, buffer.getReadPointer(i), 1, m_input_pd+i, getNumInputChannels());
    }
    sys_lock();
	libpd_process_float(ticks, m_input_pd, m_output_pd);
	sys_unlock();
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
