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

t_eclass *atan_class;
t_eclass *atan2_class;
t_eclass *atanh_class;

void *atan_new(t_symbol *s, int argc, t_atom *argv)
{
	t_edspobj *x = (t_edspobj *)eobj_new(atan_class);
    eobj_dspsetup((t_ebox *)x, 1, 1);
    x->d_misc = E_NO_INPLACE;
	return (x);
}

void atan_perform(t_edspobj *x, t_object *d, float **ins, long ni, float **outs, long no, long sampleframes, long f,void *up)
{
    while(--sampleframes)
    {
        outs[0][sampleframes] = atanf(ins[0][sampleframes]);
    }
}

void atan_dsp(t_edspobj *x, t_object *dsp, short *count, double samplerate, long maxvectorsize, long flags)
{
    object_method(dsp, gensym("dsp_add"), x, (method)atan_perform, 0, NULL);
}

void *atan2_new(t_symbol *s, int argc, t_atom *argv)
{
	t_edspobj *x = (t_edspobj *)eobj_new(atan2_class);
    eobj_dspsetup((t_ebox *)x, 2, 1);
    x->d_misc = E_NO_INPLACE;
	return (x);
}

void atan2_perform(t_edspobj *x, t_object *d, float **ins, long ni, float **outs, long no, long sampleframes, long f,void *up)
{
    while(--sampleframes)
    {
        outs[0][sampleframes] = atan2f(ins[0][sampleframes], ins[1][sampleframes]);
    }
}

void atan2_dsp(t_edspobj *x, t_object *dsp, short *count, double samplerate, long maxvectorsize, long flags)
{
    object_method(dsp, gensym("dsp_add"), x, (method)atan2_perform, 0, NULL);
}

void *atanh_new(t_symbol *s, int argc, t_atom *argv)
{
	t_edspobj *x = (t_edspobj *)eobj_new(atan2_class);
    eobj_dspsetup((t_ebox *)x, 2, 1);
    x->d_misc = E_NO_INPLACE;
	return (x);
}

void atanh_perform(t_edspobj *x, t_object *d, float **ins, long ni, float **outs, long no, long sampleframes, long f,void *up)
{
    while(--sampleframes)
    {
        outs[0][sampleframes] = atanhf(ins[0][sampleframes]);
    }
}

void atanh_dsp(t_edspobj *x, t_object *dsp, short *count, double samplerate, long maxvectorsize, long flags)
{
    object_method(dsp, gensym("dsp_add"), x, (method)atan2_perform, 0, NULL);
}

extern "C"  void setup_c0x2eatan_tilde(void)
{
	t_eclass *c;
	c = eclass_new("c.atan~", (method)atan_new, (method)eobj_dspfree, (short)sizeof(t_edspobj), 0L, A_GIMME, 0);
    eclass_dspinit(c);
	cream_initclass(c);
    eclass_addmethod(c, (method) atan_dsp,       "dsp",              A_NULL, 0);
    eclass_register(CLASS_OBJ, c);
	atan_class = c;
    
    c = eclass_new("c.atan2~", (method)atan2_new, (method)eobj_dspfree, (short)sizeof(t_edspobj), 0L, A_GIMME, 0);
    eclass_dspinit(c);
	cream_initclass(c);
    eclass_addmethod(c, (method) atan2_dsp,       "dsp",              A_NULL, 0);
    eclass_register(CLASS_OBJ, c);
	atan2_class = c;
    
    c = eclass_new("c.atanh~", (method)atanh_new, (method)eobj_dspfree, (short)sizeof(t_edspobj), 0L, A_GIMME, 0);
    eclass_dspinit(c);
	cream_initclass(c);
    eclass_addmethod(c, (method) atanh_dsp,       "dsp",              A_NULL, 0);
    eclass_register(CLASS_OBJ, c);
	atanh_class = c;
}







