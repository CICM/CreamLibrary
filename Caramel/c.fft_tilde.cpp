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

typedef struct _fft_tilde
{
	t_edspobj   j_box;
    t_sample*   f_real;
    char        f_zero;
} t_fft_tilde;

typedef struct _ifft_tilde
{
    t_edspobj   j_box;
    t_sample*   f_real;
    t_sample*   f_saved;
    char        f_zero;
} t_ifft_tilde;

static t_eclass *fft_class;
static t_eclass *ifft_class;

extern void fft_perform_zeropad(t_fft_tilde *x, t_object *d, t_sample **ins, long ninputs, t_sample **outs, long noutputs, long sampleframes, long f,void *up)
{
    memcpy(x->f_real, ins[0], sampleframes * sizeof(t_sample));
    memset(x->f_real+sampleframes, 0, sampleframes * sizeof(t_sample));
    mayer_realfft(sampleframes * 2, x->f_real);
    
    memcpy(outs[0], x->f_real, sampleframes * sizeof(t_sample));
    t_sample* out = outs[1];
    t_sample* real = x->f_real + sampleframes * 2 - 1;
    *(out++) = 0;
    while(--sampleframes)
    {
        *(out++) = -(*(real--));
    }
}

extern void ifft_perform_zeropad(t_ifft_tilde *x, t_object *d, t_sample **ins, long ninputs, t_sample **outs, long noutputs, long sampleframes, long f,void *up)
{
    memcpy(ins[0], x->f_real, sampleframes * sizeof(t_sample));
    t_sample* in = ins[1];
    t_sample* real = x->f_real + sampleframes * 2 - 1;
    for(long i = 0; i < sampleframes; i++)
    {
        *(real--) = -(*(in--));
    }
    mayer_realifft(sampleframes * 2, x->f_real);
    memcpy(outs[0], x->f_real, sampleframes * sizeof(t_sample));
    t_sample *out = outs[0], *saved = x->f_saved;
    for(long i = 0; i < sampleframes; i++)
    {
        *(out++) += *(saved++);
    }
    memcpy(x->f_saved, x->f_real+sampleframes, sampleframes * sizeof(t_sample));
}


extern void fft_perform(t_fft_tilde *x, t_object *d, t_sample **ins, long ninputs, t_sample **outs, long noutputs, long sampleframes, long f,void *up)
{
    long limit = sampleframes * 0.5;
    t_sample* in = x->f_real;
    memcpy(in, ins[0], sampleframes * sizeof(t_sample));
    memset(in+sampleframes, 0, sampleframes * sizeof(t_sample));
    mayer_realfft(sampleframes, in);
    
    memcpy(outs[0], in, (limit + 1) * sizeof(t_sample));
    memset(outs[0]+(limit + 1), 0, (limit - 1) * sizeof(t_sample));
    
    t_sample* out = outs[1];
    in = x->f_real + sampleframes - 1;
    *(out++) = 0;
    while(--limit)
    {
        *(out++) = -(*(in--));
    }
    memset(out, 0, (sampleframes * 0.5 - 1) * sizeof(t_sample));
}

extern void fft_dsp(t_fft_tilde *x, t_object *dsp, short *count, double samplerate, long maxvectorsize, long flags)
{
    if(x->f_real)
    {
        free(x->f_real);
    }
    x->f_real = (t_sample *)malloc(maxvectorsize * 2 * sizeof(t_sample));
    if(x->f_zero)
    {
        object_method(dsp, gensym("dsp_add"), x, (method)fft_perform_zeropad, 0, NULL);
    }
    else
    {
        object_method(dsp, gensym("dsp_add"), x, (method)fft_perform, 0, NULL);
    }
    
}

extern void ifft_dsp(t_ifft_tilde *x, t_object *dsp, short *count, double samplerate, long maxvectorsize, long flags)
{
    if(x->f_real)
    {
        free(x->f_real);
    }
    if(x->f_saved)
    {
        free(x->f_saved);
    }
    x->f_real = (t_sample *)malloc(maxvectorsize * 2 * sizeof(t_sample));
    x->f_saved= (t_sample *)malloc(maxvectorsize * sizeof(t_sample));
    memset( x->f_saved, 0, maxvectorsize * sizeof(t_sample));
    if(x->f_zero)
    {
        object_method(dsp, gensym("dsp_add"), x, (method)ifft_perform_zeropad, 0, NULL);
    }
    else
    {
        int todo;
        //object_method(dsp, gensym("dsp_add"), x, (method)fft_perform, 0, NULL);
    }
    
}

extern void *fft_new(t_symbol *s, int argc, t_atom *argv)
{
    t_fft_tilde *x = (t_fft_tilde *)eobj_new(fft_class);
    if(x)
    {
        x->f_zero = argc;
        eobj_dspsetup((t_ebox *)x, 1, 2);
        return x;
    }
    
    return NULL;
}


extern void *ifft_new(t_symbol *s, int argc, t_atom *argv)
{
    t_fft_tilde *x = (t_fft_tilde *)eobj_new(fft_class);
    if(x)
    {
        x->f_zero = argc;
        eobj_dspsetup((t_ebox *)x, 2, 1);
        return x;
    }
    
    return NULL;
}


extern void fft_free(t_fft_tilde *x)
{
    eobj_dspfree((t_ebox *)x);
    if(x->f_real)
    {
        free(x->f_real);
    }
}

extern void ifft_free(t_ifft_tilde *x)
{
    eobj_dspfree((t_ebox *)x);
    if(x->f_real)
    {
        free(x->f_real);
    }
    if(x->f_saved)
    {
        free(x->f_saved);
    }
}

extern "C" void setup_c0x2efft_tilde(void)
{
    t_eclass *c;
    
    c = eclass_new("c.fft~", (method)fft_new, (method)fft_free, (short)sizeof(t_fft_tilde), CLASS_NOINLET, A_GIMME, 0);
    
    eclass_dspinit(c);
    cream_initclass(c);
    eclass_addmethod(c, (method) fft_dsp,             "dsp",              A_NULL, 0);
    eclass_register(CLASS_OBJ, c);
    fft_class = c;
    
    c = eclass_new("c.ifft~", (method)ifft_new, (method)ifft_free, (short)sizeof(t_ifft_tilde), CLASS_NOINLET, A_GIMME, 0);
    
    eclass_dspinit(c);
    cream_initclass(c);
    eclass_addmethod(c, (method) ifft_dsp,             "dsp",              A_NULL, 0);
    eclass_register(CLASS_OBJ, c);
    ifft_class = c;
}




