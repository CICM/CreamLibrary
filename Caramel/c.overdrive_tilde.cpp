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

typedef struct _overdrive
{
	t_edspobj   j_box;
    float       f_gain;
} t_overdrive;

t_eclass *overdrive_class;

void *overdrive_new(t_symbol *s, int argc, t_atom *argv);
void overdrive_free(t_overdrive *x);
void overdrive_assist(t_overdrive *x, void *b, long m, long a, char *s);

void overdrive_dsp(t_overdrive *x, t_object *dsp, short *count, double samplerate, long maxvectorsize, long flags);
void overdrive_perform(t_overdrive *x, t_object *d, float **ins, long ni, float **outs, long no, long sf, long f,void *up);
void overdrive_float(t_overdrive *x, float f);

extern "C"  void setup_c0x2eoverdrive_tilde(void)
{
	t_eclass *c;
    
	c = eclass_new("c.overdrive~", (method)overdrive_new, (method)overdrive_free, (short)sizeof(t_overdrive), 0L, A_GIMME, 0);
    
    eclass_dspinit(c);
    cream_initclass(c);
    
    eclass_addmethod(c, (method) overdrive_dsp,             "dsp",              A_CANT, 0);
	eclass_addmethod(c, (method) overdrive_assist,          "assist",           A_CANT, 0);
    eclass_addmethod(c, (method) overdrive_float,           "float",            A_FLOAT,0);
    
    eclass_register(CLASS_OBJ, c);
	overdrive_class = c;
}

void *overdrive_new(t_symbol *s, int argc, t_atom *argv)
{
	t_overdrive *x =  NULL;

	x = (t_overdrive *)eobj_new(overdrive_class);
	x->f_gain = 1;
    eobj_dspsetup((t_ebox *)x, 1, 1);
    eproxy_new(x);
    if(argc && argv && atom_gettype(argv) == A_FLOAT)
        x->f_gain = pd_clip_min(atom_getfloat(argv), 1);
    
	return (x);
}

void overdrive_float(t_overdrive *x, float f)
{
    x->f_gain = pd_clip_min(f, 1);
}

void overdrive_free(t_overdrive *x)
{
	eobj_dspfree((t_ebox *)x);
}

void overdrive_assist(t_overdrive *x, void *b, long m, long a, char *s)
{
	;
}

void overdrive_dsp(t_overdrive *x, t_object *dsp, short *count, double samplerate, long maxvectorsize, long flags)
{
    object_method(dsp, gensym("dsp_add"), x, (method)overdrive_perform, 0, NULL);
}

void overdrive_perform(t_overdrive *x, t_object *d, float **ins, long ni, float **outs, long no, long sampleframes, long f,void *up)
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




