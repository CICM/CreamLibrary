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

typedef struct _gain
{
	t_edspbox   j_box;
    t_outlet*   f_out;
    float       f_value_ref;
    float       f_value_last;
    
    float       f_amp;
    float       f_amp_old;
    float       f_amp_step;
    float       f_ramp;
    float       f_ramp_sample;
    int         f_counter;
    float       f_sample_rate;
    
    t_rgba		f_color_background;
	t_rgba		f_color_border;
	t_rgba		f_color_knob;
    char        f_modifier;
    char        f_direction;
} t_gain;

static t_eclass *gain_class;

static void gain_output(t_gain *x)
{
    t_pd* send = ebox_getsender((t_ebox *) x);
    const float val = ebox_parameter_getvalue((t_ebox *)x, 1);
    outlet_float(x->f_out, val);
    if(send)
    {
        pd_float(send,  val);
    }
}

static void gain_set(t_gain *x, float f)
{
    ebox_parameter_setvalue((t_ebox *)x, 1, f, 0);
    x->f_amp_old = x->f_amp;
    x->f_amp = powf(10., ebox_parameter_getvalue((t_ebox *)x, 1) * 0.05);
    x->f_amp_step = (float)(x->f_amp - x->f_amp_old) / (float)x->f_ramp_sample;
    x->f_counter  = 0;
    
    ebox_invalidate_layer((t_ebox *)x, cream_sym_knob_layer);
    ebox_redraw((t_ebox *)x);
}

static void gain_float(t_gain *x, float f)
{
    ebox_parameter_setvalue((t_ebox *)x, 1, f, 1);
    x->f_amp_old = x->f_amp;
    x->f_amp = powf(10., ebox_parameter_getvalue((t_ebox *)x, 1) * 0.05);
    x->f_amp_step = (float)(x->f_amp - x->f_amp_old) / (float)x->f_ramp_sample;
    x->f_counter  = 0;
    
    gain_output(x);
    ebox_invalidate_layer((t_ebox *)x, cream_sym_knob_layer);
    ebox_redraw((t_ebox *)x);
}

static void gain_linear(t_gain *x, float f)
{
    ebox_parameter_setvalue((t_ebox *)x, 1, 20.f * log10f(pd_clip(f, 0.00001, 8.)), 1);
    x->f_amp_old = x->f_amp;
    x->f_amp = powf(10., ebox_parameter_getvalue((t_ebox *)x, 1) * 0.05);
    x->f_amp_step = (float)(x->f_amp - x->f_amp_old) / (float)x->f_ramp_sample;
    x->f_counter  = 0;
    
    gain_output(x);
    ebox_invalidate_layer((t_ebox *)x, cream_sym_knob_layer);
    ebox_redraw((t_ebox *)x);
}

static void gain_bang(t_gain *x)
{
    ebox_parameter_notify_changes((t_ebox *)x, 1);
    gain_output(x);
}

static void gain_perform(t_gain *x, t_object *d, t_sample **ins, long ni, t_sample **outs, long no, long sf, long f,void *up)
{
    int i = 0, count    = x->f_counter;
    const int limit     = x->f_ramp_sample;
    float const step    = x->f_amp_step, result = x->f_amp;
    float old = x->f_amp_old;
    if(old != result)
    {
        for(; i < sf; i++)
        {
            outs[0][i] = ins[0][i] * old;
            old += step;
            if(count++ >= limit)
            {
                old   = result;
                count = 0;
                break;
            }
        }
    }
    for(; i < sf; i++)
    {
        outs[0][i]   = ins[0][i] * result;
    }
    x->f_amp_old = old;
    x->f_counter = count;
}

static void gain_dsp(t_gain *x, t_object *dsp, short *count, double samplerate, long maxvectorsize, long flags)
{
    x->f_sample_rate = pd_clip_min((float)samplerate, 1);
    x->f_ramp_sample = pd_clip_min(x->f_ramp * x->f_sample_rate / 1000.f, 1.f);
    object_method(dsp, gensym("dsp_add"), x, (method)gain_perform, 0, NULL);
}

static t_pd_err gain_notify(t_gain *x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
	if(msg == cream_sym_attr_modified)
	{
		if(s == cream_sym_bgcolor || s == cream_sym_bdcolor || s == cream_sym_kncolor)
		{
            ebox_invalidate_layer((t_ebox *)x, cream_sym_knob_layer);
			ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
		}
	}
    else if(msg == cream_sym_value_changed)
    {
        x->f_amp_old = x->f_amp;
        x->f_amp = powf(10., ebox_parameter_getvalue((t_ebox *)x, 1) * 0.05);
        x->f_amp_step = (float)(x->f_amp - x->f_amp_old) / (float)x->f_ramp_sample;
        x->f_counter  = 0;
        gain_output(x);
        ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
        ebox_redraw((t_ebox *)x);
    }
    return 0;
}

static void gain_getdrawparams(t_gain *x, t_object *patcherview, t_edrawparams *params)
{
    params->d_borderthickness   = 2;
    params->d_cornersize        = 2;
    params->d_bordercolor       = x->f_color_border;
    params->d_boxfillcolor      = x->f_color_background;
}

static void gain_oksize(t_gain *x, t_rect *newrect)
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

static void draw_background(t_gain *x, t_object *view, t_rect *rect)
{
	t_elayer *g = ebox_start_layer((t_ebox *)x, cream_sym_background_layer, rect->width, rect->height);
	if(g)
	{
        egraphics_set_line_width(g, 2.f);
        egraphics_set_color_rgba(g, &x->f_color_border);
        if(x->f_direction)
        {
            egraphics_line(g, (1.f - 0.16666666f) * rect->width, 0.f, (1.f - 0.16666666f) * rect->width, rect->height);
            egraphics_stroke(g);
            egraphics_line(g, (1.f - 0.35185185f) * rect->width, rect->height * 0.65f, (1.f - 0.35185185f) * rect->width, rect->height);
            egraphics_stroke(g);
            egraphics_line(g, (1.f - 0.537037037f) * rect->width, rect->height * 0.65f, (1.f - 0.537037037f) * rect->width, rect->height);
            egraphics_stroke(g);
            egraphics_line(g, (1.f - 0.722222222f) * rect->width, rect->height * 0.65f, (1.f - 0.722222222f) * rect->width, rect->height);
            egraphics_stroke(g);
            egraphics_line(g, (1.f -0.907407407f) * rect->width, rect->height * 0.65f, (1.f -0.907407407f) * rect->width, rect->height);
            egraphics_stroke(g);
        }
        else
        {
            egraphics_line(g, 0.f, 0.16666666f * rect->height, rect->width, 0.16666666f * rect->height);
            egraphics_stroke(g);
            egraphics_line(g, rect->width * 0.65f, 0.35185185f * rect->height, rect->width, 0.35185185f * rect->height);
            egraphics_stroke(g);
            egraphics_line(g, rect->width * 0.65f, 0.537037037f * rect->height, rect->width, 0.537037037f * rect->height);
            egraphics_stroke(g);
            egraphics_line(g, rect->width * 0.65f, 0.722222222f * rect->height, rect->width, 0.722222222f * rect->height);
            egraphics_stroke(g);
            egraphics_line(g, rect->width * 0.65f, 0.907407407f * rect->height, rect->width, 0.907407407f * rect->height);
            egraphics_stroke(g);
        }
        ebox_end_layer((t_ebox*)x, cream_sym_background_layer);
	}
	ebox_paint_layer((t_ebox *)x, cream_sym_background_layer, 0., 0.);
}

static void draw_knob(t_gain *x, t_object *view, t_rect *rect)
{
	t_elayer *g = ebox_start_layer((t_ebox *)x, cream_sym_knob_layer, rect->width, rect->height);
	if (g)
	{
        egraphics_set_line_width(g, 4.f);
        egraphics_set_color_rgba(g, &x->f_color_knob);
        if(x->f_direction)
        {
            const float val = (ebox_parameter_getvalue((t_ebox *)x, 1) + 90.f) / 108.f * rect->width;
            egraphics_line_fast(g, val, 0.f, val, rect->height);
        }
        else
        {
            const float val = (1.f - (ebox_parameter_getvalue((t_ebox *)x, 1) + 90.f) / 108.f) * rect->height;
            egraphics_line_fast(g, 0.f, val, rect->width, val);
        }
        ebox_end_layer((t_ebox*)x, cream_sym_knob_layer);
	}
	ebox_paint_layer((t_ebox *)x, cream_sym_knob_layer, 0., 0.);
}

static void gain_paint(t_gain *x, t_object *view)
{
    t_rect rect;
    ebox_get_rect_for_view((t_ebox *)x, &rect);
    draw_background(x, view, &rect);
    draw_knob(x, view, &rect);
}

static void gain_mousedown(t_gain *x, t_object *patcherview, t_pt pt, long modifiers)
{
    t_rect rect;
    ebox_get_rect_for_view((t_ebox *)x, &rect);
    ebox_parameter_begin_changes((t_ebox *)x, 1);
    if(modifiers == EMOD_SHIFT)
    {
        x->f_modifier = EMOD_SHIFT;
        x->f_value_last = ebox_parameter_getvalue((t_ebox *)x, 1);
        if(x->f_direction)
        {
            x->f_value_ref = pd_clip(pt.x / rect.width * 108.f - 90.f, -90.f, 18.f);
        }
        else
        {
            x->f_value_ref = pd_clip((rect.height - pt.y) / rect.height * 108.f - 90.f, -90.f, 18.f);
        }
    }
    else if(modifiers == EMOD_CTRL || modifiers == EMOD_CMD)
    {
        x->f_modifier = EMOD_CTRL;
    }
    else
    {
        x->f_modifier = EMOD_NONE;
        if(x->f_direction)
        {
            ebox_parameter_setvalue((t_ebox *)x, 1, pt.x / rect.width * 108.f - 90.f, 1);
        }
        else
        {
            ebox_parameter_setvalue((t_ebox *)x, 1, (rect.height - pt.y) / rect.height * 108.f - 90.f, 1);
        }
        
        x->f_amp_old = x->f_amp;
        x->f_amp = powf(10., ebox_parameter_getvalue((t_ebox *)x, 1) * 0.05);
        x->f_amp_step = (float)(x->f_amp - x->f_amp_old) / (float)x->f_ramp_sample;
        x->f_counter  = 0;
        
        gain_output(x);
        ebox_invalidate_layer((t_ebox *)x, cream_sym_knob_layer);
        ebox_redraw((t_ebox *)x);
    }
}

static void gain_mousedrag(t_gain *x, t_object *patcherview, t_pt pt, long modifiers)
{
    t_rect rect;
    ebox_get_rect_for_view((t_ebox *)x, &rect);
    if(x->f_modifier == EMOD_SHIFT)
    {
        const float newval = x->f_direction ? pt.x / rect.width * 108.f - 90.f : (rect.height - pt.y) / rect.height * 108.f - 90.f;
        const float value = pd_clip(x->f_value_last + newval - x->f_value_ref, -90.f, 18.);
        ebox_parameter_setvalue((t_ebox *)x, 1, value, 1);
        if(value == -90.f || value == 18.f)
        {
            x->f_value_last = value;
            x->f_value_ref  = newval;
        }
        
        x->f_amp_old = x->f_amp;
        x->f_amp = powf(10., ebox_parameter_getvalue((t_ebox *)x, 1) * 0.05);
        x->f_amp_step = (float)(x->f_amp - x->f_amp_old) / (float)x->f_ramp_sample;
        x->f_counter  = 0;
        
        gain_output(x);
        ebox_invalidate_layer((t_ebox *)x, cream_sym_knob_layer);
        ebox_redraw((t_ebox *)x);
    }
    else if(x->f_modifier == EMOD_NONE)
    {
        if(x->f_direction)
        {
            ebox_parameter_setvalue((t_ebox *)x, 1, pt.x / rect.width * 108.f - 90.f, 1);
        }
        else
        {
            ebox_parameter_setvalue((t_ebox *)x, 1, (rect.height - pt.y) / rect.height * 108.f - 90.f, 1);
        }
        
        x->f_amp_old = x->f_amp;
        x->f_amp = powf(10., ebox_parameter_getvalue((t_ebox *)x, 1) * 0.05);
        x->f_amp_step = (float)(x->f_amp - x->f_amp_old) / (float)x->f_ramp_sample;
        x->f_counter  = 0;
        
        gain_output(x);
        ebox_invalidate_layer((t_ebox *)x, cream_sym_knob_layer);
        ebox_redraw((t_ebox *)x);
    }
}

static void gain_mouseup(t_gain *x, t_object *patcherview, t_pt pt, long modifiers)
{
    ebox_parameter_end_changes((t_ebox *)x, 1);
}

static void gain_dblclick(t_gain *x, t_object *patcherview, t_pt pt, long modifiers)
{
    if(x->f_modifier == EMOD_CTRL)
    {
        ebox_parameter_setvalue((t_ebox *)x, 1, 0.f, 1);
        gain_output(x);
        ebox_invalidate_layer((t_ebox *)x, cream_sym_knob_layer);
        ebox_redraw((t_ebox *)x);
    }
}

static t_pd_err gain_ramp_set(t_gain *x, t_object *attr, int argc, t_atom *argv)
{
    if(argc && argv && atom_gettype(argv) == A_FLOAT)
    {
        x->f_ramp = atom_getfloat(argv);
        x->f_ramp_sample = pd_clip_min(x->f_ramp * x->f_sample_rate / 1000., 1);
    }
    return 0;
}

static void *gain_new(t_symbol *s, int argc, t_atom *argv)
{
    t_gain *x   = (t_gain *)eobj_new(gain_class);
    t_binbuf* d = binbuf_via_atoms(argc,argv);
    
    if(x && d)
    {
        ebox_new((t_ebox *)x, 0 | EBOX_GROWINDI);
        ebox_parameter_create((t_ebox *)x, 1);
        ebox_parameter_setminmax((t_ebox *)x, 1, -90.f, 18.f);
        eobj_dspsetup((t_ebox *)x, 1, 1);
        x->f_out = outlet_new((t_object *)x, &s_float);
        
        x->f_amp    = 1.;
        x->f_amp_old = 1.;
        x->f_amp_step = 0.;
        x->f_counter = 0;
        x->f_sample_rate = sys_getsr();
        
        ebox_attrprocess_viabinbuf(x, d);
        ebox_ready((t_ebox *)x);
        
        return (x);
    }
    
    return NULL;
}

extern "C"  void setup_c0x2egain_tilde(void)
{
    t_eclass *c;
    
    c = eclass_new("c.gain~", (method)gain_new, (method)ebox_free, (short)sizeof(t_gain), 0L, A_GIMME, 0);
    
    eclass_dspinit(c);
    eclass_guiinit(c, 0);
    
    
    eclass_addmethod(c, (method) gain_dsp,             "dsp",              A_NULL, 0);
    
    eclass_addmethod(c, (method) gain_paint,           "paint",            A_NULL, 0);
    eclass_addmethod(c, (method) gain_notify,          "notify",           A_NULL, 0);
    eclass_addmethod(c, (method) gain_getdrawparams,   "getdrawparams",    A_NULL, 0);
    eclass_addmethod(c, (method) gain_oksize,          "oksize",           A_NULL, 0);
    eclass_addmethod(c, (method) gain_set,             "set",              A_FLOAT,0);
    eclass_addmethod(c, (method) gain_float,           "float",            A_FLOAT,0);
    eclass_addmethod(c, (method) gain_linear,          "linear",           A_FLOAT,0);
    eclass_addmethod(c, (method) gain_bang,            "bang",             A_NULL, 0);
    
    eclass_addmethod(c, (method) gain_mousedown,       "mousedown",        A_NULL, 0);
    eclass_addmethod(c, (method) gain_mousedrag,       "mousedrag",        A_NULL, 0);
    eclass_addmethod(c, (method) gain_mouseup,         "mouseup",          A_NULL, 0);
    eclass_addmethod(c, (method) gain_dblclick,        "dblclick",         A_NULL, 0);
    
    CLASS_ATTR_DEFAULT              (c, "size", 0, "13 85");
    
    CLASS_ATTR_FLOAT                (c, "ramp", 0, t_gain, f_ramp);
    CLASS_ATTR_LABEL                (c, "ramp", 0, "Ramp Time (ms)");
    CLASS_ATTR_ACCESSORS			(c, "ramp", NULL, gain_ramp_set);
    CLASS_ATTR_ORDER                (c, "ramp", 0, "1");
    CLASS_ATTR_FILTER_MIN           (c, "ramp", 0);
    CLASS_ATTR_DEFAULT              (c, "ramp", 0, "20");
    CLASS_ATTR_SAVE                 (c, "ramp", 1);
    CLASS_ATTR_STYLE                (c, "ramp", 0, "number");
    
    CLASS_ATTR_RGBA                 (c, "bgcolor", 0, t_gain, f_color_background);
    CLASS_ATTR_LABEL                (c, "bgcolor", 0, "Background Color");
    CLASS_ATTR_ORDER                (c, "bgcolor", 0, "1");
    CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bgcolor", 0, "0.75 0.75 0.75 1.");
    CLASS_ATTR_STYLE                (c, "bgcolor", 0, "color");
    
    CLASS_ATTR_RGBA                 (c, "bdcolor", 0, t_gain, f_color_border);
    CLASS_ATTR_LABEL                (c, "bdcolor", 0, "Border Color");
    CLASS_ATTR_ORDER                (c, "bdcolor", 0, "2");
    CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bdcolor", 0, "0.5 0.5 0.5 1.");
    CLASS_ATTR_STYLE                (c, "bdcolor", 0, "color");
    
    CLASS_ATTR_RGBA                 (c, "kncolor", 0, t_gain, f_color_knob);
    CLASS_ATTR_LABEL                (c, "kncolor", 0, "Knob Color");
    CLASS_ATTR_ORDER                (c, "kncolor", 0, "3");
    CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "kncolor", 0, "0.5 0.5 0.5 1.");
    CLASS_ATTR_STYLE                (c, "kncolor", 0, "color");
    
    eclass_register(CLASS_BOX, c);
    gain_class = c;
}


