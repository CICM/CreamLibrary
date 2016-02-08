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
<<<<<<< HEAD
    t_edspbox   j_box;

    t_outlet*   f_out;

    float       f_value;
=======
	t_edspbox   j_box;
    t_outlet*   f_out;
>>>>>>> v1
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
<<<<<<< HEAD
    t_rgba		f_color_border;
    t_rgba		f_color_knob;
    long        f_mode;
    char        f_direction;

} t_gain;

t_eclass *gain_class;

void *gain_new(t_symbol *s, int argc, t_atom *argv);
void gain_free(t_gain *x);
void gain_assist(t_gain *x, void *b, long m, long a, char *s);

void gain_float(t_gain *x, float f);
void gain_set(t_gain *x, float f);
void gain_linear(t_gain *x, float f);
void gain_bang(t_gain *x);
void gain_output(t_gain *x);

void gain_dsp(t_gain *x, t_object *dsp, short *count, double samplerate, long maxvectorsize, long flags);
void gain_perform(t_gain *x, t_object *d, t_sample **ins, long ni, t_sample **outs, long no, long sf, long f,void *up);

t_pd_err gain_notify(t_gain *x, t_symbol *s, t_symbol *msg, void *sender, void *data);
t_pd_err gain_ramp_set(t_gain *x, t_object *attr, int argc, t_atom *argv);

void gain_preset(t_gain *x, t_binbuf *b);

void gain_getdrawparams(t_gain *x, t_object *patcherview, t_edrawparams *params);
void gain_oksize(t_gain *x, t_rect *newrect);

void gain_paint(t_gain *x, t_object *view);
void draw_background(t_gain *x, t_object *view, t_rect *rect);
void draw_knob(t_gain *x,  t_object *view, t_rect *rect);

void gain_mousedown(t_gain *x, t_object *patcherview, t_pt pt, long modifiers);
void gain_mousedrag(t_gain *x, t_object *patcherview, t_pt pt, long modifiers);
void gain_dblclick(t_gain *x, t_object *patcherview, t_pt pt, long modifiers);

extern "C"  void setup_c0x2egain_tilde(void)
{
    t_eclass *c;

    c = eclass_new("c.gain~", (method)gain_new, (method)gain_free, (short)sizeof(t_gain), 0L, A_GIMME, 0);

    eclass_dspinit(c);
    eclass_guiinit(c, 0);


    eclass_addmethod(c, (method) gain_dsp,             "dsp",              A_NULL, 0);
    eclass_addmethod(c, (method) gain_assist,          "assist",           A_NULL, 0);
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
    eclass_addmethod(c, (method) gain_dblclick,        "dblclick",         A_NULL, 0);
    eclass_addmethod(c, (method) gain_preset,          "preset",           A_NULL, 0);

    CLASS_ATTR_INVISIBLE            (c, "fontname", 1);
    CLASS_ATTR_INVISIBLE            (c, "fontweight", 1);
    CLASS_ATTR_INVISIBLE            (c, "fontslant", 1);
    CLASS_ATTR_INVISIBLE            (c, "fontsize", 1);
    CLASS_ATTR_DEFAULT              (c, "size", 0, "20. 160.");

    CLASS_ATTR_LONG                 (c, "mode", 0, t_gain, f_mode);
    CLASS_ATTR_LABEL                (c, "mode", 0, "Relative Mode");
    CLASS_ATTR_ORDER                (c, "mode", 0, "1");
    CLASS_ATTR_FILTER_CLIP          (c, "mode", 0, 1);
    CLASS_ATTR_DEFAULT              (c, "mode", 0, "0");
    CLASS_ATTR_SAVE                 (c, "mode", 1);
    CLASS_ATTR_STYLE                (c, "mode", 0, "onoff");

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

void *gain_new(t_symbol *s, int argc, t_atom *argv)
{
    t_gain *x =  NULL;
    t_binbuf* d;
    long flags;
    if (!(d = binbuf_via_atoms(argc,argv)))
        return NULL;

    x = (t_gain *)eobj_new(gain_class);
    flags = 0
        | EBOX_GROWINDI
        ;
    ebox_new((t_ebox *)x, flags);
    eobj_dspsetup((t_ebox *)x, 1, 1);
    x->f_out = outlet_new((t_object *)x, &s_float);

    x->f_value  = 0.;
    x->f_amp    = 1.;
    x->f_amp_old = 1.;
    x->f_amp_step = 0.;
    x->f_counter = 0;
    x->f_sample_rate = sys_getsr();

    ebox_attrprocess_viabinbuf(x, d);
    ebox_ready((t_ebox *)x);
    return (x);
}

void gain_getdrawparams(t_gain *x, t_object *patcherview, t_edrawparams *params)
{
    params->d_borderthickness   = 2;
    params->d_cornersize        = 2;
    params->d_bordercolor       = x->f_color_border;
    params->d_boxfillcolor      = x->f_color_background;
}
=======
	t_rgba		f_color_border;
	t_rgba		f_color_knob;
    char        f_modifier;
    char        f_direction;
} t_gain;

static t_eclass *gain_class;
>>>>>>> v1

static void gain_output(t_gain *x)
{
<<<<<<< HEAD
    newrect->width = pd_clip_min(newrect->width, 8.);
    newrect->height = pd_clip_min(newrect->height, 8.);

    if(newrect->width > newrect->height)
        x->f_direction = 1;
    else
        x->f_direction = 0;

    if(x->f_direction)
=======
    t_pd* send = ebox_getsender((t_ebox *) x);
    const float val = ebox_parameter_getvalue((t_ebox *)x, 1);
    outlet_float(x->f_out, val);
    if(send)
>>>>>>> v1
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
    
    ebox_invalidate_layer((t_ebox *)x, NULL, cream_sym_knob_layer);
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
    ebox_invalidate_layer((t_ebox *)x, NULL, cream_sym_knob_layer);
    ebox_redraw((t_ebox *)x);
}

static void gain_linear(t_gain *x, float f)
{
<<<<<<< HEAD
    f = pd_clip_minmax(f, 0.00001, 8.);
    f = (20.f * log10f(f));

    if(f < -90)
    {
        f  = -90.;
    }
    gain_float(x, f);
}

void gain_bang(t_gain *x)
{
    ebox_invalidate_layer((t_ebox *)x, cream_sym_knob_layer);
    ebox_redraw((t_ebox *)x);
=======
    ebox_parameter_setvalue((t_ebox *)x, 1, 20.f * log10f(pd_clip(f, 0.00001, 8.)), 1);
    x->f_amp_old = x->f_amp;
    x->f_amp = powf(10., ebox_parameter_getvalue((t_ebox *)x, 1) * 0.05);
    x->f_amp_step = (float)(x->f_amp - x->f_amp_old) / (float)x->f_ramp_sample;
    x->f_counter  = 0;
    
>>>>>>> v1
    gain_output(x);
    ebox_invalidate_layer((t_ebox *)x, NULL, cream_sym_knob_layer);
    ebox_redraw((t_ebox *)x);
}

static void gain_bang(t_gain *x)
{
<<<<<<< HEAD
    ebox_free((t_ebox *)x);
=======
    ebox_parameter_notify_changes((t_ebox *)x, 1);
    gain_output(x);
>>>>>>> v1
}

static void gain_perform(t_gain *x, t_object *d, t_sample **ins, long ni, t_sample **outs, long no, long sf, long f,void *up)
{
<<<<<<< HEAD
    ;
=======
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
>>>>>>> v1
}

static void gain_dsp(t_gain *x, t_object *dsp, short *count, double samplerate, long maxvectorsize, long flags)
{
    x->f_sample_rate = pd_clip_min((float)samplerate, 1);
    x->f_ramp_sample = pd_clip_min(x->f_ramp * x->f_sample_rate / 1000.f, 1.f);
<<<<<<< HEAD
    object_method(dsp, gensym("dsp_add"), x, (method)gain_perform, 0, NULL);
}

void gain_perform(t_gain *x, t_object *d, t_sample **ins, long ni, t_sample **outs, long no, long sf, long f,void *up)
{
    int i;
    for(i = 0; i < sf; i++)
    {

        outs[0][i] = ins[0][i] * x->f_amp_old;
        x->f_amp_old += x->f_amp_step;
        if(x->f_counter++ >= x->f_ramp_sample)
        {
            x->f_amp_step = 0.;
            x->f_amp_old  = x->f_amp;
            x->f_counter  = 0;
        }
    }
=======
    mess4((t_pd *)dsp, gensym("dsp_add"), x, (void *)gain_perform, 0, NULL);
>>>>>>> v1
}

static t_pd_err gain_notify(t_gain *x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
<<<<<<< HEAD
    if (msg == cream_sym_attr_modified)
    {
        if(s == cream_sym_bgcolor || s == cream_sym_bdcolor || s == gensym("kncolor"))
        {
            ebox_invalidate_layer((t_ebox *)x, cream_sym_knob_layer);
            ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
        }
=======
	if(msg == cream_sym_attr_modified)
	{
		if(s == cream_sym_bgcolor || s == cream_sym_bdcolor || s == cream_sym_kncolor)
		{
            ebox_invalidate_layer((t_ebox *)x, NULL, cream_sym_knob_layer);
			ebox_invalidate_layer((t_ebox *)x, NULL, cream_sym_background_layer);
		}
	}
    else if(msg == cream_sym_value_changed)
    {
        x->f_amp_old = x->f_amp;
        x->f_amp = powf(10., ebox_parameter_getvalue((t_ebox *)x, 1) * 0.05);
        x->f_amp_step = (float)(x->f_amp - x->f_amp_old) / (float)x->f_ramp_sample;
        x->f_counter  = 0;
        gain_output(x);
        ebox_invalidate_layer((t_ebox *)x, NULL, cream_sym_background_layer);
>>>>>>> v1
        ebox_redraw((t_ebox *)x);
    }
    return 0;
}

static void gain_getdrawparams(t_gain *x, t_object *view, t_edrawparams *params)
{
<<<<<<< HEAD
    t_rect rect;
    ebox_get_rect_for_view((t_ebox *)x, &rect);
    draw_background(x, view, &rect);
    draw_knob(x, view, &rect);
=======
    params->d_borderthickness   = 2;
    params->d_cornersize        = 2;
    params->d_bordercolor       = x->f_color_border;
    params->d_boxfillcolor      = x->f_color_background;
>>>>>>> v1
}

static void gain_oksize(t_gain *x, t_rect *newrect)
{
<<<<<<< HEAD
    float ratio;
    t_elayer *g = ebox_start_layer((t_ebox *)x, cream_sym_background_layer, rect->width, rect->height);

    if (g)
    {
        if(x->f_direction)
        {
            ratio = 90.f / 108.f;
            egraphics_set_line_width(g, pd_clip_minmax(rect->height * 0.1f, 2.f, 4.f));

            egraphics_set_color_rgba(g, &x->f_color_border);
            egraphics_line_fast(g, rect->width * ratio, 0, rect->width * ratio, rect->height);

            egraphics_set_color_rgba(g, &x->f_color_background);
            egraphics_line_fast(g, rect->width * ratio, pd_clip_min(rect->height * 0.1f, 2.f), rect->width * ratio, rect->height - pd_clip_min(rect->height * 0.1f, 2));

            egraphics_set_color_rgba(g, &x->f_color_border);
            egraphics_line_fast(g, pd_clip_min(rect->height * 0.1f, 2), rect->height * 0.5f, rect->width-pd_clip_min(rect->height * 0.1f, 2), rect->height * 0.5f);
        }
        else
        {
            ratio = 1.f - 90.f / 108.f;
            egraphics_set_line_width(g, pd_clip_minmax(rect->width * 0.1f, 2.f, 4.f));

            egraphics_set_color_rgba(g, &x->f_color_border);
            egraphics_line_fast(g, 0, rect->height * ratio, rect->width, rect->height * ratio);

            egraphics_set_color_rgba(g, &x->f_color_background);
            egraphics_line_fast(g, pd_clip_min(rect->width * 0.1f, 2), rect->height * ratio, rect->width - pd_clip_min(rect->width * 0.1f, 2), rect->height * ratio);

            egraphics_set_color_rgba(g, &x->f_color_border);
            egraphics_line_fast(g, rect->width * 0.5f, pd_clip_min(rect->width * 0.1f, 2.f), rect->width * 0.5f, rect->height -pd_clip_min(rect->width * 0.1f, 2.f));

        }
        ebox_end_layer((t_ebox*)x, cream_sym_background_layer);
    }
    ebox_paint_layer((t_ebox *)x, cream_sym_background_layer, 0., 0.);
=======
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
	t_elayer *g = ebox_start_layer((t_ebox *)x, view, cream_sym_background_layer, rect->width, rect->height);
	if(g)
	{
        elayer_set_line_width(g, 2.f);
        elayer_set_color_rgba(g, &x->f_color_border);
        if(x->f_direction)
        {
            elayer_line(g, (1.f - 0.16666666f) * rect->width, 0.f, (1.f - 0.16666666f) * rect->width, rect->height);
            elayer_stroke(g);
            elayer_line(g, (1.f - 0.35185185f) * rect->width, rect->height * 0.65f, (1.f - 0.35185185f) * rect->width, rect->height);
            elayer_stroke(g);
            elayer_line(g, (1.f - 0.537037037f) * rect->width, rect->height * 0.65f, (1.f - 0.537037037f) * rect->width, rect->height);
            elayer_stroke(g);
            elayer_line(g, (1.f - 0.722222222f) * rect->width, rect->height * 0.65f, (1.f - 0.722222222f) * rect->width, rect->height);
            elayer_stroke(g);
            elayer_line(g, (1.f -0.907407407f) * rect->width, rect->height * 0.65f, (1.f -0.907407407f) * rect->width, rect->height);
            elayer_stroke(g);
        }
        else
        {
            elayer_line(g, 0.f, 0.16666666f * rect->height, rect->width, 0.16666666f * rect->height);
            elayer_stroke(g);
            elayer_line(g, rect->width * 0.65f, 0.35185185f * rect->height, rect->width, 0.35185185f * rect->height);
            elayer_stroke(g);
            elayer_line(g, rect->width * 0.65f, 0.537037037f * rect->height, rect->width, 0.537037037f * rect->height);
            elayer_stroke(g);
            elayer_line(g, rect->width * 0.65f, 0.722222222f * rect->height, rect->width, 0.722222222f * rect->height);
            elayer_stroke(g);
            elayer_line(g, rect->width * 0.65f, 0.907407407f * rect->height, rect->width, 0.907407407f * rect->height);
            elayer_stroke(g);
        }
        ebox_end_layer((t_ebox*)x, view, cream_sym_background_layer);
	}
	ebox_paint_layer((t_ebox *)x, view, cream_sym_background_layer, 0., 0.);
>>>>>>> v1
}

static void draw_knob(t_gain *x, t_object *view, t_rect *rect)
{
<<<<<<< HEAD
    float ratio;
    t_elayer *g = ebox_start_layer((t_ebox *)x, cream_sym_knob_layer, rect->width, rect->height);

    if (g)
    {
        float value = (x->f_value + 90.) / 108.;
        egraphics_set_color_rgba(g, &x->f_color_knob);
=======
	t_elayer *g = ebox_start_layer((t_ebox *)x, view, cream_sym_knob_layer, rect->width, rect->height);
	if (g)
	{
        elayer_set_line_width(g, 4.f);
        elayer_set_color_rgba(g, &x->f_color_knob);
>>>>>>> v1
        if(x->f_direction)
        {
            const float val = (ebox_parameter_getvalue((t_ebox *)x, 1) + 90.f) / 108.f * rect->width;
            elayer_line_fast(g, val, 0.f, val, rect->height);
        }
        else
        {
            const float val = (1.f - (ebox_parameter_getvalue((t_ebox *)x, 1) + 90.f) / 108.f) * rect->height;
            elayer_line_fast(g, 0.f, val, rect->width, val);
        }
<<<<<<< HEAD
        ebox_end_layer((t_ebox*)x, cream_sym_knob_layer);
    }
    ebox_paint_layer((t_ebox *)x, cream_sym_knob_layer, 0., 0.);
=======
        ebox_end_layer((t_ebox*)x, view, cream_sym_knob_layer);
	}
	ebox_paint_layer((t_ebox *)x, view, cream_sym_knob_layer, 0., 0.);
>>>>>>> v1
}

static void gain_paint(t_gain *x, t_object *view)
{
    t_rect rect;
    ebox_getdrawbounds((t_ebox *)x, view,  &rect);
    draw_background(x, view, &rect);
    draw_knob(x, view, &rect);
}

static void gain_mousedown(t_gain *x, t_object *view, t_pt pt, long modifiers)
{
    t_rect rect;
    ebox_getdrawbounds((t_ebox *)x, view,  &rect);
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
        ebox_invalidate_layer((t_ebox *)x, NULL, cream_sym_knob_layer);
        ebox_redraw((t_ebox *)x);
    }
}

static void gain_mousedrag(t_gain *x, t_object *view, t_pt pt, long modifiers)
{
    t_rect rect;
    ebox_getdrawbounds((t_ebox *)x, view,  &rect);
    if(x->f_modifier == EMOD_SHIFT)
    {
<<<<<<< HEAD
        if(x->f_direction)
        {
            newvalue = pt.x / x->j_box.b_rect.width * 108. - 90.;
        }
        else
        {
            newvalue = (x->j_box.b_rect.height - pt.y) / x->j_box.b_rect.height * 108. - 90.;
        }
        value = pd_clip_minmax(x->f_value_last + newvalue - x->f_value_ref, -90., 18.);

        if(value == -90. || value == 18.)
=======
        const float newval = x->f_direction ? pt.x / rect.width * 108.f - 90.f : (rect.height - pt.y) / rect.height * 108.f - 90.f;
        const float value = pd_clip(x->f_value_last + newval - x->f_value_ref, -90.f, 18.);
        ebox_parameter_setvalue((t_ebox *)x, 1, value, 1);
        if(value == -90.f || value == 18.f)
>>>>>>> v1
        {
            x->f_value_last = value;
            x->f_value_ref  = newval;
        }
        
        x->f_amp_old = x->f_amp;
        x->f_amp = powf(10., ebox_parameter_getvalue((t_ebox *)x, 1) * 0.05);
        x->f_amp_step = (float)(x->f_amp - x->f_amp_old) / (float)x->f_ramp_sample;
        x->f_counter  = 0;
        
        gain_output(x);
        ebox_invalidate_layer((t_ebox *)x, NULL, cream_sym_knob_layer);
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
        ebox_invalidate_layer((t_ebox *)x, NULL, cream_sym_knob_layer);
        ebox_redraw((t_ebox *)x);
    }
<<<<<<< HEAD

    gain_float(x, value);
=======
>>>>>>> v1
}

static void gain_mouseup(t_gain *x, t_object *view, t_pt pt, long modifiers)
{
    ebox_parameter_end_changes((t_ebox *)x, 1);
}

static void gain_dblclick(t_gain *x, t_object *view, t_pt pt, long modifiers)
{
    if(x->f_modifier == EMOD_CTRL)
    {
        ebox_parameter_setvalue((t_ebox *)x, 1, 0.f, 1);
        gain_output(x);
        ebox_invalidate_layer((t_ebox *)x, NULL, cream_sym_knob_layer);
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
<<<<<<< HEAD

=======
>>>>>>> v1
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
        
        eobj_attr_read(x, d);
        ebox_ready((t_ebox *)x);
        
        return (x);
    }
    
    return NULL;
}

extern "C"  void setup_c0x2egain_tilde(void)
{
    t_eclass *c;
    
    c = eclass_new("c.gain~", (t_method)gain_new, (t_method)ebox_free, (short)sizeof(t_gain), 0L, A_GIMME, 0);
    
    eclass_dspinit(c);
    eclass_guiinit(c, 0);
    
    
    eclass_addmethod(c, (t_method) gain_dsp,             "dsp",              A_NULL, 0);
    
    eclass_addmethod(c, (t_method) gain_paint,           "paint",            A_NULL, 0);
    eclass_addmethod(c, (t_method) gain_notify,          "notify",           A_NULL, 0);
    eclass_addmethod(c, (t_method) gain_getdrawparams,   "getdrawparams",    A_NULL, 0);
    eclass_addmethod(c, (t_method) gain_oksize,          "oksize",           A_NULL, 0);
    eclass_addmethod(c, (t_method) gain_set,             "set",              A_FLOAT,0);
    eclass_addmethod(c, (t_method) gain_float,           "float",            A_FLOAT,0);
    eclass_addmethod(c, (t_method) gain_linear,          "linear",           A_FLOAT,0);
    eclass_addmethod(c, (t_method) gain_bang,            "bang",             A_NULL, 0);
    
    eclass_addmethod(c, (t_method) gain_mousedown,       "mousedown",        A_NULL, 0);
    eclass_addmethod(c, (t_method) gain_mousedrag,       "mousedrag",        A_NULL, 0);
    eclass_addmethod(c, (t_method) gain_mouseup,         "mouseup",          A_NULL, 0);
    eclass_addmethod(c, (t_method) gain_dblclick,        "dblclick",         A_NULL, 0);
    
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


