/*
 * Cream Library
 * Copyright (C) 2013 Pierre Guillot, CICM - Université Paris 8
 * All rights reserved.
 * Website  : https://github.com/CICM/CreamLibrary
 * Contacts : cicm.mshparisnord@gmail.com
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 */

#include "../c.library.hpp"

typedef struct _toggle
{
	t_ebox      j_box;
    t_outlet*   f_out;
	t_rgba		f_color_background;
	t_rgba		f_color_border;
	t_rgba		f_color_cross;
    char        f_active;
} t_toggle;

static t_eclass *toggle_class;

static void toggle_output(t_toggle *x)
{
    t_pd* send = ebox_getsender((t_ebox *) x);
    ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
    ebox_redraw((t_ebox *)x);
    outlet_float(x->f_out, (float)x->f_active);
    if(send)
    {
        pd_float(send, (float)x->f_active);
    }
}

static void toggle_getdrawparams(t_toggle *x, t_object *patcherview, t_edrawparams *params)
{
	params->d_borderthickness   = 2;
	params->d_cornersize        = 2;
    params->d_bordercolor       = x->f_color_border;
    params->d_boxfillcolor      = x->f_color_background;
}

static void toggle_oksize(t_toggle *x, t_rect *newrect)
{
    newrect->width = pd_clip_min(newrect->width, 15.);
    newrect->height = pd_clip_min(newrect->height, 15.);
}

static void toggle_set(t_toggle *x, float f)
{
    x->f_active = f == 0 ? 0 : 1;
    ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
    ebox_redraw((t_ebox *)x);
}

static void toggle_float(t_toggle *x, float f)
{
    x->f_active = f == 0 ? 0 : 1;
    toggle_output(x);
}

static void toggle_bang(t_toggle *x)
{
    x->f_active = x->f_active == 0 ? 1 : 0;
    toggle_output(x);
}


static t_pd_err toggle_notify(t_toggle *x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
	if(msg == cream_sym_attr_modified)
	{
		if(s == cream_sym_bgcolor || s == cream_sym_bdcolor || s == cream_sym_crcolor)
		{
			ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
		}
	}
	return 0;
}

static void toggle_paint(t_toggle *x, t_object *view)
{
    t_elayer *g;
    t_rect rect;
    ebox_get_rect_for_view((t_ebox *)x, &rect);
    g = ebox_start_layer((t_ebox *)x, cream_sym_background_layer, rect.width, rect.height);
    if(g)
    {
        if(x->f_active)
        {
            egraphics_set_color_rgba(g, &x->f_color_cross);
            egraphics_set_line_width(g, 2);
            egraphics_line_fast(g, 0, 0, rect.width, rect.height);
            egraphics_line_fast(g, 0, rect.height, rect.width, 0);
        }
        ebox_end_layer((t_ebox*)x, cream_sym_background_layer);
    }
    ebox_paint_layer((t_ebox *)x, cream_sym_background_layer, 0., 0.);
}

static void toggle_mousedown(t_toggle *x, t_object *patcherview, t_pt pt, long modifiers)
{
    toggle_bang(x);
}

static void toggle_preset(t_toggle *x, t_binbuf *b)
{
    t_atom av[2];
    atom_setsym(av, &s_float);
    atom_setfloat(av+1, x->f_active);
    binbuf_add(b, 2, av);
}

static void *toggle_new(t_symbol *s, int argc, t_atom *argv)
{
    t_toggle *x = (t_toggle *)eobj_new(toggle_class);
    t_binbuf* d = binbuf_via_atoms(argc, argv);
    
    if(x && d)
    {
        ebox_new((t_ebox *)x, 0 | EBOX_GROWLINK);
        x->f_active = 0;
        x->f_out = outlet_new((t_object *)x, &s_float);
        ebox_attrprocess_viabinbuf(x, d);
        ebox_ready((t_ebox *)x);
        
        return x;
    }
    
    return NULL;
}

extern "C" void setup_c0x2etoggle(void)
{
    t_eclass *c = eclass_new("c.toggle", (method)toggle_new, (method)ebox_free, (short)sizeof(t_toggle), 0L, A_GIMME, 0);
    if(c)
    {
        eclass_guiinit(c, 0);
        eclass_addmethod(c, (method) toggle_paint,           "paint",            A_NULL, 0);
        eclass_addmethod(c, (method) toggle_notify,          "notify",           A_NULL, 0);
        eclass_addmethod(c, (method) toggle_getdrawparams,   "getdrawparams",    A_NULL, 0);
        eclass_addmethod(c, (method) toggle_oksize,          "oksize",           A_NULL, 0);
        eclass_addmethod(c, (method) toggle_float,           "float",            A_FLOAT,0);
        eclass_addmethod(c, (method) toggle_set,             "set",              A_FLOAT,0);
        eclass_addmethod(c, (method) toggle_bang,            "bang",             A_NULL, 0);
        eclass_addmethod(c, (method) toggle_mousedown,       "mousedown",        A_NULL, 0);
        eclass_addmethod(c, (method) toggle_preset,          "preset",           A_NULL, 0);
        
        CLASS_ATTR_INVISIBLE            (c, "fontname", 1);
        CLASS_ATTR_INVISIBLE            (c, "fontweight", 1);
        CLASS_ATTR_INVISIBLE            (c, "fontslant", 1);
        CLASS_ATTR_INVISIBLE            (c, "fontsize", 1);
        CLASS_ATTR_DEFAULT              (c, "size", 0, "15. 15.");
        
        CLASS_ATTR_RGBA                 (c, "bgcolor", 0, t_toggle, f_color_background);
        CLASS_ATTR_LABEL                (c, "bgcolor", 0, "Background Color");
        CLASS_ATTR_ORDER                (c, "bgcolor", 0, "1");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bgcolor", 0, "0.75 0.75 0.75 1.");
        CLASS_ATTR_STYLE                (c, "bgcolor", 0, "color");
        
        CLASS_ATTR_RGBA                 (c, "bdcolor", 0, t_toggle, f_color_border);
        CLASS_ATTR_LABEL                (c, "bdcolor", 0, "Border Color");
        CLASS_ATTR_ORDER                (c, "bdcolor", 0, "2");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bdcolor", 0, "0.5 0.5 0.5 1.");
        CLASS_ATTR_STYLE                (c, "bdcolor", 0, "color");
        
        CLASS_ATTR_RGBA                 (c, "crcolor", 0, t_toggle, f_color_cross);
        CLASS_ATTR_LABEL                (c, "crcolor", 0, "Cross Color");
        CLASS_ATTR_ORDER                (c, "crcolor", 0, "3");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "crcolor", 0, "0.5 0.5 0.5 1.");
        CLASS_ATTR_STYLE                (c, "crcolor", 0, "color");
        
        eclass_register(CLASS_BOX, c);
        toggle_class = c;
    }
}







