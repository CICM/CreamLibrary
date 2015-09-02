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

typedef struct _slider
{
	t_ebox          j_box;
    t_outlet*       f_out;
	t_rgba          f_color_background;
	t_rgba          f_color_border;
	t_rgba          f_color_knob;
    char            f_direction;
    long            f_mode;
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
    ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
    ebox_redraw((t_ebox *)x);
}

static void slider_set(t_slider *x, float f)
{
    ebox_parameter_setvalue((t_ebox *)x, 1, f, 0);
    ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
    ebox_redraw((t_ebox *)x);
}

static void slider_bang(t_slider *x, float f)
{
    ebox_parameter_notify_changes((t_ebox *)x, 1);
    slider_output(x);
}

static void slider_getdrawparams(t_slider *x, t_object *patcherview, t_edrawparams *params)
{
	params->d_borderthickness   = 2;
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
			ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
		}
	}
    else if(msg == cream_sym_param_changed)
    {
        slider_output(x);
        ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
        ebox_redraw((t_ebox *)x);
    }
	return 0;
}

static void slider_paint(t_slider *x, t_object *view)
{
	t_rect rect;
	ebox_get_rect_for_view((t_ebox *)x, &rect);
    
    t_elayer *g = ebox_start_layer((t_ebox *)x, cream_sym_background_layer, rect.width, rect.height);
    
    if (g)
    {
        const float temp  = ebox_parameter_getvalue_normalized((t_ebox *)x, 1);
        const float value = (ebox_parameter_isinverted((t_ebox *)x, 1)) ? (1.f -  temp) : (temp);
        egraphics_set_color_rgba(g, &x->f_color_knob);
        egraphics_set_line_width(g, 2);
        if(x->f_direction)
        {
            const float pos = (rect.width - 4.f) * value + 2.f;
            egraphics_line_fast(g, pos, 2.f, pos, rect.height - 2.f);
        }
        else
        {
            const float pos = (rect.height - 4.f) * (1. - value) + 2.f;
            egraphics_line_fast(g, 2.f, pos, rect.width - 2.f, pos);
        }
        ebox_end_layer((t_ebox*)x, cream_sym_background_layer);
    }
    ebox_paint_layer((t_ebox *)x, cream_sym_background_layer, 0., 0.);
}

static float slider_getvalue(t_slider *x, t_rect const* rect, t_pt const* pt, float min, float max)
{
    const float ratio = ( min <  max) ?  max -  min :  min -  max;
    if(x->f_direction)
    {
        if( min <  max)
        {
            return (pt->x - 4.f) / (rect->width - 4.f) * ratio +  min;
        }
        else
        {
            return (rect->width - pt->x - 8.f) / (rect->width - 4.f) * ratio +  max;
        }
    }
    else
    {
        if( min <  max)
        {
            return (rect->height - pt->y) / (rect->height - 4.f) * ratio +  min;
        }
        else
        {
            return (pt->y - 2.f) / (rect->height - 4.f) * ratio +  max;
        }
    }
}

static void slider_mousedown(t_slider *x, t_object *patcherview, t_pt pt, long modifiers)
{
    t_rect rect;
    const float min = ebox_parameter_getmin((t_ebox *)x, 1);
    const float max = ebox_parameter_getmax((t_ebox *)x, 1);
    ebox_get_rect_for_view((t_ebox *)x, &rect);
    ebox_parameter_begin_changes((t_ebox *)x, 1);
    if(x->f_mode)
    {
        x->f_value_last =  ebox_parameter_getvalue((t_ebox *)x, 1);
        if(min < max)
        {
            x->f_value_ref = pd_clip(slider_getvalue(x, &rect, &pt, min, max), min, max);
        }
        else
        {
            x->f_value_ref = pd_clip(slider_getvalue(x, &rect, &pt, min, max), max, min);
        }
    }
    else
    {
        ebox_parameter_setvalue((t_ebox *)x, 1, slider_getvalue(x, &rect, &pt, min, max), 0);
        slider_output(x);
        ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
        ebox_redraw((t_ebox *)x);
    }
}

static void slider_mousedrag(t_slider *x, t_object *patcherview, t_pt pt, long modifiers)
{
    t_rect rect;
    const float min = ebox_parameter_getmin((t_ebox *)x, 1);
    const float max = ebox_parameter_getmax((t_ebox *)x, 1);
    ebox_get_rect_for_view((t_ebox *)x, &rect);
    if(x->f_mode)
    {
        const float refvalue = slider_getvalue(x, &rect, &pt, min, max);
        ebox_parameter_setvalue((t_ebox *)x, 1, x->f_value_last + refvalue - x->f_value_ref, 0);
        const float newvalue = ebox_parameter_getvalue((t_ebox *)x, 1);
        if(newvalue == min || newvalue == max)
        {
            x->f_value_last = newvalue;
            x->f_value_ref  = refvalue;
        }
    }
    else
    {
         ebox_parameter_setvalue((t_ebox *)x, 1, slider_getvalue(x, &rect, &pt, min, max), 0);
    }
    
    slider_output(x);
    ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
    ebox_redraw((t_ebox *)x);
}

static void slider_mouseup(t_slider *x, t_object *patcherview, t_pt pt, long modifiers)
{
     ebox_parameter_end_changes((t_ebox *)x, 1);
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
        
        ebox_attrprocess_viabinbuf(x, d);
        ebox_ready((t_ebox *)x);
        return x;
    }
    
    return NULL;
}

static _FUNCTION_DEPRECTAED_ void slider_preset(t_slider *x, t_binbuf *b)
{
    binbuf_addv(b, (char *)"sf", &s_float, (float)ebox_parameter_getvalue((t_ebox *)x, 1));
}

extern "C" void setup_c0x2eslider(void)
{
    t_eclass *c = eclass_new("c.slider", (method)slider_new, (method)ebox_free, (short)sizeof(t_slider), 0L, A_GIMME, 0);
    if(c)
    {
        eclass_guiinit(c, 0);
        
        eclass_addmethod(c, (method) slider_paint,          "paint",            A_NULL, 0);
        eclass_addmethod(c, (method) slider_notify,         "notify",           A_NULL, 0);
        eclass_addmethod(c, (method) slider_getdrawparams,  "getdrawparams",    A_NULL, 0);
        eclass_addmethod(c, (method) slider_oksize,         "oksize",           A_NULL, 0);
        
        eclass_addmethod(c, (method) slider_set,            "set",              A_FLOAT,0);
        eclass_addmethod(c, (method) slider_float,          "float",            A_FLOAT,0);
        eclass_addmethod(c, (method) slider_bang,           "bang",             A_NULL, 0);
        
        eclass_addmethod(c, (method) slider_mousedown,      "mousedown",        A_NULL, 0);
        eclass_addmethod(c, (method) slider_mousedrag,      "mousedrag",        A_NULL, 0);
        eclass_addmethod(c, (method) slider_mouseup,        "mouseup",          A_NULL, 0);
        
        eclass_addmethod(c, (method) slider_preset,          "preset",           A_NULL, 0);
        
        CLASS_ATTR_DEFAULT              (c, "size", 0, "15. 120.");
        
        CLASS_ATTR_LONG                 (c, "mode", 0, t_slider, f_mode);
        CLASS_ATTR_LABEL                (c, "mode", 0, "Relative Mode");
        CLASS_ATTR_ORDER                (c, "mode", 0, "1");
        CLASS_ATTR_FILTER_CLIP          (c, "mode", 0, 1);
        CLASS_ATTR_DEFAULT              (c, "mode", 0, "0");
        CLASS_ATTR_SAVE                 (c, "mode", 1);
        CLASS_ATTR_STYLE                (c, "mode", 0, "onoff");
        
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
        /*
        CLASS_ATTR_SYMBOL               (c, "name", 0, t_slider, f_dummy);
        CLASS_ATTR_LABEL                (c, "name", 0, "Parameter Name");
        CLASS_ATTR_ORDER                (c, "name", 0, "6");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "name", 0, "");
        CLASS_ATTR_STYLE                (c, "name", 0, "entry");
        
        CLASS_ATTR_SYMBOL               (c, "label", 0, t_slider, f_dummy);
        CLASS_ATTR_LABEL                (c, "label", 0, "Parameter Label");
        CLASS_ATTR_ORDER                (c, "label", 0, "7");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "label", 0, "");
        CLASS_ATTR_STYLE                (c, "label", 0, "entry");
        
        CLASS_ATTR_FLOAT                (c, "min", 0, t_slider, f_dummy);
        CLASS_ATTR_LABEL                (c, "min", 0, "Minimum Parameter Value");
        CLASS_ATTR_ORDER                (c, "min", 0, "1");
        CLASS_ATTR_DEFAULT              (c, "min", 0, "0.");
        CLASS_ATTR_SAVE                 (c, "min", 1);
        CLASS_ATTR_STYLE                (c, "min", 0, "number");
        
        CLASS_ATTR_FLOAT                (c, "max", 0, t_slider, f_dummy);
        CLASS_ATTR_LABEL                (c, "max", 0, "Maximum Parameter Value");
        CLASS_ATTR_ORDER                (c, "max", 0, "2");
        CLASS_ATTR_DEFAULT              (c, "max", 0, "1.");
        CLASS_ATTR_SAVE                 (c, "max", 1);
        CLASS_ATTR_STYLE                (c, "max", 0, "number");
        */
        eclass_register(CLASS_BOX, c);
        slider_class = c;
    }
}





