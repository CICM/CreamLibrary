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

typedef struct _knob
{
	t_ebox      j_box;
    t_outlet*   f_outlet;
	t_rgba		f_color_background;
	t_rgba		f_color_border;
	t_rgba		f_color_needle;
    char        f_endless;
    char        f_circular;
    float       f_reference;
} t_knob;

static t_eclass *knob_class;

static void knob_output(t_knob *x)
{
    t_pd* send = ebox_getsender((t_ebox *) x);
    const float val = ebox_parameter_getvalue((t_ebox *)x, 1);
    outlet_float(x->f_outlet, val);
    if(send)
    {
        pd_float(send, val);
    }
}

static void knob_float(t_knob *x, float f)
{
    if(x->f_endless)
    {
        const float min = ebox_parameter_getmin((t_ebox *)x, 1);
        const float max = ebox_parameter_getmax((t_ebox *)x, 1);
        f = pd_wrap(f, min, max);
    }
    
    ebox_parameter_setvalue((t_ebox *)x, 1, f, 1);
    knob_output(x);
    ebox_invalidate_layer((t_ebox *)x, cream_sym_needle_layer);
    ebox_redraw((t_ebox *)x);
}

static void knob_set(t_knob *x, float f)
{
    if(x->f_endless)
    {
        const float min = ebox_parameter_getmin((t_ebox *)x, 1);
        const float max = ebox_parameter_getmax((t_ebox *)x, 1);
        f = pd_wrap(f, min, max);
    }
    ebox_parameter_setvalue((t_ebox *)x, 1, f, 0);
    ebox_invalidate_layer((t_ebox *)x, cream_sym_needle_layer);
    ebox_redraw((t_ebox *)x);
}

static void knob_bang(t_knob *x, float f)
{
    ebox_parameter_notify_changes((t_ebox *)x, 1);
    knob_output(x);
}

static void knob_getdrawparams(t_knob *x, t_object *patcherview, t_edrawparams *params)
{
    params->d_borderthickness   = 2.f;
    params->d_cornersize        = 2.f;
    params->d_bordercolor       = x->f_color_border;
    params->d_boxfillcolor      = x->f_color_background;
}

static void knob_oksize(t_knob *x, t_rect *newrect)
{
    newrect->width = pd_clip_min(newrect->width, 20.f);
    newrect->height = pd_clip_min(newrect->height, 20.f);
}

static t_pd_err knob_notify(t_knob *x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
	if(msg == cream_sym_attr_modified)
	{
		if(s == cream_sym_bgcolor || s == cream_sym_bdcolor || s == cream_sym_necolor || s == cream_sym_endless)
		{
			ebox_invalidate_layer((t_ebox *)x, cream_sym_needle_layer);
            ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
		}
	}
    else if(msg == cream_sym_param_changed)
    {
        knob_output(x);
        ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
        ebox_redraw((t_ebox *)x);
    }
	return 0;
}

static void draw_background(t_knob *x, t_object *view, t_rect *rect)
{
	t_elayer *g = ebox_start_layer((t_ebox *)x, cream_sym_background_layer, rect->width, rect->height);
	if (g)
	{
        const float size = rect->width * 0.5f;
        egraphics_set_color_rgba(g, &x->f_color_border);
        egraphics_set_line_width(g, 2.f);
        egraphics_circle(g, size, size, size - 2.f);
        egraphics_stroke(g);
        
        ebox_end_layer((t_ebox*)x, cream_sym_background_layer);
	}
	ebox_paint_layer((t_ebox *)x, cream_sym_background_layer, 0., 0.);
}

static void draw_needle(t_knob *x, t_object *view, t_rect *rect)
{
    t_elayer *g = ebox_start_layer((t_ebox *)x, cream_sym_needle_layer, rect->width, rect->height);
    
    if(g)
	{
        const float size    = rect->width * 0.5f;
        egraphics_set_color_rgba(g, &x->f_color_needle);
        egraphics_set_line_width(g, 2.f);
        if(ebox_parameter_isinverted((t_ebox *)x, 1))
        {
            const float ratio1  = x->f_endless ? (float)(EPD_2PI) : (float)(EPD_PI + EPD_PI2);
            const float ratio2  = x->f_endless ? (float)(EPD_PI2) : (float)(EPD_PI2 + EPD_PI4);
            const float angle   = (1.f - ebox_parameter_getvalue_normalized((t_ebox *)x, 1)) * ratio1 + ratio2;
            
            egraphics_line(g,
                           pd_abscissa(size - 10.f, angle) + size,
                           pd_ordinate(size - 10.f, angle) + size,
                           pd_abscissa(size - 2.f, angle) + size,
                           pd_ordinate(size - 2.f, angle) + size);
        }
        else
        {
            const float ratio1  = x->f_endless ? (float)(EPD_2PI) : (float)(EPD_PI + EPD_PI2);
            const float ratio2  = x->f_endless ? (float)(EPD_PI2) : (float)(EPD_PI2 + EPD_PI4);
            const float angle   = ebox_parameter_getvalue_normalized((t_ebox *)x, 1) * ratio1 + ratio2;
            
            egraphics_line(g,
                           pd_abscissa(size - 10.f, angle) + size,
                           pd_ordinate(size - 10.f, angle) + size,
                           pd_abscissa(size - 2.f, angle) + size,
                           pd_ordinate(size - 2.f, angle) + size);
        }
        
        
        egraphics_stroke(g);
        ebox_end_layer((t_ebox*)x, cream_sym_needle_layer);
    }
   
    ebox_paint_layer((t_ebox *)x, cream_sym_needle_layer, 0., 0.);

}

static void knob_paint(t_knob *x, t_object *view)
{
    t_rect rect;
    ebox_get_rect_for_view((t_ebox *)x, &rect);
    
    draw_background(x, view, &rect);
    draw_needle(x, view, &rect);
}


static void knob_mousedown(t_knob *x, t_object *patcherview, t_pt pt, long modifiers)
{
    t_rect rect;
    ebox_get_rect_for_view((t_ebox *)x, &rect);
    ebox_parameter_begin_changes((t_ebox *)x, 1);
    if(x->f_circular)
    {
        const float size  = rect.width * 0.5f;
        const float angle = pd_angle(pt.x - size, -(size - pt.y));
        if(x->f_endless)
        {
            const float value = pd_wrap((angle - EPD_PI2) / EPD_2PI, 0.f, 1.f);
            if(ebox_parameter_isinverted((t_ebox *)x, 1))
            {
                ebox_parameter_setvalue_normalized((t_ebox *)x, 1, 1.f - value, 0.);
            }
            else
            {
                ebox_parameter_setvalue_normalized((t_ebox *)x, 1, value, 0.);
            }
            
        }
        else
        {
            const float value = (pd_clip(pd_wrap((angle - EPD_PI2) / EPD_2PI, 0.f, 1.f), 0.125f, 0.875f) - 0.125f) / 0.75f;
            if(ebox_parameter_isinverted((t_ebox *)x, 1))
            {
                ebox_parameter_setvalue_normalized((t_ebox *)x, 1, 1.f - value, 0.);
            }
            else
            {
                ebox_parameter_setvalue_normalized((t_ebox *)x, 1, value, 0.);
            }
        }
        
        knob_output(x);
        ebox_invalidate_layer((t_ebox *)x, cream_sym_needle_layer);
        ebox_redraw((t_ebox *)x);
    }
    else
    {
        x->f_reference = pt.y;
    }
}

static void knob_mousedrag(t_knob *x, t_object *patcherview, t_pt pt, long modifiers)
{
    t_rect rect;
    ebox_get_rect_for_view((t_ebox *)x, &rect);
    if(x->f_circular)
    {
        const float size  = rect.width * 0.5f;
        const float angle = pd_angle(pt.x - size, -(size - pt.y));
        if(x->f_endless)
        {
            const float value = pd_wrap((angle - EPD_PI2) / EPD_2PI, 0.f, 1.f);
            if(ebox_parameter_isinverted((t_ebox *)x, 1))
            {
                ebox_parameter_setvalue_normalized((t_ebox *)x, 1, 1.f - value, 0.);
            }
            else
            {
                ebox_parameter_setvalue_normalized((t_ebox *)x, 1, value, 0.);
            }
        }
        else
        {
            const float value = (pd_clip(pd_wrap((angle - EPD_PI2) / EPD_2PI, 0.f, 1.f), 0.125f, 0.875f) - 0.125f) / 0.75f;
            if(ebox_parameter_isinverted((t_ebox *)x, 1))
            {
                ebox_parameter_setvalue_normalized((t_ebox *)x, 1, 1.f - value, 0.);
            }
            else
            {
                ebox_parameter_setvalue_normalized((t_ebox *)x, 1, value, 0.);
            }
        }
        
        knob_output(x);
        ebox_invalidate_layer((t_ebox *)x, cream_sym_needle_layer);
        ebox_redraw((t_ebox *)x);
    }
    else
    {
        const float current = ebox_parameter_getvalue_normalized((t_ebox *)x, 1);
        const float diff    = (x->f_reference - pt.y) / (rect.width) * (ebox_parameter_isinverted((t_ebox *)x, 1) ? -1.f : 1.f);
        if(x->f_endless)
        {
            ebox_parameter_setvalue_normalized((t_ebox *)x, 1, pd_wrap(current + diff, 0.f, 1.f), 0.);
        }
        else
        {
            ebox_parameter_setvalue_normalized((t_ebox *)x, 1, pd_clip(current + diff, 0.f, 1.f), 0.);
        }
        x->f_reference      = pt.y;
    }
    knob_output(x);
    ebox_invalidate_layer((t_ebox *)x, cream_sym_needle_layer);
    ebox_redraw((t_ebox *)x);
}

static void knob_mouseup(t_knob *x, t_object *patcherview, t_pt pt, long modifiers)
{
    ebox_parameter_end_changes((t_ebox *)x, 1);
}

static void *knob_new(t_symbol *s, int argc, t_atom *argv)
{
    t_knob *x = (t_knob *)eobj_new(knob_class);
    t_binbuf* d = binbuf_via_atoms(argc, argv);

    if(x && d)
    {
        ebox_new((t_ebox *)x, 0 | EBOX_GROWLINK);
        ebox_parameter_create((t_ebox *)x, 1);
        x->f_outlet = outlet_new((t_object *)x, &s_float);
        ebox_attrprocess_viabinbuf(x, d);
        ebox_ready((t_ebox *)x);
        
        return x;
    }
    
    return NULL;
}

static _FUNCTION_DEPRECTAED_ void knob_preset(t_knob *x, t_binbuf *b)
{
    binbuf_addv(b, (char *)"sf", &s_float, ebox_parameter_getvalue((t_ebox *)x, 1));
}

extern "C" void setup_c0x2eknob(void)
{
    t_eclass *c = eclass_new("c.knob", (method)knob_new, (method)ebox_free, (short)sizeof(t_knob), 0L, A_GIMME, 0);
    
    if(c)
    {
        eclass_guiinit(c, 0);
        
        eclass_addmethod(c, (method) knob_paint,            "paint",            A_NULL, 0);
        eclass_addmethod(c, (method) knob_notify,           "notify",           A_NULL, 0);
        eclass_addmethod(c, (method) knob_getdrawparams,    "getdrawparams",    A_NULL, 0);
        eclass_addmethod(c, (method) knob_oksize,           "oksize",           A_NULL, 0);
        eclass_addmethod(c, (method) knob_set,              "set",              A_FLOAT,0);
        eclass_addmethod(c, (method) knob_float,            "float",            A_FLOAT,0);
        eclass_addmethod(c, (method) knob_bang,             "bang",             A_NULL, 0);
        
        eclass_addmethod(c, (method) knob_mousedown,        "mousedown",        A_NULL, 0);
        eclass_addmethod(c, (method) knob_mousedrag,        "mousedrag",        A_NULL, 0);
        eclass_addmethod(c, (method) knob_mouseup,          "mouseup",          A_NULL, 0);
        
        eclass_addmethod(c, (method) knob_preset,          "preset",            A_NULL, 0);
        
        CLASS_ATTR_DEFAULT              (c, "size", 0, "50. 50.");
        
        CLASS_ATTR_CHAR                 (c, "endless", 0, t_knob, f_endless);
        CLASS_ATTR_LABEL                (c, "endless", 0, "Endless Mode");
        CLASS_ATTR_ORDER                (c, "endless", 0, "1");
        CLASS_ATTR_FILTER_CLIP          (c, "endless", 0, 1);
        CLASS_ATTR_DEFAULT              (c, "endless", 0, "0");
        CLASS_ATTR_SAVE                 (c, "endless", 0);
        CLASS_ATTR_STYLE                (c, "endless", 0, "onoff");
        CLASS_ATTR_PAINT                (c, "endless", 0);
        
        CLASS_ATTR_CHAR                 (c, "circular", 0, t_knob, f_circular);
        CLASS_ATTR_LABEL                (c, "circular", 0, "Circular Mouse Tracking");
        CLASS_ATTR_ORDER                (c, "circular", 0, "1");
        CLASS_ATTR_FILTER_CLIP          (c, "circular", 0, 1);
        CLASS_ATTR_DEFAULT              (c, "circular", 0, "0");
        CLASS_ATTR_SAVE                 (c, "circular", 0);
        CLASS_ATTR_STYLE                (c, "circular", 0, "onoff");
        
        CLASS_ATTR_RGBA                 (c, "bgcolor", 0, t_knob, f_color_background);
        CLASS_ATTR_LABEL                (c, "bgcolor", 0, "Background Color");
        CLASS_ATTR_ORDER                (c, "bgcolor", 0, "1");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bgcolor", 0, "0.75 0.75 0.75 1.");
        CLASS_ATTR_STYLE                (c, "bgcolor", 0, "color");
        
        CLASS_ATTR_RGBA                 (c, "bdcolor", 0, t_knob, f_color_border);
        CLASS_ATTR_LABEL                (c, "bdcolor", 0, "Border Color");
        CLASS_ATTR_ORDER                (c, "bdcolor", 0, "2");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bdcolor", 0, "0.5 0.5 0.5 1.");
        CLASS_ATTR_STYLE                (c, "bdcolor", 0, "color");
        
        CLASS_ATTR_RGBA                 (c, "necolor", 0, t_knob, f_color_needle);
        CLASS_ATTR_LABEL                (c, "necolor", 0, "Needle Color");
        CLASS_ATTR_ORDER                (c, "necolor", 0, "3");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "necolor", 0, "0.5 0.5 0.5 1.");
        CLASS_ATTR_STYLE                (c, "necolor", 0, "color");
        
        eclass_register(CLASS_BOX, c);
        knob_class = c;
    }
}



