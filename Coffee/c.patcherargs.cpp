/*
 * PdEnhanced - Pure Data Enhanced
 *
 * An add-on for Pure Data
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

typedef struct  _patcherargs
{
	t_eobj      j_box;
	t_outlet*   f_out_args;
    t_outlet*   f_out_attrs;
    t_outlet*   f_out_done;
    t_canvas*   f_cnv;
    t_atom*     f_args;
    long        f_argc;

    long        f_n_attrs;
    t_symbol**  f_attr_name;
    t_atom**    f_attr_vals;
    long*       f_attr_size;
    double      f_time;
    char        f_init;

} t_patcherargs;

t_eclass *patcherargs_class;

static char patcherargs_initialize(t_patcherargs *x)
{
    t_canvas* cnv = x->f_cnv;
    if(cnv)
    {
        t_binbuf *b = cnv->gl_obj.te_binbuf;
        if(b)
        {
            int ac      = binbuf_getnatom(b);
            t_atom* av  = binbuf_getvec(b);
            if(atom_gettype(av) == A_SYM && atom_getsym(av) == gensym("pd"))
            {
                ac--;
                av++;
            }
            int argc = atoms_get_attributes_offset(ac, av);
            
            if(argc > x->f_argc)
            {
                x->f_argc = argc;
                x->f_args = (t_atom *)realloc(x->f_args, x->f_argc * sizeof(t_atom));
            }
            memcpy(x->f_args, av, argc * sizeof(t_atom));
            
            for(int i = 0; i < x->f_n_attrs; i++)
            {
                long nattr;
                t_atom* attrs;
                if(!atoms_get_attribute(ac-argc, av+argc, x->f_attr_name[i], &nattr, &attrs))
                {
                    x->f_attr_size[i] = nattr;
                    x->f_attr_vals[i] = (t_atom *)realloc(x->f_attr_vals[i], nattr * sizeof(t_atom));
                    memcpy(x->f_attr_vals[i], attrs, nattr * sizeof(t_atom));
                    free(attrs);
                }
            }
            return 1;
        }
    }
    return 0;
}

static void patcherargs_output(t_patcherargs *x)
{
    if(!x->f_init)
    {
        x->f_init = patcherargs_initialize(x);
    }
    outlet_list(x->f_out_args, &s_list, x->f_argc, x->f_args);
    for(int i = 0; i < x->f_n_attrs; i++)
    {
        outlet_anything(x->f_out_attrs, gensym(x->f_attr_name[i]->s_name+1), x->f_attr_size[i], x->f_attr_vals[i]);
    }
    outlet_bang(x->f_out_done);
}

static void patcherargs_click(t_patcherargs *x)
{
    if(clock_gettimesince(x->f_time) < 250.)
        patcherargs_output(x);
    x->f_time = clock_getsystime();
}

static void patcherargs_free(t_patcherargs *x)
{
    int i;
    if(x->f_argc && x->f_args)
    {
        free(x->f_args);
    }
    if(x->f_n_attrs)
    {
        for(i = 0; i < x->f_n_attrs; i++)
        {
            if(x->f_attr_size[i] && x->f_attr_vals[i])
            {
                free(x->f_attr_vals[i]);
            }
        }
        free(x->f_attr_name);
        free(x->f_attr_size);
        free(x->f_attr_vals);
    }
    eobj_free(x);
}

static void *patcherargs_new(t_symbol *s, int argc, t_atom *argv)
{
    int i;
    t_patcherargs *x = (t_patcherargs *)eobj_new(patcherargs_class);
    if(x)
    {
        x->f_argc = atoms_get_attributes_offset(argc, argv);
        x->f_args = (t_atom *)malloc(x->f_argc * sizeof(t_atom));
        memcpy(x->f_args, argv, x->f_argc * sizeof(t_atom));
        
        x->f_n_attrs = atoms_get_keys(argc-x->f_argc, argv+x->f_argc, &x->f_attr_name);
        if(x->f_n_attrs)
        {
            x->f_attr_vals = (t_atom **)malloc(x->f_n_attrs * sizeof(t_atom *));
            x->f_attr_size = (long *)malloc(x->f_n_attrs * sizeof(long));
            for(i = 0; i < x->f_n_attrs; i++)
            {
                atoms_get_attribute(argc-x->f_argc, argv+x->f_argc, x->f_attr_name[i], &x->f_attr_size[i], &x->f_attr_vals[i]);
            }
        }

        x->f_out_args = (t_outlet *)listout(x);
        x->f_out_attrs = (t_outlet *)listout(x);
        x->f_out_done = (t_outlet *)bangout(x);
        x->f_time = clock_getsystime();
        if(canvas_getcurrent())
        {
            x->f_cnv = glist_getcanvas(canvas_getcurrent());
        }
        else
            x->f_cnv = NULL;
        
        x->f_init = patcherargs_initialize(x);
    }
    
    return (x);
}


extern "C" void setup_c0x2epatcherargs(void)
{
	t_eclass *c;

	c = eclass_new("c.patcherargs", (method)patcherargs_new, (method)patcherargs_free, (short)sizeof(t_patcherargs), 0L, A_GIMME, 0);
    class_addcreator((t_newmethod)patcherargs_new, gensym("c.canvasargs"), A_GIMME, 0);
    cream_initclass(c);

    eclass_addmethod(c, (method)patcherargs_output,      "bang",       A_NULL, 0);
    eclass_addmethod(c, (method)patcherargs_click,       "click",      A_NULL, 0);

    eclass_register(CLASS_OBJ, c);
	patcherargs_class = c;
}


