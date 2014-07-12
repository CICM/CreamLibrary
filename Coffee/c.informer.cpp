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

typedef struct  _informer
{
	t_eobj      l_box;
	t_outlet*   l_out;
    int         l_ninputs;
    int         l_noutputs;
    t_clock*    l_clock;
    int         baba;
    
} t_informer;

typedef struct _my_hoa_dac
{
    t_object    x_obj;
    t_int       x_n;
    t_int*      x_vec;
    t_float     x_f;
} t_my_hoa_dac;

t_eclass *informer_class;
t_symbol *sym_camomile1572 = gensym("camomile1572");
void *informer_new(t_symbol *s, int argc, t_atom *argv);
void informer_free(t_informer *x);
void informer_assist(t_informer *x, void *b, long m, long a, char *s);

void informer_click(t_informer *x);
void informer_output(t_informer *x);
void informer_loadbang(t_informer *x);

extern "C" void setup_c0x2einformer(void)
{
	t_eclass *c;
    
	c = eclass_new("c.informer", (method)informer_new, (method)informer_free, (short)sizeof(t_informer), 0L, A_GIMME, 0);
    cream_initclass(c);
    
    eclass_addmethod(c, (method) informer_loadbang,    "loadbang",         A_CANT, 0);
    eclass_addmethod(c, (method) informer_output,      "bang",             A_CANT, 0);
	eclass_addmethod(c, (method) informer_click,       "click",            A_CANT, 0);
    eclass_addmethod(c, (method) informer_assist,      "assist",           A_CANT, 0);
	
    eclass_register(CLASS_OBJ, c);
	informer_class = c;
}

void *informer_new(t_symbol *s, int argc, t_atom *argv)
{
	t_informer *x =  NULL;
	t_binbuf* d;
    
    if (!(d = binbuf_via_atoms(argc,argv)))
        return NULL;
    
    x = (t_informer *)eobj_new(informer_class);
    x->l_out = (t_outlet *)anythingout(x);
    
    pd_bind((t_pd *)x, gensym("informer1572"));
    x->baba = 0;
    x->l_clock = clock_new(x, (t_method)informer_output);
    return (x);
}

void informer_loadbang(t_informer *x)
{
    t_gobj *y = NULL;
    t_object *z = NULL;
    t_my_hoa_dac *h = NULL;
    t_atom *av = NULL;
    int ac = 0;
    x->l_ninputs = 0;
    x->l_noutputs = 0;
    t_atom argv[2];
    
    for(y = eobj_getcanvas(x)->gl_list; y; y = y->g_next)
    {
        if(eobj_getclassname(y) == gensym("dac~"))
        {
            z = (t_object *)y;
            av = binbuf_getvec(z->te_binbuf);
            ac = binbuf_getnatom(z->te_binbuf);
            for(int i = 0; i < ac; i++)
            {
                if(atom_gettype(av+i) == A_FLOAT && atom_getfloat(av+i) > x->l_noutputs)
                    x->l_noutputs = atom_getfloat(av+i);
            }
            ac = 0;
            av = NULL;
        }
        else if(eobj_getclassname(y) == gensym("hoa.dac~"))
        {
            h = (t_my_hoa_dac *)y;
            for(int i = 0; i < h->x_n; i++)
            {
                if(h->x_vec[i] > x->l_noutputs)
                    x->l_noutputs = h->x_vec[i];
            }
        }
        else if(eobj_getclassname(y) == gensym("adc~"))
        {
            z = (t_object *)y;
            av = binbuf_getvec(z->te_binbuf);
            ac = binbuf_getnatom(z->te_binbuf);
            for(int i = 0; i < ac; i++)
            {
                if(atom_gettype(av+i) == A_FLOAT && atom_getfloat(av+i) > x->l_ninputs)
                    x->l_ninputs = atom_getfloat(av+i);
            }
            ac = 0;
            av = NULL;
        }
    }
    
    atom_setlong(argv, x->l_ninputs);
    atom_setlong(argv+1, x->l_noutputs);
    if(sym_camomile1572->s_thing)
    {
        pd_typedmess((t_pd *)sym_camomile1572->s_thing, gensym("io"), 2, argv);
    }
    clock_set(x->l_clock, 100);
}

void informer_output(t_informer *x)
{
    outlet_float(x->l_out, x->baba);
    x->baba = 1 - x->baba;
    clock_delay(x->l_clock, 100);
}

void informer_click(t_informer *x)
{
    ;
}

void informer_free(t_informer *x)
{
	clock_free(x->l_clock);
}

void informer_assist(t_informer *x, void *b, long m, long a, char *s)
{
	;
}


