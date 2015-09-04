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

typedef struct _rslider
{
	t_ebox      j_box;
    t_outlet*   f_out_left;
    t_outlet*   f_out_right;
    char        f_listmode;
	t_rgba		f_color_background;
	t_rgba		f_color_border;
	t_rgba		f_color_knob;
    char        f_direction;
    char        f_highchange;
} t_rslider;

static t_eclass *rslider_class;

static void rslider_output(t_rslider *x)
{
    t_atom argv[2];
    t_pd* send = ebox_getsender((t_ebox *) x);
    const float lval = ebox_parameter_getvalue((t_ebox *)x, 1);
    const float hval = ebox_parameter_getvalue((t_ebox *)x, 2);
    atom_setfloat(argv, lval);
    atom_setfloat(argv+1, hval);
    outlet_list(x->f_out_left, &s_list, 2, argv);
    outlet_float(x->f_out_right, hval);
    if(send)
    {
        pd_list(send, &s_list, 2, argv);
    }
}

static void rslider_float(t_rslider *x, float f)
{
    ebox_parameter_setvalue((t_ebox *)x, 1, f, eobj_getproxy(x) + 1);
    rslider_output(x);
    ebox_invalidate_layer((t_ebox *)x, cream_sym_knob_layer);
    ebox_redraw((t_ebox *)x);
}


static void rslider_set(t_rslider *x, t_symbol* s, int argc, t_atom *argv)
{
    if(argc > 0 && atom_gettype(argv) == A_FLOAT)
    {
        ebox_parameter_setvalue((t_ebox *)x, 1, atom_getfloat(argv), 0);
    }
    if(argc > 1 && atom_gettype(argv+1) == A_FLOAT)
    {
        ebox_parameter_setvalue((t_ebox *)x, 1, atom_getfloat(argv+1), 0);
    }
    ebox_invalidate_layer((t_ebox *)x, cream_sym_knob_layer);
    ebox_redraw((t_ebox *)x);
}

static void rslider_bang(t_rslider *x)
{
    ebox_parameter_notify_changes((t_ebox *)x, 1);
    ebox_parameter_notify_changes((t_ebox *)x, 2);
    rslider_output(x);
}

static void rslider_list(t_rslider *x, t_symbol* s, int argc, t_atom *argv)
{
    if(argc > 0 && atom_gettype(argv) == A_FLOAT)
    {
        ebox_parameter_setvalue((t_ebox *)x, 1, atom_getfloat(argv), 1);
    }
    if(argc > 1 && atom_gettype(argv+1) == A_FLOAT)
    {
        ebox_parameter_setvalue((t_ebox *)x, 1, atom_getfloat(argv+1), 1);
    }
    rslider_output(x);
    ebox_invalidate_layer((t_ebox *)x, cream_sym_knob_layer);
    ebox_redraw((t_ebox *)x);
}

static void rslider_getdrawparams(t_rslider *x, t_object *patcherview, t_edrawparams *params)
{
    params->d_borderthickness   = 2;
    params->d_cornersize        = 2;
    params->d_bordercolor       = x->f_color_border;
    params->d_boxfillcolor      = x->f_color_background;
}

static void rslider_oksize(t_rslider *x, t_rect *newrect)
{
    newrect->width = pd_clip_min(newrect->width, 16.);
    newrect->height = pd_clip_min(newrect->height, 16.);
    x->f_direction = (char)(newrect->width > newrect->height);
    if(x->f_direction)
    {
        newrect->width = pd_clip_min(newrect->width, 50.);
    }
    else
    {
        newrect->height = pd_clip_min(newrect->height, 50.);
    }
}

static t_pd_err rslider_notify(t_rslider *x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
    if(msg == cream_sym_attr_modified)
    {
        if(s == cream_sym_bgcolor ||
           s == cream_sym_bdcolor ||
           s == cream_sym_kncolor)
        {
            ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
        }
        else if(s == ebox_parameter_getbind((t_ebox *)x, 1))
        {
            ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
            ebox_parameter_setmin((t_ebox *)x, 2, ebox_parameter_getmin((t_ebox *)x, 1));
            ebox_parameter_setmax((t_ebox *)x, 2, ebox_parameter_getmax((t_ebox *)x, 1));
            ebox_parameter_setnstep((t_ebox *)x, 2, ebox_parameter_getnstep((t_ebox *)x, 1));
        }
    }
    else if(msg == cream_sym_value_changed)
    {
        rslider_output(x);
        ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
        ebox_redraw((t_ebox *)x);
    }
    return 0;
}

static void draw_background(t_rslider *x, t_object *view, t_rect *rect)
{
    t_elayer *g = ebox_start_layer((t_ebox *)x, cream_sym_background_layer, rect->width, rect->height);
    if(g)
    {
        egraphics_set_color_rgba(g, &x->f_color_border);
        egraphics_set_line_width(g, 2);
        if(x->f_direction)
        {
            egraphics_line(g, 2.f, 2.f, 2.f, rect->height - 2.f);
            egraphics_line(g, rect->width - 2.f, 2.f, rect->width - 2.f, rect->height - 2.f);
            egraphics_stroke(g);
        }
        else
        {
            egraphics_line(g, 2.f, 2.f, rect->width - 2.f, 2.f);
            egraphics_line(g, 2.f, rect->height - 2.f, rect->width - 2.f, rect->height - 2.f);
            egraphics_stroke(g);
        }
        ebox_end_layer((t_ebox*)x, cream_sym_background_layer);
    }
    ebox_paint_layer((t_ebox *)x, cream_sym_background_layer, 0., 0.);
}

static void draw_knob(t_rslider *x, t_object *view, t_rect *rect)
{
    t_rgba rectcolor;
    t_elayer *g = ebox_start_layer((t_ebox *)x, cream_sym_knob_layer, rect->width, rect->height);
    if(g)
    {
        const float lval    = ebox_parameter_getvalue_normalized((t_ebox *)x, 1);
        const float hval    = ebox_parameter_getvalue_normalized((t_ebox *)x, 2);
        const char inv      = ebox_parameter_isinverted((t_ebox *)x, 2);
        rgba_set(&rectcolor,
                 (x->f_color_background.red + x->f_color_knob.red) * 0.5,
                 (x->f_color_background.green + x->f_color_knob.green) * 0.5,
                 (x->f_color_background.blue + x->f_color_knob.blue) * 0.5, 1.);
        
        egraphics_set_line_width(g, 2);
        if(x->f_direction)
        {
            const float lpos = (inv ? (1.f - lval) : lval) * (rect->width - 4.f) + 2.f;
            const float hpos = (inv ? (1.f - hval) : hval) * (rect->width - 4.f) + 2.f;
            egraphics_set_color_rgba(g, &rectcolor);
            egraphics_rectangle(g, lpos, 2.f, hpos - lpos, rect->height - 4.f);
            egraphics_fill_preserve(g);
            egraphics_set_color_rgba(g, &x->f_color_knob);
            egraphics_stroke(g);
        }
        else
        {
            const float lpos = (inv ? lval : (1.f - lval)) * (rect->height - 4.f) + 2.f;
            const float hpos = (inv ? hval : (1.f - hval)) * (rect->height - 4.f) + 2.f;
            egraphics_set_color_rgba(g, &rectcolor);
            egraphics_rectangle(g, 2.f, lpos, rect->width - 4.f, hpos - lpos);
            egraphics_fill_preserve(g);
            egraphics_set_color_rgba(g, &x->f_color_knob);
            egraphics_stroke(g);
        }
        ebox_end_layer((t_ebox*)x, cream_sym_knob_layer);
    }
    ebox_paint_layer((t_ebox *)x, cream_sym_knob_layer, 0., 0.);
}

static void rslider_paint(t_rslider *x, t_object *view)
{
    t_rect rect;
    ebox_get_rect_for_view((t_ebox *)x, &rect);
    draw_background(x, view, &rect);
    draw_knob(x, view, &rect);
}

static float rslider_getvalue(char direction, t_rect const* rect, t_pt const* pt, float min, float max)
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

static void rslider_mousedown(t_rslider *x, t_object *patcherview, t_pt pt, long modifiers)
{
    t_rect rect;
    ebox_get_rect_for_view((t_ebox *)x, &rect);
    const float min   = ebox_parameter_getmin((t_ebox *)x, 1);
    const float max   = ebox_parameter_getmax((t_ebox *)x, 1);
    const float value = rslider_getvalue(x->f_direction, &rect, &pt, min, max);
    if(modifiers == EMOD_SHIFT)
    {
        const float lval = ebox_parameter_getvalue((t_ebox *)x, 1);
        const float hval = ebox_parameter_getvalue((t_ebox *)x, 2);
        if(fabs(lval - value) < fabs(hval - value))
        {
            ebox_parameter_begin_changes((t_ebox *)x, 1);
            ebox_parameter_setvalue((t_ebox *)x, 1, value, 1);
            x->f_highchange = 0;
        }
        else
        {
            ebox_parameter_begin_changes((t_ebox *)x, 2);
            ebox_parameter_setvalue((t_ebox *)x, 2, value, 1);
            x->f_highchange = 1;
        }
    }
    else
    {
        ebox_parameter_setvalue((t_ebox *)x, 1, value, 1);
        ebox_parameter_begin_changes((t_ebox *)x, 2);
        ebox_parameter_setvalue((t_ebox *)x, 2, value, 1);
        x->f_highchange = 1;
    }
    
    rslider_output(x);
    ebox_invalidate_layer((t_ebox *)x, cream_sym_knob_layer);
    ebox_redraw((t_ebox *)x);
}

static void rslider_mousedrag(t_rslider *x, t_object *patcherview, t_pt pt, long modifiers)
{
    t_rect rect;
    ebox_get_rect_for_view((t_ebox *)x, &rect);
    
    const float min   = ebox_parameter_getmin((t_ebox *)x, 1);
    const float max   = ebox_parameter_getmax((t_ebox *)x, 1);
    const float value = rslider_getvalue(x->f_direction, &rect, &pt, min, max);
    ebox_parameter_setvalue((t_ebox *)x, (int)(x->f_highchange) + 1, value, 1);
    
    rslider_output(x);
    ebox_invalidate_layer((t_ebox *)x, cream_sym_knob_layer);
    ebox_redraw((t_ebox *)x);
}

static void rslider_mouseup(t_rslider *x, t_object *patcherview, t_pt pt, long modifiers)
{
    ebox_parameter_end_changes((t_ebox *)x, (int)(x->f_highchange) + 1);
}

static void *rslider_new(t_symbol *s, int argc, t_atom *argv)
{
    t_rslider *x = (t_rslider *)eobj_new(rslider_class);
    t_binbuf  *d = binbuf_via_atoms(argc,argv);
    
    if(x && d)
    {
        ebox_new((t_ebox *)x, 0 | EBOX_GROWINDI);
        ebox_parameter_create((t_ebox *)x, 1);
        ebox_parameter_create((t_ebox *)x, 2);
        ebox_parameter_setflags((t_ebox *)x, 2, EPARAM_STATIC_MIN | EPARAM_STATIC_MAX |
                                EPARAM_STATIC_INVERTED | EPARAM_STATIC_NSTEPS),
        eobj_proxynew(x);
        eobj_proxynew(x);
        x->f_out_left = outlet_new((t_object *)x, &s_list);
        x->f_out_right = outlet_new((t_object *)x, &s_float);
        
        ebox_attrprocess_viabinbuf(x, d);
        ebox_ready((t_ebox *)x);
        
        return x;
    }
    
    return NULL;
}

extern "C" void setup_c0x2erslider(void)
{
	t_eclass *c = eclass_new("c.rslider", (method)rslider_new, (method)ebox_free, (short)sizeof(t_rslider), CLASS_NOINLET, A_GIMME, 0);
    
    if(c)
    {
        eclass_guiinit(c, 0);
        
        eclass_addmethod(c, (method) rslider_paint,             "paint",            A_NULL, 0);
        eclass_addmethod(c, (method) rslider_notify,            "notify",           A_NULL, 0);
        eclass_addmethod(c, (method) rslider_getdrawparams,     "getdrawparams",    A_NULL, 0);
        eclass_addmethod(c, (method) rslider_oksize,            "oksize",           A_NULL, 0);
        eclass_addmethod(c, (method) rslider_set,               "set",              A_GIMME,0);
        eclass_addmethod(c, (method) rslider_list,              "list",             A_GIMME,0);
        eclass_addmethod(c, (method) rslider_float,             "float",            A_FLOAT,0);
        eclass_addmethod(c, (method) rslider_bang,              "bang",             A_NULL, 0);
        
        eclass_addmethod(c, (method) rslider_mousedown,         "mousedown",        A_NULL, 0);
        eclass_addmethod(c, (method) rslider_mousedrag,         "mousedrag",        A_NULL, 0);
        eclass_addmethod(c, (method) rslider_mouseup,           "mouseup",          A_NULL, 0);
        
        CLASS_ATTR_DEFAULT              (c, "size", 0, "15. 120.");
        
        CLASS_ATTR_RGBA                 (c, "bgcolor", 0, t_rslider, f_color_background);
        CLASS_ATTR_LABEL                (c, "bgcolor", 0, "Background Color");
        CLASS_ATTR_ORDER                (c, "bgcolor", 0, "1");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bgcolor", 0, "0.75 0.75 0.75 1.");
        CLASS_ATTR_STYLE                (c, "bgcolor", 0, "color");
        
        CLASS_ATTR_RGBA                 (c, "bdcolor", 0, t_rslider, f_color_border);
        CLASS_ATTR_LABEL                (c, "bdcolor", 0, "Border Color");
        CLASS_ATTR_ORDER                (c, "bdcolor", 0, "2");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bdcolor", 0, "0.5 0.5 0.5 1.");
        CLASS_ATTR_STYLE                (c, "bdcolor", 0, "color");
        
        CLASS_ATTR_RGBA                 (c, "kncolor", 0, t_rslider, f_color_knob);
        CLASS_ATTR_LABEL                (c, "kncolor", 0, "Knob Color");
        CLASS_ATTR_ORDER                (c, "kncolor", 0, "3");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "kncolor", 0, "0.5 0.5 0.5 1.");
        CLASS_ATTR_STYLE                (c, "kncolor", 0, "color");
        
        eclass_register(CLASS_BOX, c);
        rslider_class = c;        
    }
}








