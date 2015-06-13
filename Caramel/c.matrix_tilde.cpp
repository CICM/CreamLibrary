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

typedef struct _matrix_tilde
{
	t_edspobj   j_box;
    t_sample*   f_values;
    long        f_inputs;
    long        f_outputs;
    
} t_matrix_tilde;

t_eclass *matrix_class;

void *matrix_new(t_symbol *s, int argc, t_atom *argv);
void matrix_free(t_matrix_tilde *x);
void matrix_assist(t_matrix_tilde *x, void *b, long m, long a, char *s);

void matrix_list(t_matrix_tilde *x, t_symbol *s, int ac, t_atom *av);

void matrix_dsp(t_matrix_tilde *x, t_object *dsp, short *count, double samplerate, long maxvectorsize, long flags);
void matrix_perform(t_matrix_tilde *x, t_object *d, t_sample **ins, long ni, t_sample **outs, long no, long sf, long f,void *up);

extern "C" void setup_c0x2ematrix_tilde(void)
{
	t_eclass *c;
    
	c = eclass_new("c.matrix~", (method)matrix_new, (method)matrix_free, (short)sizeof(t_matrix_tilde), 0L, A_GIMME, 0);
    
    eclass_dspinit(c);
    cream_initclass(c);
    
    eclass_addmethod(c, (method) matrix_dsp,             "dsp",              A_NULL, 0);
	eclass_addmethod(c, (method) matrix_assist,          "assist",           A_NULL, 0);
    eclass_addmethod(c, (method) matrix_list,            "list",             A_GIMME,0);
    
    eclass_register(CLASS_OBJ, c);
	matrix_class = c;
}

void *matrix_new(t_symbol *s, int argc, t_atom *argv)
{
	t_matrix_tilde *x = (t_matrix_tilde *)eobj_new(matrix_class);
	if(x)
    {
        x->f_inputs     = 1;
        x->f_outputs    = 1;
        if(argc && atom_gettype(argv) == A_LONG)
        {
            x->f_inputs = pd_clip_min(atom_getlong(argv), 1);
        }
        if(argc > 1 && atom_gettype(argv+1) == A_LONG)
        {
            x->f_outputs = pd_clip_min(atom_getlong(argv+1), 1);
        }
        x->f_values = (t_sample *)malloc((size_t)(x->f_inputs * x->f_outputs) * sizeof(t_sample));
        eobj_dspsetup((t_ebox *)x, x->f_inputs, x->f_outputs);
        x->j_box.d_misc = E_NO_INPLACE;
    }
    
	return (x);
}


void matrix_free(t_matrix_tilde *x)
{
	eobj_dspfree((t_ebox *)x);
    free(x->f_values);
}

void matrix_list(t_matrix_tilde *x, t_symbol *s, int ac, t_atom *av)
{
    if(ac && av)
    {
        for(long i = 2; i < ac; i += 3)
        {
            if(atom_gettype(av+i-2) == A_FLOAT && atom_gettype(av+i-1) == A_FLOAT && atom_gettype(av+i) == A_FLOAT)
            {
                long column  = atom_getfloat(av+i-2);
                long row     = atom_getfloat(av+i-1);
                float value  = atom_getfloat(av+i);
                if(column < x->f_inputs && row < x->f_outputs)
                {
                    x->f_values[row * x->f_inputs + column] = value;
                }
            }
        }
    }
}


void matrix_assist(t_matrix_tilde *x, void *b, long m, long a, char *s)
{
	;
}

void matrix_dsp(t_matrix_tilde *x, t_object *dsp, short *count, double samplerate, long maxvectorsize, long flags)
{
    object_method(dsp, gensym("dsp_add"), x, (method)matrix_perform, 0, NULL);
}

void matrix_perform(t_matrix_tilde *x, t_object *d, t_sample **ins, long ninputs, t_sample **outs, long noutputs, long sampleframes, long f,void *up)
{
    t_sample* gains = x->f_values+noutputs*ninputs-1;
    while(noutputs--)
    {
        long nins = ninputs;
        memset(outs[noutputs], 0, (size_t)sampleframes * sizeof(t_sample));
        while(nins--)
        {
            long frames = sampleframes;
            t_sample* output = outs[noutputs];
            t_sample* input  = ins[nins];
            while(frames--)
            {
                (*output++) += (*input++) * *gains;
            }
            --gains;
        }
    }
}



