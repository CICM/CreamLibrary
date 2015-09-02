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
    t_outlet*   f_outlet;
	t_rgba		f_color_background;
	t_rgba		f_color_border;
	t_rgba		f_color_cross;
} t_toggle;

static t_eclass *toggle_class;

static void toggle_output(t_toggle *x)
{
    t_pd* send = ebox_getsender((t_ebox *)x);
    const float val = ebox_parameter_getvalue((t_ebox *)x, 1);
    outlet_float(x->f_outlet, val);
    if(send)
    {
        pd_float(send, val);
    }
}

static void toggle_float(t_toggle *x, float f)
{
    ebox_parameter_setvalue((t_ebox *)x, 1, (f == 0.f) ? 0.f : 1.f, 1);
    toggle_output(x);
    ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
    ebox_redraw((t_ebox *)x);
}

static void toggle_set(t_toggle *x, float f)
{
    ebox_parameter_setvalue((t_ebox *)x, 1, (f == 0.f) ? 0.f : 1.f, 0);
    ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
    ebox_redraw((t_ebox *)x);
}

static void toggle_bang(t_toggle *x)
{
    const float f = ebox_parameter_getvalue((t_ebox *)x, 1);
    ebox_parameter_setvalue((t_ebox *)x, 1, (f == 0.f) ? 1.f : 0.f, 1);
    toggle_output(x);
    ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
    ebox_redraw((t_ebox *)x);
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

static t_pd_err toggle_notify(t_toggle *x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
	if(msg == cream_sym_attr_modified)
	{
		if(s == cream_sym_bgcolor || s == cream_sym_bdcolor || s == cream_sym_crcolor)
		{
			ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
		}
	}
    else if(msg == cream_sym_param_changed)
    {
        toggle_output(x);
        ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
        ebox_redraw((t_ebox *)x);
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
        const char state = ebox_parameter_getvalue((t_ebox *)x, 1);
        if(state)
        {
            egraphics_set_color_rgba(g, &x->f_color_cross);
            egraphics_set_line_width(g, 2);
            egraphics_line_fast(g, 2.f, 2.f, rect.width - 2.f, rect.height - 2.f);
            egraphics_line_fast(g, 2.f, rect.height - 2.f, rect.width - 2.f, 2.f);
        }
        ebox_end_layer((t_ebox*)x, cream_sym_background_layer);
    }
    ebox_paint_layer((t_ebox *)x, cream_sym_background_layer, 0., 0.);
}

static void toggle_mousedown(t_toggle *x, t_object *patcherview, t_pt pt, long modifiers)
{
    toggle_bang(x);
}

static void toggle_setter_t(t_toggle *x, int index, char const* text)
{
    if(!strcmp(text, "on"))
    {
        ebox_parameter_setvalue((t_ebox *)x, index, 1.f, 0);
    }
    else if(!strcmp(text, "off"))
    {
        ebox_parameter_setvalue((t_ebox *)x, index, 0.f, 0);
    }
    else if(isdigit(text[0]))
    {
        ebox_parameter_setvalue((t_ebox *)x, index, atof(text), 0);
    }
}

static void toggle_getter_t(t_toggle *x, int index, char* text)
{
    if(ebox_parameter_getvalue((t_ebox *)x, index))
    {
        sprintf(text, "on");
    }
    else
    {
        sprintf(text, "off");
    }
}

static void *toggle_new(t_symbol *s, int argc, t_atom *argv)
{
    t_toggle *x = (t_toggle *)eobj_new(toggle_class);
    t_binbuf* d = binbuf_via_atoms(argc, argv);
    
    if(x && d)
    {
        ebox_new((t_ebox *)x, 0 | EBOX_GROWLINK);
        ebox_parameter_create((t_ebox *)x, 1);
        ebox_parameter_setminmax((t_ebox *)x, 1, 0, 1);
        ebox_parameter_setnstep((t_ebox *)x, 1, 2);
        ebox_parameter_setflags((t_ebox *)x, 1, 0 | EPARAM_STATIC_MIN | EPARAM_STATIC_MAX | EPARAM_STATIC_NSTEPS);
        ebox_parameter_setsettergetter_text((t_ebox *)x, 1, (t_param_setter_t)toggle_setter_t, t_param_getter_t(toggle_getter_t));
        
        x->f_outlet = outlet_new((t_object *)x, &s_float);
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







