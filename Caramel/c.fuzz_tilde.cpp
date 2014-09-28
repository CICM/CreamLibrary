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

typedef struct _fuzz
{
	t_edspobj   j_box;
    float       f_gain;
} t_fuzz;

t_eclass *fuzz_class;

void *fuzz_new(t_symbol *s, int argc, t_atom *argv);
void fuzz_free(t_fuzz *x);
void fuzz_assist(t_fuzz *x, void *b, long m, long a, char *s);

void fuzz_dsp(t_fuzz *x, t_object *dsp, short *count, double samplerate, long maxvectorsize, long flags);
void fuzz_perform(t_fuzz *x, t_object *d, float **ins, long ni, float **outs, long no, long sf, long f,void *up);
void fuzz_float(t_fuzz *x, float f);

extern "C"  void setup_c0x2efuzz_tilde(void)
{
	t_eclass *c;
    
	c = eclass_new("c.fuzz~", (method)fuzz_new, (method)fuzz_free, (short)sizeof(t_fuzz), 0L, A_GIMME, 0);
    
    eclass_dspinit(c);
    cream_initclass(c);
    
    eclass_addmethod(c, (method) fuzz_dsp,             "dsp",              A_CANT, 0);
	eclass_addmethod(c, (method) fuzz_assist,          "assist",           A_CANT, 0);
    eclass_addmethod(c, (method) fuzz_float,           "float",            A_FLOAT,0);
    
    eclass_register(CLASS_OBJ, c);
	fuzz_class = c;
}

void *fuzz_new(t_symbol *s, int argc, t_atom *argv)
{
	t_fuzz *x =  NULL;

	x = (t_fuzz *)eobj_new(fuzz_class);
	x->f_gain = 1;
    eobj_dspsetup((t_ebox *)x, 1, 1);
    eproxy_new(x);
    if(argc && argv && atom_gettype(argv) == A_FLOAT)
        x->f_gain = pd_clip_min(atom_getfloat(argv), 1);
    
	return (x);
}

void fuzz_float(t_fuzz *x, float f)
{
    x->f_gain = pd_clip_min(f, 1);
}

void fuzz_free(t_fuzz *x)
{
	eobj_dspfree((t_ebox *)x);
}

void fuzz_assist(t_fuzz *x, void *b, long m, long a, char *s)
{
	;
}

void fuzz_dsp(t_fuzz *x, t_object *dsp, short *count, double samplerate, long maxvectorsize, long flags)
{
    object_method(dsp, gensym("dsp_add"), x, (method)fuzz_perform, 0, NULL);
}

void fuzz_perform(t_fuzz *x, t_object *d, float **ins, long ni, float **outs, long no, long sampleframes, long f,void *up)
{
    float in, fin, temp;
    while(--sampleframes)
    {
        in = *(ins[0]+sampleframes);
        fin = fabsf(in);
        temp = in / fin;
        if(fin == 0.f)
            *(outs[0]+sampleframes) = 0.f;
        else
            *(outs[0]+sampleframes) = (float)pd_clip_minmax(temp * (1.f - exp(x->f_gain * in * temp)), -1., 1.);
    }
    in = *(ins[0]);
    fin = fabsf(in);
    temp = in / fin;
    if(fin == 0.f)
        *(outs[0]+sampleframes) = 0.f;
    else
        *(outs[0]) = (float)pd_clip_minmax(temp * (1.f - exp(x->f_gain * in * temp)), -1., 1.);
}




