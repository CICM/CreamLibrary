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

#include "../c.library.h"

#ifdef __APPLE__
#include <Accelerate/Accelerate.h>

typedef unsigned long ulong;

template <typename T> class FftForward
{
    const ulong         m_size;
    DSPSplitComplex     m_complex;
    FFTSetup            m_fft_setup;
    
    FftForward(const ulong size) :
    m_size(size)
    {
        m_fft_setup = vDSP_create_fftsetup(14, 1 << 14);
        m_complex.realp = new float[m_size];
        m_complex.imagp = new float[m_size];
    }

    ~FftForward()
    {
        delete [] m_complex.realp;
        delete [] m_complex.imagp;
        vDSP_destroy_fftsetup(m_fft_setup);
    }
    
    void process()
    {
        
    }
};

#endif

typedef struct  _spectroscope
{
	t_edspbox   j_box;

	t_clock*	f_clock;
	int			f_startclock;
    long        f_interval;
    
	t_sample*   f_buffer_real;
    t_sample*   f_buffer_imag;
    t_sample*   f_buffer_spectrum;
    char        f_real_mode;
    long        f_buffer_size;
    
	t_rgba		f_color_background;
	t_rgba		f_color_border;
	t_rgba		f_color_spectrum;
} t_spectroscope;

t_eclass *cspectroscope_class;

void *spectroscope_new(t_symbol *s, int argc, t_atom *argv);
void spectroscope_free(t_spectroscope *x);
void spectroscope_assist(t_spectroscope *x, void *b, long m, long a, char *s);

void spectroscope_dsp(t_spectroscope *x, t_object *dsp, short *count, double samplerate, long maxvectorsize, long flags);
void spectroscope_tick(t_spectroscope *x);

t_pd_err spectroscope_notify(t_spectroscope *x, t_symbol *s, t_symbol *msg, void *sender, void *data);

void spectroscope_getdrawparams(t_spectroscope *x, t_object *patcherview, t_edrawparams *params);
void spectroscope_oksize(t_spectroscope *x, t_rect *newrect);
void spectroscope_paint(t_spectroscope *x, t_object *view);

t_symbol* csym_spectrum_layer = gensym("spectrum_layer");
t_symbol* csym_background_layer = gensym("background_layer");

extern "C" void setup_c0x2espectroscope_tilde(void)
{
	t_eclass *c;

	c = eclass_new("c.spectroscope~", (method)spectroscope_new, (method)spectroscope_free, (short)sizeof(t_spectroscope), 0L, A_GIMME, 0);

	eclass_dspinit(c);
	eclass_init(c, 0);
    cream_initclass(c);

	eclass_addmethod(c, (method) spectroscope_dsp,             "dsp",              A_NULL, 0);
	eclass_addmethod(c, (method) spectroscope_assist,          "assist",           A_NULL, 0);
	eclass_addmethod(c, (method) spectroscope_paint,           "paint",            A_NULL, 0);
	eclass_addmethod(c, (method) spectroscope_notify,          "notify",           A_NULL, 0);
    eclass_addmethod(c, (method) spectroscope_getdrawparams,   "getdrawparams",    A_NULL, 0);
    eclass_addmethod(c, (method) spectroscope_oksize,          "oksize",           A_NULL, 0);

    CLASS_ATTR_INVISIBLE            (c, "fontname", 1);
    CLASS_ATTR_INVISIBLE            (c, "fontweight", 1);
    CLASS_ATTR_INVISIBLE            (c, "fontslant", 1);
    CLASS_ATTR_INVISIBLE            (c, "fontsize", 1);
    CLASS_ATTR_INVISIBLE            (c, "send", 1);
	CLASS_ATTR_DEFAULT              (c, "size", 0, "200 100");
    
    CLASS_ATTR_LONG                 (c, "interval", 0, t_spectroscope, f_interval);
    CLASS_ATTR_ORDER                (c, "interval", 0, "2");
    CLASS_ATTR_LABEL                (c, "interval", 0, "Refresh Interval in Milliseconds");
    CLASS_ATTR_FILTER_MIN           (c, "interval", 20);
    CLASS_ATTR_DEFAULT              (c, "interval", 0, "50");
    CLASS_ATTR_SAVE                 (c, "interval", 1);
    CLASS_ATTR_STYLE                (c, "interval", 0, "number");

	CLASS_ATTR_RGBA                 (c, "bgcolor", 0, t_spectroscope, f_color_background);
	CLASS_ATTR_LABEL                (c, "bgcolor", 0, "Background Color");
	CLASS_ATTR_ORDER                (c, "bgcolor", 0, "1");
	CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bgcolor", 0, "0.75 0.75 0.75 1.");
	CLASS_ATTR_STYLE                (c, "bgcolor", 0, "color");

	CLASS_ATTR_RGBA                 (c, "bdcolor", 0, t_spectroscope, f_color_border);
	CLASS_ATTR_LABEL                (c, "bdcolor", 0, "Border Color");
	CLASS_ATTR_ORDER                (c, "bdcolor", 0, "2");
	CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bdcolor", 0, "0.5 0.5 0.5 1.");
	CLASS_ATTR_STYLE                (c, "bdcolor", 0, "color");

	CLASS_ATTR_RGBA                 (c, "spcolor", 0, t_spectroscope, f_color_spectrum);
	CLASS_ATTR_LABEL                (c, "spcolor", 0, "Spectrum Color");
	CLASS_ATTR_ORDER                (c, "spcolor", 0, "3");
	CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "spcolor", 0, "0. 0.6 0. 0.8");
	CLASS_ATTR_STYLE                (c, "spcolor", 0, "color");

    eclass_register(CLASS_BOX, c);
	cspectroscope_class = c;
}

void *spectroscope_new(t_symbol *s, int argc, t_atom *argv)
{
    long flags;
	t_spectroscope *x = (t_spectroscope *)eobj_new(cspectroscope_class);
    t_binbuf* d = binbuf_via_atoms(argc,argv);
    
    if(x && d)
    {
        flags = 0
        | EBOX_GROWINDI
        | EBOX_IGNORELOCKCLICK
        ;

        ebox_new((t_ebox *)x, flags);
        eobj_dspsetup((t_ebox *)x, 2, 0);

        x->f_buffer_real = (t_sample *)calloc(80192, sizeof(t_sample));
        x->f_buffer_imag = (t_sample *)calloc(80192, sizeof(t_sample));
        x->f_buffer_spectrum = (t_sample *)calloc(80192 * 2, sizeof(t_sample));
        
        x->f_clock          = clock_new(x,(t_method)spectroscope_tick);
        x->f_startclock     = 0;

        ebox_attrprocess_viabinbuf(x, d);
        ebox_ready((t_ebox *)x);
    }
	return (x);
}

void spectroscope_getdrawparams(t_spectroscope *x, t_object *patcherview, t_edrawparams *params)
{
	params->d_borderthickness   = 2.;
	params->d_cornersize        = 2.;
    params->d_bordercolor       = x->f_color_border;
    params->d_boxfillcolor      = x->f_color_background;
}

void spectroscope_oksize(t_spectroscope *x, t_rect *newrect)
{
    newrect->width = pd_clip_min(newrect->width, 8.);
    newrect->height = pd_clip_min(newrect->height, 8.);
}

void spectroscope_perform_real(t_spectroscope *x, t_object *dsp, t_sample **ins, long ni, t_sample **outs, long no,long nsamples,long f,void *up)
{
    memcpy(x->f_buffer_real, ins[0], nsamples * sizeof(t_sample));
    if(x->f_startclock)
    {
        x->f_startclock = 0;
        clock_delay(x->f_clock, 0);
    }
}

void spectroscope_perform(t_spectroscope *x, t_object *dsp, t_sample **ins, long ni, t_sample **outs, long no,long nsamples,long f,void *up)
{
    memcpy(x->f_buffer_real, ins[0], nsamples * sizeof(t_sample));
    memcpy(x->f_buffer_imag, ins[1], nsamples * sizeof(t_sample));
    if(x->f_startclock)
    {
        x->f_startclock = 0;
        clock_delay(x->f_clock, 0);
    }
}

void spectroscope_dsp(t_spectroscope *x, t_object *dsp, short *count, double samplerate, long maxvectorsize, long flags)
{
    x->f_buffer_size = maxvectorsize;
    if(count[1])
    {
        x->f_real_mode = 0;
        object_method(dsp, gensym("dsp_add"), x, (method)spectroscope_perform, 0, NULL);
    }
    else
    {
        x->f_real_mode = 1;
        object_method(dsp, gensym("dsp_add"), x, (method)spectroscope_perform_real, 0, NULL);
    }
    x->f_startclock = 1;
}

void spectroscope_tick(t_spectroscope *x)
{
    if(x->f_real_mode)
    {
        mayer_realfft(x->f_buffer_size, x->f_buffer_real);
        vDSP_polar(x->f_buffer_real, 1, x->f_buffer_spectrum, 1, x->f_buffer_size * 0.5);
    }
    else
    {
        
    }
    
    ebox_invalidate_layer((t_ebox *)x, csym_spectrum_layer);
    ebox_redraw((t_ebox *)x);
    
    if(sys_getdspstate())
    {
        clock_delay(x->f_clock, x->f_interval);
    }
}

void spectroscope_free(t_spectroscope *x)
{
	ebox_free((t_ebox *)x);
    clock_free(x->f_clock);
    free(x->f_buffer_real);
    free(x->f_buffer_imag);
    free(x->f_buffer_spectrum);
}

void spectroscope_assist(t_spectroscope *x, void *b, long m, long a, char *s)
{
	;
}

t_pd_err spectroscope_notify(t_spectroscope *x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
	if(msg == gensym("attr_modified"))
	{
		if(s == gensym("bgcolor") || s == gensym("bdcolor") || s == gensym("spcolor"))
		{
			ebox_invalidate_layer((t_ebox *)x, csym_background_layer);
			ebox_invalidate_layer((t_ebox *)x, csym_spectrum_layer);
		}
		ebox_redraw((t_ebox *)x);
	}
	return 0;
}

void draw_background(t_spectroscope *x,  t_object *view, t_rect *rect)
{
	int i;
	t_elayer *g = ebox_start_layer((t_ebox *)x, csym_background_layer, rect->width, rect->height);

	if(g)
	{
        egraphics_set_color_rgba(g, &x->f_color_border);
        for(i = 1; i < 4; i++)
        {
            egraphics_line_fast(g, -2, rect->height * 0.25 * (float)i, rect->width + 4, rect->height * 0.25 * (float)i);
        }
        for(i = 1; i < 6; i++)
        {
            egraphics_line_fast(g, rect->width * (1. / 6.) * (float)i, -2, rect->width * (1. / 6.) * (float)i, rect->height + 4);
        }
		ebox_end_layer((t_ebox*)x, csym_background_layer);
	}
	ebox_paint_layer((t_ebox *)x, csym_background_layer, 0.f, 0.f);
}


void draw_spectrum(t_spectroscope *x, t_object *view, t_rect *rect)
{
	t_elayer *g = ebox_start_layer((t_ebox *)x, csym_spectrum_layer, rect->width, rect->height);
	if(g)
	{
        double width = rect->width / (float)(x->f_buffer_size - 1);
        egraphics_set_color_rgba(g, &x->f_color_spectrum);
        egraphics_set_line_width(g, 2);
        egraphics_move_to(g, 0, x->f_buffer_spectrum[0] * rect->height);
        for(int i = 1; i < x->f_buffer_size; i++)
        {
            egraphics_line_to(g, width * (float)(i), x->f_buffer_spectrum[i] * rect->height);
        }
        egraphics_stroke(g);
		ebox_end_layer((t_ebox *)x, csym_spectrum_layer);
	}
	ebox_paint_layer((t_ebox *)x, csym_spectrum_layer, 0., 0.);
}

void spectroscope_paint(t_spectroscope *x, t_object *view)
{
    t_rect rect;
    ebox_get_rect_for_view((t_ebox *)x, &rect);
    draw_background(x, view, &rect);
    draw_spectrum(x, view, &rect);
}



