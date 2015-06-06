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

typedef struct  _prepend
{
	t_eobj      j_box;
	t_outlet*   f_out;
    t_atom*     f_argv;
	long        f_argc;
    t_symbol*   f_selector;

} t_prepend;

t_eclass *prepend_class;

static void prepend_bang(t_prepend *x)
{
    t_atom* av = (t_atom *)malloc(1 + x->f_argc * sizeof(t_atom));
    if(av)
    {
        memcpy(av, x->f_argv, x->f_argc * sizeof(t_atom));
        atom_setsym(av+x->f_argc, &s_bang);
        if(x->f_selector != &s_list)
        {
            outlet_anything(x->f_out, x->f_selector, 1 + x->f_argc, av);
        }
        else
        {
            outlet_list(x->f_out, &s_list, 1 + x->f_argc, av);
        }
        free(av);
    }
    else
    {
        pd_error(x, "can't allocate memory.");
    }
}

static void prepend_float(t_prepend *x, float f)
{
    t_atom* av = (t_atom *)malloc(1 + x->f_argc * sizeof(t_atom));
    if(av)
    {
        memcpy(av, x->f_argv, x->f_argc * sizeof(t_atom));
        atom_setfloat(av+x->f_argc, f);
        if(x->f_selector != &s_list)
        {
            outlet_anything(x->f_out, x->f_selector, 1 + x->f_argc, av);
        }
        else
        {
            outlet_list(x->f_out, &s_list, 1 + x->f_argc, av);
        }
        free(av);
    }
    else
    {
        pd_error(x, "can't allocate memory.");
    }
}

static void prepend_symbol(t_prepend *x, t_symbol *s)
{
    t_atom* av = (t_atom *)malloc(1 + x->f_argc * sizeof(t_atom));
    if(av)
    {
        memcpy(av, x->f_argv, x->f_argc * sizeof(t_atom));
        atom_setsym(av+x->f_argc, s);
        if(x->f_selector != &s_list)
        {
            outlet_anything(x->f_out, x->f_selector, 1 + x->f_argc, av);
        }
        else
        {
            outlet_list(x->f_out, &s_list, 1 + x->f_argc, av);
        }
        free(av);
    }
    else
    {
        pd_error(x, "can't allocate memory.");
    }
}

static void prepend_list(t_prepend *x, t_symbol *s, int argc, t_atom *argv)
{
    if(argc && argv)
    {
        t_atom* av = (t_atom *)malloc(argc + x->f_argc * sizeof(t_atom));
        if(av)
        {
            memcpy(av, x->f_argv, x->f_argc * sizeof(t_atom));
            memcpy(av+x->f_argc, argv, argc * sizeof(t_atom));
            if(x->f_selector != &s_list)
            {
                outlet_anything(x->f_out, x->f_selector, argc + x->f_argc, av);
            }
            else
            {
                outlet_list(x->f_out, &s_list, argc + x->f_argc, av);
            }
            free(av);
        }
        else
        {
            pd_error(x, "can't allocate memory.");
        }
    }
    else
    {
        if(x->f_selector != &s_list)
        {
            outlet_anything(x->f_out, x->f_selector, x->f_argc, x->f_argv);
        }
        else
        {
            outlet_list(x->f_out, &s_list, x->f_argc, x->f_argv);
        }
    }
}

static void prepend_set(t_prepend *x, t_symbol *s, int argc, t_atom *argv)
{
    if(argc && argv)
    {
        if(atom_gettype(argv) == A_SYM)
        {
            x->f_argc = --argc;
            x->f_selector = atom_getsym(argv++);
        }
        else
        {
            x->f_argc = argc;
            x->f_selector = &s_list;
        }
        if(x->f_argv)
        {
            x->f_argv = (t_atom *)realloc(x->f_argv, x->f_argc * sizeof(t_atom));
        }
        else
        {
            x->f_argv = (t_atom *)malloc(x->f_argc * sizeof(t_atom));
        }
        if(x->f_argv)
        {
            for(int i = 0; i < x->f_argc; i++)
            {
                x->f_argv[i] = argv[i];
            }
        }
        else
        {
            pd_error(x, "can't allocate memory.");
        }
    }
    else
    {
        x->f_argc = 0;
        if(x->f_argv)
        {
            free(x->f_argv);
            x->f_argv = NULL;
        }
    }
}

static void prepend_free(t_prepend *x)
{
    if(x->f_argv)
    {
        free(x->f_argv);
    }
    eobj_free(x);
}

static void *prepend_new(t_symbol *s, int argc, t_atom *argv)
{
    t_prepend *x = (t_prepend *)eobj_new(prepend_class);
    if(x)
    {
        x->f_argv = NULL;
        x->f_argc = 0;
        prepend_set(x, gensym("set"), argc, argv);
        x->f_out = (t_outlet *)listout(x);
    }
    return (x);
}

extern "C" void setup_c0x2eprepend(void)
{
	t_eclass *c;
    
	c = eclass_new("c.prepend", (method)prepend_new, (method)prepend_free, (short)sizeof(t_prepend), 0L, A_GIMME, 0);
    cream_initclass(c);
    
    eclass_addmethod(c, (method)prepend_list,        "anything",       A_GIMME, 0);
    eclass_addmethod(c, (method)prepend_list,        "list",           A_GIMME, 0);
    eclass_addmethod(c, (method)prepend_set,         "set",            A_GIMME, 0);
    eclass_addmethod(c, (method)prepend_float,       "float",          A_FLOAT, 0);
    eclass_addmethod(c, (method)prepend_symbol,      "symbol",         A_SYMBOL,0);
    eclass_addmethod(c, (method)prepend_bang,        "bang",           A_NULL,  0);
    
    eclass_register(CLASS_OBJ, c);
	prepend_class = c;
}



