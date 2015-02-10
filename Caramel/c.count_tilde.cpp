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

typedef struct _count_tilde
{
	t_edspobj   j_box;
    t_sample    f_min;
    t_sample    f_max;
    t_sample    f_value;
} t_count_tilde;

static t_eclass *count_class;

extern void *count_new(t_symbol *s, int argc, t_atom *argv)
{
	t_count_tilde *x = (t_count_tilde *)eobj_new(count_class);
	if(x)
    {
        if(argc > 1 && atom_gettype(argv) == A_FLOAT && atom_gettype(argv+1) == A_FLOAT)
        {
            x->f_min = atom_getfloat(argv);
            x->f_max = atom_getfloat(argv+1);
            if(x->f_min >= x->f_max)
            {
                pd_error(x, "c.count~ : The minimum value should be inferior to the maximum value.");
                eobj_dspfree((t_ebox *)x);
                return NULL;
            }
            x->f_value = x->f_min;
            eobj_dspsetup((t_ebox *)x, 0, 1);
            return x;
        }
        else
        {
            pd_error(x, "c.count~ : The object needs a minimum and a maximum values.");
            return NULL;
        }
    }
    
	return NULL;
}


extern void count_free(t_count_tilde *x)
{
	eobj_dspfree((t_ebox *)x);
}

extern void count_perform(t_count_tilde *x, t_object *d, t_sample **ins, long ninputs, t_sample **outs, long noutputs, long sampleframes, long f,void *up)
{
    t_sample* output = outs[0];
    while(sampleframes--)
    {
        *output++ = x->f_value++;
        if(x->f_value >= x->f_max)
        {
            x->f_value = x->f_min;
        }
    }
}

extern void count_dsp(t_count_tilde *x, t_object *dsp, short *count, double samplerate, long maxvectorsize, long flags)
{
    x->f_value = x->f_min;
    object_method(dsp, gensym("dsp_add"), x, (method)count_perform, 0, NULL);
}

extern "C" void setup_c0x2ecount_tilde(void)
{
    t_eclass *c;
    
    c = eclass_new("c.count~", (method)count_new, (method)count_free, (short)sizeof(t_count_tilde), CLASS_NOINLET, A_GIMME, 0);
    
    eclass_dspinit(c);
    cream_initclass(c);
    
    eclass_addmethod(c, (method) count_dsp,             "dsp",              A_NULL, 0);
    
    eclass_register(CLASS_OBJ, c);
    count_class = c;
}




