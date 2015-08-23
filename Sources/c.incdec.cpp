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

typedef struct _incdec
{
	t_ebox      j_box;

    t_outlet*   f_out;
    float       f_increment;
    float       f_value;
	t_rgba		f_color_background;
	t_rgba		f_color_border;
	t_rgba		f_color_arrow;
    char        f_mouse_down;
} t_incdec;

static t_eclass *incdec_class;

static void *incdec_new(t_symbol *s, int argc, t_atom *argv)
{
	t_incdec *x = (t_incdec *)eobj_new(incdec_class);
    t_binbuf* d = binbuf_via_atoms(argc,argv);
	
    if(x && d)
    {
        ebox_new((t_ebox *)x, 0 | EBOX_GROWINDI);
        
        x->f_value = 0.;
        x->f_mouse_down = 0;
        x->f_out = outlet_new((t_object *)x, &s_float);
        ebox_attrprocess_viabinbuf(x, d);
        ebox_ready((t_ebox *)x);
    }
    
	return (x);
}

static void incdec_output(t_incdec *x)
{
    t_pd* send = ebox_getsender((t_ebox *) x);
    outlet_float(x->f_out, (float)x->f_value);
    if(send)
    {
        pd_float(send, (float)x->f_value);
    }
}

static void incdec_set(t_incdec *x, float f)
{
    x->f_value = f;
}

static void incdec_float(t_incdec *x, float f)
{
    if(x->f_value != f)
    {
        x->f_value = f;
        incdec_output(x);
    }
}

static void incdec_inc(t_incdec *x)
{
    x->f_value += x->f_increment;
    incdec_output(x);
}

static void incdec_dec(t_incdec *x)
{
    x->f_value -= x->f_increment;
    incdec_output(x);
}

static t_pd_err incdec_notify(t_incdec *x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
	if(msg == cream_sym_attr_modified)
	{
		if(s == cream_sym_bgcolor || s == cream_sym_bdcolor || s == cream_sym_arcolor)
		{
			ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
		}
	}
	return 0;
}

static void incdec_getdrawparams(t_incdec *x, t_object *patcherview, t_edrawparams *params)
{
    params->d_borderthickness   = 2;
    params->d_cornersize        = 2;
    params->d_bordercolor       = x->f_color_border;
    params->d_boxfillcolor      = x->f_color_background;
}

static void incdec_oksize(t_incdec *x, t_rect *newrect)
{
    newrect->width = pd_clip_min(newrect->width, 15.);
    newrect->height = pd_clip_min(newrect->height, 15.);
}

static void incdec_paint(t_incdec *x, t_object *view)
{
	t_rect rect;
	ebox_get_rect_for_view((t_ebox *)x, &rect);
    t_elayer *g = ebox_start_layer((t_ebox *)x, cream_sym_background_layer, rect.width, rect.height);
    if (g)
    {
        egraphics_set_color_rgba(g, &x->f_color_arrow);
        if(x->f_mouse_down)
        {
            if(x->f_mouse_down == 1)
            {
                egraphics_rectangle(g, 0.f, 0.f, rect.width, rect.height * 0.5f);
            }
            else
            {
                egraphics_rectangle(g, 0.f, rect.height * 0.5f, rect.width, rect.height);
            }
            egraphics_fill(g);
        }
        
        if(x->f_mouse_down == 1)
        {
            egraphics_set_color_rgba(g, &x->f_color_background);
        }
        egraphics_move_to(g, 2.f, rect.height * 0.5f - 3.f);
        egraphics_line_to(g, rect.width - 2.f, rect.height * 0.5f - 3.f);
        egraphics_line_to(g, rect.width * 0.5f, 2.f);
        egraphics_fill(g);
        
        if(x->f_mouse_down == -1)
        {
            egraphics_set_color_rgba(g, &x->f_color_background);
        }
        else
        {
            egraphics_set_color_rgba(g, &x->f_color_arrow);
        }
        egraphics_move_to(g, 2.f, rect.height * 0.5f + 3.f);
        egraphics_line_to(g, rect.width - 2.f, rect.height * 0.5f + 3.f);
        egraphics_line_to(g, rect.width * 0.5f, rect.height - 2.f);
        egraphics_fill(g);
        
        egraphics_set_color_rgba(g, &x->f_color_border);
        egraphics_set_line_width(g, 2.f);
        egraphics_line_fast(g, 0., rect.height * 0.5f, rect.width, rect.height * 0.5f);
        
        ebox_end_layer((t_ebox*)x, cream_sym_background_layer);
    }
    ebox_paint_layer((t_ebox *)x, cream_sym_background_layer, 0., 0.);
}

static void incdec_mousedown(t_incdec *x, t_object *patcherview, t_pt pt, long modifiers)
{
    t_rect rect;
    ebox_get_rect_for_view((t_ebox *)x, &rect);
    if(pt.y < rect.height * 0.5f)
    {
        incdec_inc(x);
        x->f_mouse_down = 1;
    }
    else
    {
        incdec_dec(x);
        x->f_mouse_down = -1;
    }
    ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
    ebox_redraw((t_ebox *)x);
}


static void incdec_mouseup(t_incdec *x, t_object *patcherview, t_pt pt, long modifiers)
{
    x->f_mouse_down = 0;
    ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
    ebox_redraw((t_ebox *)x);
}

extern "C" void setup_c0x2eincdec(void)
{
    t_eclass *c;
    
    c = eclass_new("c.incdec", (method)incdec_new, (method)ebox_free, (short)sizeof(t_incdec), 0L, A_GIMME, 0);
    
    eclass_guiinit(c, 0);
    eclass_addmethod(c, (method) incdec_paint,           "paint",            A_NULL, 0);
    eclass_addmethod(c, (method) incdec_notify,          "notify",           A_NULL, 0);
    eclass_addmethod(c, (method) incdec_getdrawparams,   "getdrawparams",    A_NULL, 0);
    eclass_addmethod(c, (method) incdec_oksize,          "oksize",           A_NULL, 0);
    eclass_addmethod(c, (method) incdec_set,             "set",              A_FLOAT,0);
    eclass_addmethod(c, (method) incdec_float,           "float",            A_FLOAT,0);
    eclass_addmethod(c, (method) incdec_output,          "bang",             A_NULL, 0);
    eclass_addmethod(c, (method) incdec_inc,             "inc",              A_NULL, 0);
    eclass_addmethod(c, (method) incdec_dec,             "dec",              A_NULL, 0);
    eclass_addmethod(c, (method) incdec_mousedown,       "mousedown",        A_NULL, 0);
    eclass_addmethod(c, (method) incdec_mouseup,         "mouseup",          A_NULL, 0);
    
    CLASS_ATTR_INVISIBLE            (c, "fontname", 1);
    CLASS_ATTR_INVISIBLE            (c, "fontweight", 1);
    CLASS_ATTR_INVISIBLE            (c, "fontslant", 1);
    CLASS_ATTR_INVISIBLE            (c, "fontsize", 1);
    CLASS_ATTR_INVISIBLE            (c, "send", 1);
    CLASS_ATTR_DEFAULT              (c, "size", 0, "13 20");
    
    CLASS_ATTR_FLOAT                (c, "step", 0, t_incdec, f_increment);
    CLASS_ATTR_LABEL                (c, "step", 0, "Step increment");
    CLASS_ATTR_FILTER_MIN           (c, "step", 0.);
    CLASS_ATTR_ORDER                (c, "step", 0, "1");
    CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "step", 0, "1.");
    CLASS_ATTR_STYLE                (c, "step", 0, "number");
    CLASS_ATTR_STEP                 (c, "step", 0.1);
    
    CLASS_ATTR_RGBA                 (c, "bgcolor", 0, t_incdec, f_color_background);
    CLASS_ATTR_LABEL                (c, "bgcolor", 0, "Background Color");
    CLASS_ATTR_ORDER                (c, "bgcolor", 0, "1");
    CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bgcolor", 0, "0.75 0.75 0.75 1.");
    CLASS_ATTR_STYLE                (c, "bgcolor", 0, "color");
    
    CLASS_ATTR_RGBA                 (c, "bdcolor", 0, t_incdec, f_color_border);
    CLASS_ATTR_LABEL                (c, "bdcolor", 0, "Border Color");
    CLASS_ATTR_ORDER                (c, "bdcolor", 0, "2");
    CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bdcolor", 0, "0.5 0.5 0.5 1.");
    CLASS_ATTR_STYLE                (c, "bdcolor", 0, "color");
    
    CLASS_ATTR_RGBA                 (c, "arcolor", 0, t_incdec, f_color_arrow);
    CLASS_ATTR_LABEL                (c, "arcolor", 0, "Arrow Color");
    CLASS_ATTR_ORDER                (c, "arcolor", 0, "3");
    CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "arcolor", 0, "0. 0. 0. 1.");
    CLASS_ATTR_STYLE                (c, "arcolor", 0, "color");
    
    eclass_register(CLASS_BOX, c);
    incdec_class = c;
}







