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
#include <float.h>

typedef struct _slider
{
	t_ebox          j_box;
    t_outlet*       f_out;
    int             f_bdsize;
	t_rgba          f_color_background;
	t_rgba          f_color_border;
	t_rgba          f_color_knob;
    char            f_direction;
    char            f_relative;
    float           f_value_ref;
    float           f_value_last;
    void*           f_dummy;
} t_slider;

static t_eclass *slider_class;

static void slider_output(t_slider *x)
{
    t_pd* send = ebox_getsender((t_ebox *) x);
    const float val = ebox_parameter_getvalue((t_ebox *)x, 1);
    outlet_float(x->f_out, val);
    if(send)
    {
        pd_float(send,  val);
    }
}

static void slider_float(t_slider *x, float f)
{
    ebox_parameter_setvalue((t_ebox *)x, 1, f, 1);
    slider_output(x);
    ebox_invalidate_layer((t_ebox *)x, NULL, cream_sym_background_layer);
    ebox_redraw((t_ebox *)x);
}

static void slider_set(t_slider *x, float f)
{
    ebox_parameter_setvalue((t_ebox *)x, 1, f, 0);
    ebox_invalidate_layer((t_ebox *)x, NULL, cream_sym_background_layer);
    ebox_redraw((t_ebox *)x);
}

static void slider_bang(t_slider *x, float f)
{
    ebox_parameter_notify_changes((t_ebox *)x, 1);
    slider_output(x);
}

static void slider_getdrawparams(t_slider *x, t_object *view, t_edrawparams *params)
{
	params->d_borderthickness   = x->f_bdsize;
	params->d_cornersize        = 2;
    params->d_bordercolor       = x->f_color_border;
    params->d_boxfillcolor      = x->f_color_background;
}

static void slider_oksize(t_slider *x, t_rect *newrect)
{
    newrect->width = pd_clip_min(newrect->width, 8.);
    newrect->height = pd_clip_min(newrect->height, 8.);
    x->f_direction = newrect->width > newrect->height ? 1 : 0;
    if(x->f_direction)
    {
        newrect->width = pd_clip_min(newrect->width, 50.);
    }
    else
    {
        newrect->height = pd_clip_min(newrect->height, 50.);
    }
}

static t_pd_err slider_notify(t_slider *x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
	if(msg == cream_sym_attr_modified)
	{
		if(s == cream_sym_bgcolor || s == cream_sym_bdcolor || s == cream_sym_kncolor)
		{
			ebox_invalidate_layer((t_ebox *)x, NULL, cream_sym_background_layer);
		}
	}
    else if(msg == cream_sym_value_changed)
    {
        slider_output(x);
        ebox_invalidate_layer((t_ebox *)x, NULL, cream_sym_background_layer);
        ebox_redraw((t_ebox *)x);
    }
	return 0;
}

static void slider_paint(t_slider *x, t_object *view)
{
	t_rect rect;
	ebox_getdrawbounds((t_ebox *)x, view,  &rect);
    
    t_elayer *g = ebox_start_layer((t_ebox *)x, view, cream_sym_background_layer, rect.width, rect.height);
    
    if (g)
    {
        const float temp  = ebox_parameter_getvalue_normalized((t_ebox *)x, 1);
        const float value = (ebox_parameter_isinverted((t_ebox *)x, 1)) ? (1.f -  temp) : (temp);
        elayer_set_color_rgba(g, &x->f_color_knob);
        elayer_set_line_width(g, 2);
        if(x->f_direction)
        {
            const float pos = (rect.width - 4.f) * value + 2.f;
            elayer_line_fast(g, pos, 2.f, pos, rect.height - 2.f);
        }
        else
        {
            const float pos = (rect.height - 4.f) * (1. - value) + 2.f;
            elayer_line_fast(g, 2.f, pos, rect.width - 2.f, pos);
        }
        ebox_end_layer((t_ebox*)x, view, cream_sym_background_layer);
    }
    ebox_paint_layer((t_ebox *)x, view, cream_sym_background_layer, 0., 0.);
}

static float slider_getvalue(char direction, t_rect const* rect, t_pt const* pt, float min, float max)
{
    const char inverted = (char)(min < max);
    if(direction && inverted)
        return (pt->x - 2.f) * (max - min) / (rect->width - 4.f) + min;
    else if(direction && !inverted)
        return (rect->width - pt->x - 2.f) * (min - max) / (rect->width - 4.f) + max;
    else if(!direction && inverted)
        return (rect->height - pt->y - 2.f) * (max - min) / (rect->height - 4.f) + min;
    return (pt->y - 2.f) * (min - max) / (rect->height - 4.f) + max;
}

static void slider_mousedown(t_slider *x, t_object *view, t_pt pt, long modifiers)
{
    t_rect rect;
    const float min = ebox_parameter_getmin((t_ebox *)x, 1);
    const float max = ebox_parameter_getmax((t_ebox *)x, 1);
    ebox_getdrawbounds((t_ebox *)x, view,  &rect);
    ebox_parameter_begin_changes((t_ebox *)x, 1);
    if(modifiers == EMOD_SHIFT)
    {
        x->f_relative = 1;
        x->f_value_last =  ebox_parameter_getvalue((t_ebox *)x, 1);
        if(min < max)
        {
            x->f_value_ref = pd_clip(slider_getvalue(x->f_direction, &rect, &pt, min, max), min, max);
        }
        else
        {
            x->f_value_ref = pd_clip(slider_getvalue(x->f_direction, &rect, &pt, min, max), max, min);
        }
    }
    else
    {
        x->f_relative = 0;
        ebox_parameter_setvalue((t_ebox *)x, 1, slider_getvalue(x->f_direction, &rect, &pt, min, max), 1);
        slider_output(x);
        ebox_invalidate_layer((t_ebox *)x, NULL, cream_sym_background_layer);
        ebox_redraw((t_ebox *)x);
    }
}

static void slider_mousedrag(t_slider *x, t_object *view, t_pt pt, long modifiers)
{
    t_rect rect;
    const float min = ebox_parameter_getmin((t_ebox *)x, 1);
    const float max = ebox_parameter_getmax((t_ebox *)x, 1);
    ebox_getdrawbounds((t_ebox *)x, view,  &rect);
    if(x->f_relative)
    {
        const float refvalue = slider_getvalue(x->f_direction, &rect, &pt, min, max);
        ebox_parameter_setvalue((t_ebox *)x, 1, x->f_value_last + refvalue - x->f_value_ref, 1);
        const float newvalue = ebox_parameter_getvalue((t_ebox *)x, 1);
        if(newvalue == min || newvalue == max)
        {
            x->f_value_last = newvalue;
            x->f_value_ref  = refvalue;
        }
    }
    else
    {
         ebox_parameter_setvalue((t_ebox *)x, 1, slider_getvalue(x->f_direction, &rect, &pt, min, max), 1);
    }
    
    slider_output(x);
    ebox_invalidate_layer((t_ebox *)x, NULL, cream_sym_background_layer);
    ebox_redraw((t_ebox *)x);
}

static void slider_mouseup(t_slider *x, t_object *view, t_pt pt, long modifiers)
{
     ebox_parameter_end_changes((t_ebox *)x, 1);
}

static t_pd_err slider_minmax_set(t_slider *x, t_object *attr, int ac, t_atom *av)
{
    if(ac && av)
    {
        const float min = pd_clip((atom_gettype(av) == A_FLOAT) ? atom_getfloat(av) : ebox_parameter_getmin((t_ebox *)x, 1), -FLT_MAX, FLT_MAX);
        const float max = pd_clip((ac > 1  && atom_gettype(av+1) == A_FLOAT) ? atom_getfloat(av+1) : ebox_parameter_getmax((t_ebox *)x, 1), -FLT_MAX, FLT_MAX);
        ebox_parameter_setminmax((t_ebox *)x, 1, min, max);
    }
    
    ebox_invalidate_layer((t_ebox *)x, NULL, cream_sym_background_layer);
    ebox_redraw((t_ebox *)x);
    return 0;
}


static t_pd_err slider_minmax_get(t_slider *x, t_object *attr, int* ac, t_atom **av)
{
    *ac = 2;
    *av = (t_atom *)malloc(2 * sizeof(t_atom));
    if(*av)
    {
        atom_setfloat(*av, ebox_parameter_getmin((t_ebox *)x, 1));
        atom_setfloat(*av+1, ebox_parameter_getmax((t_ebox *)x, 1));
    }
    else
    {
        *ac = 0;
    }
    return 0;
}


static void *slider_new(t_symbol *s, int argc, t_atom *argv)
{
    t_slider *x = (t_slider *)eobj_new(slider_class);
    t_binbuf* d = binbuf_via_atoms(argc,argv);
    
    if(x && d)
    {
        ebox_new((t_ebox *)x, 0 | EBOX_GROWINDI);
        ebox_parameter_create((t_ebox *)x, 1);
        x->f_out = outlet_new((t_object *)x, &s_float);
        
        eobj_attr_read(x, d);
        ebox_ready((t_ebox *)x);
        return x;
    }
    
    return NULL;
}

extern "C" void setup_c0x2eslider(void)
{
    t_eclass *c = eclass_new("c.slider", (t_method)slider_new, (t_method)ebox_free, (short)sizeof(t_slider), 0L, A_GIMME, 0);
    if(c)
    {
        eclass_guiinit(c, 0);
        
        eclass_addmethod(c, (t_method) slider_paint,          "paint",            A_NULL, 0);
        eclass_addmethod(c, (t_method) slider_notify,         "notify",           A_NULL, 0);
        eclass_addmethod(c, (t_method) slider_getdrawparams,  "getdrawparams",    A_NULL, 0);
        eclass_addmethod(c, (t_method) slider_oksize,         "oksize",           A_NULL, 0);
        
        eclass_addmethod(c, (t_method) slider_set,            "set",              A_FLOAT,0);
        eclass_addmethod(c, (t_method) slider_float,          "float",            A_FLOAT,0);
        eclass_addmethod(c, (t_method) slider_bang,           "bang",             A_NULL, 0);
        
        eclass_addmethod(c, (t_method) slider_mousedown,      "mousedown",        A_NULL, 0);
        eclass_addmethod(c, (t_method) slider_mousedrag,      "mousedrag",        A_NULL, 0);
        eclass_addmethod(c, (t_method) slider_mouseup,        "mouseup",          A_NULL, 0);
        
        CLASS_ATTR_DEFAULT              (c, "size", 0, "15. 120.");
        
        CLASS_ATTR_FLOAT_ARRAY          (c, "minmax", 0, t_slider, f_dummy, 2);
        CLASS_ATTR_ORDER                (c, "minmax", 0, "3");
        CLASS_ATTR_LABEL                (c, "minmax", 0, "Min/Max Values");
        CLASS_ATTR_DEFAULT              (c, "minmax", 0, "0 1");
        CLASS_ATTR_ACCESSORS            (c, "minmax", slider_minmax_get, slider_minmax_set);
        CLASS_ATTR_SAVE                 (c, "minmax", 1);
        
        CLASS_ATTR_INT                  (c, "bdsize", 0, t_slider, f_bdsize);
        CLASS_ATTR_LABEL                (c, "bdsize", 0, "Border Size");
        CLASS_ATTR_ORDER                (c, "bdsize", 0, "1");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bdsize", 0, "2");
        CLASS_ATTR_FILTER_CLIP          (c, "bdsize", 0, 4);
        CLASS_ATTR_STYLE                (c, "bdsize", 0, "number");
        
        CLASS_ATTR_RGBA                 (c, "bgcolor", 0, t_slider, f_color_background);
        CLASS_ATTR_LABEL                (c, "bgcolor", 0, "Background Color");
        CLASS_ATTR_ORDER                (c, "bgcolor", 0, "3");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bgcolor", 0, "0.75 0.75 0.75 1.");
        CLASS_ATTR_STYLE                (c, "bgcolor", 0, "color");
        
        CLASS_ATTR_RGBA                 (c, "bdcolor", 0, t_slider, f_color_border);
        CLASS_ATTR_LABEL                (c, "bdcolor", 0, "Border Color");
        CLASS_ATTR_ORDER                (c, "bdcolor", 0, "4");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bdcolor", 0, "0.5 0.5 0.5 1.");
        CLASS_ATTR_STYLE                (c, "bdcolor", 0, "color");
        
        CLASS_ATTR_RGBA                 (c, "kncolor", 0, t_slider, f_color_knob);
        CLASS_ATTR_LABEL                (c, "kncolor", 0, "Knob Color");
        CLASS_ATTR_ORDER                (c, "kncolor", 0, "5");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "kncolor", 0, "0.5 0.5 0.5 1.");
        CLASS_ATTR_STYLE                (c, "kncolor", 0, "color");

        eclass_register(CLASS_BOX, c);
        slider_class = c;
    }
}





