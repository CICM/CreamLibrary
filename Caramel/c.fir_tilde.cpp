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

#include <algorithm>
#include <cstring>
#include <numeric>
#include <vector>

#include <cmath>
#include <cstdio>
#include <cstdlib>

typedef struct _fir
{
	t_edspobj     j_box;
    long          f_size;
    t_sample*     f_buffer;
    t_sample*     f_temp;
    bool          f_normalize;
    t_symbol*     f_name;
} t_fir;

static t_eclass *fir_class;

static void *fir_new(t_symbol *s, int argc, t_atom *argv)
{
	t_fir *x = (t_fir *)eobj_new(fir_class);
    if(x)
    {
        eobj_dspsetup((t_ebox *)x, 1, 1);
        
        x->f_name       = NULL;
        x->f_normalize  = false;
        x->f_buffer     = NULL;
        x->f_temp       = NULL;
        x->f_size       = 0;
        if(argc && argv)
        {
            if(atom_gettype(argv) == A_SYM)
            {
                x->f_name = atom_getsym(argv);
            }
            if(argc > 1)
            {
                if((atom_gettype(argv+1) == A_SYM && atom_getsym(argv+1) == gensym("normalize")) || (atom_gettype(argv+1) == A_LONG && atom_getlong(argv+1) != 0))
                {
                     x->f_normalize = 1;
                }
            }
        }
    }
    
	return (x);
}

static void fir_set_do(t_fir *x, t_symbol *s, char dsp)
{
    t_garray *a = NULL;
    if(s)
    {
        int     state;
        t_word* buffer;
        int     size;
        x->f_name = s;
        if(!dsp)
        {
            state = canvas_suspend_dsp();
        }
        if(!(a = (t_garray *)pd_findbyclass(x->f_name, garray_class)))
        {
            object_error(x, "c.fir~: %s no such array.", x->f_name->s_name);
            return;
        }
        else if(!garray_getfloatwords(a, &size, &buffer))
        {
            object_error(x, "c.fir~: %s array is empty.", x->f_name->s_name);
            return;
        }
        else
        {
            if(x->f_size != size && x->f_buffer)
            {
                x->f_buffer = (t_sample *)realloc(x->f_buffer, (size_t)size * sizeof(t_sample));
            }
            else
            {
                x->f_buffer = (t_sample *)malloc((size_t)size * sizeof(t_sample));
            }
            if(x->f_buffer)
            {
                x->f_size = size;
                for(int i = 0; i < x->f_size; i++)
                {
                    x->f_buffer[i] = buffer[i].w_float;
                }
                if(x->f_normalize)
                {
                    float max = 0;
                    for(int i = 0; i < x->f_size; i++)
                    {
                        if(fabs(x->f_buffer[i]) > max)
                            max = fabs(x->f_buffer[i]);
                    }
                    if(max != 0)
                    {
                        max = 1.f / max;
                        for(int i = 0; i < x->f_size; i++)
                        {
                            x->f_buffer[i] *= max;
                        }
                    }
                }
            }
            else
            {
                x->f_size = 0;
                object_error(x, "c.fir~: can't allocate buffer memory.");
                return;
            }
        }
        if(!dsp)
        {
            canvas_resume_dsp(state);
        }
    }
    else
    {
        if(x->f_buffer)
        {
            free(x->f_buffer);
        }
        x->f_buffer = NULL;
        x->f_size   = 0;
    }
    x->f_name = s;
}

static void fir_set(t_fir *x, t_symbol *s)
{
    fir_set_do(x, s, 0);
}

static void fir_free(t_fir *x)
{
    if(x->f_buffer)
    {
        free(x->f_buffer);
    }
    if(x->f_temp)
    {
        free(x->f_temp);
    }
	eobj_dspfree((t_ebox *)x);
}

static void fir_normalize(t_fir *x, float f)
{
    if(bool(f) != x->f_normalize)
    {
        x->f_normalize = bool(f);
        fir_set_do(x, x->f_name, 0);
    }
}

static void fir_perform(t_fir *x, t_object *d, t_sample **ins, long ni, t_sample **outs, long no, long sampleframes, long f,void *up)
{
    const long buffersize  = x->f_size;
    const t_sample* buffer = x->f_buffer;
    for(long i = 0; i < buffersize; i++)
    {
        t_sample* out      = x->f_temp+i;
        t_sample  g        = buffer[i];
        const t_sample* in = ins[0];
        if(g)
        {
            for(long j = sampleframes>>3; j; --j, in += 8, out += 8)
            {
                const t_sample f0 = in[0] * g, f1 = in[1] * g, f2 = in[2] * g, f3 = in[3] * g;
                const t_sample f4 = in[4] * g, f5 = in[5] * g, f6 = in[6] * g, f7 = in[7] * g;
                out[0] += f0; out[1] += f1; out[2] += f2; out[3] += f3;
                out[4] += f4; out[5] += f5; out[6] += f6; out[7] += f7;
            }
            for(long j = sampleframes&7; j; --j, in++, out++)
            {
                out[0] += g * in[0];
            }
        }
    }
    t_sample*  temp  = x->f_temp;
    memcpy(outs[0], temp, (size_t)sampleframes * sizeof(t_sample));
    memcpy(temp, temp+sampleframes, (size_t)(buffersize - 1) * sizeof(t_sample));
    memset(temp+(buffersize - 1), 0, (size_t)sampleframes * sizeof(t_sample));
}

static void fir_dsp(t_fir *x, t_object *dsp, short *count, double samplerate, long maxvectorsize, long flags)
{
    fir_set_do(x, x->f_name, 1);
    if(x->f_name && x->f_buffer)
    {
        if(x->f_temp)
        {
            x->f_temp = (t_sample *)realloc(x->f_temp , (size_t)(maxvectorsize + x->f_size - 1) * sizeof(t_sample));
        }
        else
        {
            x->f_temp = (t_sample *)malloc((size_t)(maxvectorsize + x->f_size - 1) * sizeof(t_sample));
        }
        if(x->f_temp)
        {
            memset(x->f_temp, 0, (size_t)(maxvectorsize + x->f_size - 1) * sizeof(t_sample));
            object_method(dsp, gensym("dsp_add"), x, (method)fir_perform, 0, NULL);
        }
        else
        {
            object_error(x, "c.fir~ : can't allocate memory.");
        }
    }
}

extern "C"  void setup_c0x2efir_tilde(void)
{
    t_eclass *c;
    
    c = eclass_new("c.fir~", (method)fir_new, (method)fir_free, (short)sizeof(t_fir), 0L, A_GIMME, 0);
    
    eclass_dspinit(c);
    cream_initclass(c);
    eclass_addmethod(c, (method) fir_dsp,       "dsp",              A_NULL, 0);
    eclass_addmethod(c, (method) fir_set,       "set",              A_SYM,  0);
    eclass_addmethod(c, (method) fir_normalize, "normalize",        A_LONG, 0);
    eclass_register(CLASS_OBJ, c);
    fir_class = c;
}







