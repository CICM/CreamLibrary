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

t_eclass *sin_class;

void *sin_new(t_symbol *s, int argc, t_atom *argv)
{
	t_edspobj *x = (t_edspobj *)eobj_new(sin_class);
    eobj_dspsetup((t_ebox *)x, 1, 1);
	return (x);
}

void sin_perform(t_edspobj *x, t_object *d, float **ins, long ni, float **outs, long no, long sampleframes, long f,void *up)
{
    while(--sampleframes)
    {
        outs[0][sampleframes] = sinf(ins[0][sampleframes]);
    }
}

void sin_dsp(t_edspobj *x, t_object *dsp, short *count, double samplerate, long maxvectorsize, long flags)
{
    object_method(dsp, gensym("dsp_add"), x, (method)sin_perform, 0, NULL);
}

extern "C"  void setup_c0x2esin_tilde(void)
{
	t_eclass *c;
	c = eclass_new("c.sin~", (method)sin_new, (method)eobj_dspfree, (short)sizeof(t_edspobj), 0L, A_GIMME, 0);
    eclass_dspinit(c);
	cream_initclass(c);
    eclass_addmethod(c, (method) sin_dsp,       "dsp",              A_NULL, 0);
    eclass_register(CLASS_OBJ, c);
	sin_class = c;
}







