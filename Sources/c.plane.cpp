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

typedef struct  _plane
{
	t_ebox      j_box;
    t_outlet*   f_out_x;
    t_outlet*   f_out_y;
    float       f_size;
	t_rgba		f_color_background;
	t_rgba		f_color_border;
	t_rgba		f_color_point;
    void*       f_dummy;
} t_plane;

static t_eclass *plane_class;

static void plane_output(t_plane *x)
{
    t_atom argv[2];
    t_pd* send = ebox_getsender((t_ebox *) x);
    const float xval = ebox_parameter_getvalue((t_ebox *)x, 1);
    const float yval = ebox_parameter_getvalue((t_ebox *)x, 2);
    atom_setfloat(argv, xval);
    atom_setfloat(argv+1, yval);
    outlet_list(x->f_out_x, &s_list, 2, argv);
    outlet_float(x->f_out_y, yval);
    if(send)
    {
        pd_list(send, &s_list, 2, argv);
    }
}

static void plane_float(t_plane *x, float f)
{
    ebox_parameter_setvalue((t_ebox *)x, 1, f, eobj_getproxy(x) + 1);
    plane_output(x);
    ebox_invalidate_layer((t_ebox *)x, NULL, cream_sym_points_layer);
    ebox_redraw((t_ebox *)x);
}

static void plane_set(t_plane *x, t_symbol* s, int argc, t_atom *argv)
{
    if(argc > 0 && atom_gettype(argv) == A_FLOAT)
    {
        ebox_parameter_setvalue((t_ebox *)x, 1, atom_getfloat(argv), 0);
    }
    if(argc > 1 && atom_gettype(argv+1) == A_FLOAT)
    {
        ebox_parameter_setvalue((t_ebox *)x, 1, atom_getfloat(argv+1), 0);
    }
    ebox_invalidate_layer((t_ebox *)x, NULL, cream_sym_points_layer);
    ebox_redraw((t_ebox *)x);
}

static void plane_bang(t_plane *x)
{
    ebox_parameter_notify_changes((t_ebox *)x, 1);
    ebox_parameter_notify_changes((t_ebox *)x, 2);
    plane_output(x);
}

static void plane_list(t_plane *x, t_symbol* s, int argc, t_atom *argv)
{
    if(argc > 0 && atom_gettype(argv) == A_FLOAT)
    {
        ebox_parameter_setvalue((t_ebox *)x, 1, atom_getfloat(argv), 1);
    }
    if(argc > 1 && atom_gettype(argv+1) == A_FLOAT)
    {
        ebox_parameter_setvalue((t_ebox *)x, 1, atom_getfloat(argv+1), 1);
    }
    plane_output(x);
    ebox_invalidate_layer((t_ebox *)x, NULL, cream_sym_points_layer);
    ebox_redraw((t_ebox *)x);
}


static void plane_getdrawparams(t_plane *x, t_object *view, t_edrawparams *params)
{
    params->d_borderthickness   = 2;
    params->d_cornersize        = 2;
    params->d_bordercolor       = x->f_color_border;
    params->d_boxfillcolor      = x->f_color_background;
}

static void plane_oksize(t_plane *x, t_rect *newrect)
{
    newrect->width = pd_clip_min(newrect->width, 15.);
    newrect->height = pd_clip_min(newrect->height, 15.);
}

static t_pd_err plane_notify(t_plane *x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
	if(msg == cream_sym_attr_modified)
	{
		if(s == cream_sym_bgcolor || s == cream_sym_bdcolor || s == cream_sym_ptcolor || s == cream_sym_ptsize)
		{
			ebox_invalidate_layer((t_ebox *)x, NULL, cream_sym_points_layer);
		}
	}
    else if(msg == cream_sym_value_changed)
    {
        plane_output(x);
        ebox_invalidate_layer((t_ebox *)x, NULL, cream_sym_points_layer);
        ebox_redraw((t_ebox *)x);
    }
	return 0;
}

static void plane_paint(t_plane *x, t_object *view)
{
	t_rect rect;
	ebox_getdrawbounds((t_ebox *)x, view,  &rect);
    t_elayer *g = ebox_start_layer((t_ebox *)x, view, cream_sym_points_layer, rect.width, rect.height);
    if(g)
    {
        const float size = x->f_size;
        const char inv1 = ebox_parameter_isinverted((t_ebox *)x, 1);
        const char inv2 = ebox_parameter_isinverted((t_ebox *)x, 2);
        const float valx = ebox_parameter_getvalue_normalized((t_ebox *)x, 1);
        const float valy = ebox_parameter_getvalue_normalized((t_ebox *)x, 2);
        elayer_set_color_rgba(g, &x->f_color_point);
        if(!inv1 && !inv2)
            elayer_circle(g, valx * (rect.width - size * 2.f) + size, (1.f - valy) * (rect.height - size * 2.f) + size, size);
        else if(!inv1 && inv2)
            elayer_circle(g, valx * (rect.width - size * 2.f) + size, valy * (rect.height - size * 2.f) + size, size);
        else if(inv1 && !inv2)
            elayer_circle(g, (1.f - valx) * (rect.width - size * 2.f) + size, (1.f - valy) * (rect.height - size * 2.f) + size, size);
        else
            elayer_circle(g, (1.f - valx) * (rect.width - size * 2.f) + size, valy * (rect.height - size * 2.f) + size, size);
        
        elayer_fill(g);
        ebox_end_layer((t_ebox*)x, view, cream_sym_points_layer);
    }
    ebox_paint_layer((t_ebox *)x, view, cream_sym_points_layer, 0, 0);
    
}

static float plane_getvalue(t_rect const* rect, float p, float min, float max, float size)
{
    const char inverted = (char)(min < max);
    if(inverted)
        return (p - size) * (max - min) / (rect->width - size * 2.f) + min;
    else
        return (rect->width - p - size) * (min - max) / (rect->width - size * 2.f) + max;
}

static void plane_mousedown(t_plane *x, t_object *view, t_pt pt, long modifiers)
{
    t_rect rect;
    ebox_getdrawbounds((t_ebox *)x, view,  &rect);
    ebox_parameter_begin_changes((t_ebox *)x, 1);
    ebox_parameter_begin_changes((t_ebox *)x, 2);
    const float minx = ebox_parameter_getmin((t_ebox *)x, 1);
    const float maxx = ebox_parameter_getmax((t_ebox *)x, 1);
    const float miny = ebox_parameter_getmin((t_ebox *)x, 2);
    const float maxy = ebox_parameter_getmax((t_ebox *)x, 2);
    const float valx = plane_getvalue(&rect, pt.x, minx, maxx, x->f_size);
    const float valy = plane_getvalue(&rect, pt.y, miny, maxy, x->f_size);
    ebox_parameter_setvalue((t_ebox *)x, 1, valx, 1);
    ebox_parameter_setvalue((t_ebox *)x, 2, valy, 1);
}

static void plane_mousedrag(t_plane *x, t_object *view, t_pt pt, long modifiers)
{
    t_rect rect;
    ebox_getdrawbounds((t_ebox *)x, view,  &rect);
    const float minx = ebox_parameter_getmin((t_ebox *)x, 1);
    const float maxx = ebox_parameter_getmax((t_ebox *)x, 1);
    const float miny = ebox_parameter_getmin((t_ebox *)x, 2);
    const float maxy = ebox_parameter_getmax((t_ebox *)x, 2);
    const float valx = plane_getvalue(&rect, pt.x, minx, maxx, x->f_size);
    const float valy = plane_getvalue(&rect, (rect.height - pt.y), miny, maxy, x->f_size);
    ebox_parameter_setvalue((t_ebox *)x, 1, valx, 1);
    ebox_parameter_setvalue((t_ebox *)x, 2, valy, 1);
    
    plane_output(x);
    ebox_invalidate_layer((t_ebox *)x, NULL, cream_sym_points_layer);
    ebox_redraw((t_ebox *)x);
}

static void plane_mouseup(t_plane *x, t_object *view, t_pt pt, long modifiers)
{
    ebox_parameter_end_changes((t_ebox *)x, 1);
    ebox_parameter_end_changes((t_ebox *)x, 2);
}

static t_pd_err plane_bounds_set(t_plane *x, t_object *attr, int ac, t_atom *av)
{
    if(ac && av)
    {
        if(ac && av)
        {
            const float minx = pd_clip((atom_gettype(av) == A_FLOAT) ? atom_getfloat(av) : ebox_parameter_getmin((t_ebox *)x, 1), -FLT_MAX, FLT_MAX);
            const float miny = pd_clip((ac > 1  && atom_gettype(av+1) == A_FLOAT) ? atom_getfloat(av+1) : ebox_parameter_getmin((t_ebox *)x, 2), -FLT_MAX, FLT_MAX);
            const float maxx = pd_clip((ac > 2  && atom_gettype(av+2) == A_FLOAT) ? atom_getfloat(av+2) : ebox_parameter_getmax((t_ebox *)x, 1), -FLT_MAX, FLT_MAX);
            const float maxy = pd_clip((ac > 3  && atom_gettype(av+3) == A_FLOAT) ? atom_getfloat(av+3) : ebox_parameter_getmax((t_ebox *)x, 2), -FLT_MAX, FLT_MAX);
            
            ebox_parameter_setminmax((t_ebox *)x, 1, minx, maxx);
            ebox_parameter_setminmax((t_ebox *)x, 2, miny, maxy);
        }
        
        ebox_invalidate_layer((t_ebox *)x, NULL, cream_sym_points_layer);
        ebox_redraw((t_ebox *)x);
    }
    return 0;
}

static t_pd_err plane_bounds_get(t_plane *x, t_object *attr, int* ac, t_atom **av)
{
    *ac = 4;
    *av = (t_atom *)malloc(4 * sizeof(t_atom));
    if(*av)
    {
        atom_setfloat(*av, ebox_parameter_getmin((t_ebox *)x, 1));
        atom_setfloat(*av+1, ebox_parameter_getmin((t_ebox *)x, 2));
        atom_setfloat(*av+2, ebox_parameter_getmax((t_ebox *)x, 1));
        atom_setfloat(*av+3, ebox_parameter_getmax((t_ebox *)x, 2));
    }
    else
    {
        *ac = 0;
    }
    return 0;
}

static void *plane_new(t_symbol *s, int argc, t_atom *argv)
{
    t_plane *x = (t_plane *)eobj_new(plane_class);
    t_binbuf* d = binbuf_via_atoms(argc,argv);
    if(x && d)
    {
        ebox_new((t_ebox *)x, 0 | EBOX_GROWLINK);
        eobj_proxynew(x);
        eobj_proxynew(x);
        ebox_parameter_create((t_ebox *)x, 1);
        ebox_parameter_create((t_ebox *)x, 2);
        x->f_out_x = outlet_new((t_object *)x, &s_float);
        x->f_out_y = outlet_new((t_object *)x, &s_float);
        
        eobj_attr_read(x, d);
        ebox_ready((t_ebox *)x);
        return x;
    }
    
    return  NULL;
}

extern "C" void setup_c0x2eplane(void)
{
    t_eclass *c = eclass_new("c.plane", (t_method)plane_new, (t_method)ebox_free, (short)sizeof(t_plane), CLASS_NOINLET, A_GIMME, 0);
    
    if(c)
    {
        eclass_guiinit(c, 0);
        
        eclass_addmethod(c, (t_method) plane_paint,           "paint",            A_NULL, 0);
        eclass_addmethod(c, (t_method) plane_notify,          "notify",           A_NULL, 0);
        eclass_addmethod(c, (t_method) plane_getdrawparams,   "getdrawparams",    A_NULL, 0);
        eclass_addmethod(c, (t_method) plane_oksize,          "oksize",           A_NULL, 0);
        
        eclass_addmethod(c, (t_method) plane_bang,            "bang",             A_FLOAT,0);
        eclass_addmethod(c, (t_method) plane_float,           "float",            A_FLOAT,0);
        eclass_addmethod(c, (t_method) plane_set,             "set",              A_GIMME,0);
        eclass_addmethod(c, (t_method) plane_list,            "list",             A_GIMME,0);
        eclass_addmethod(c, (t_method) plane_output,          "bang",             A_NULL, 0);
        
        eclass_addmethod(c, (t_method) plane_mousedown,       "mousedown",        A_NULL, 0);
        eclass_addmethod(c, (t_method) plane_mousedrag,       "mousedrag",        A_NULL, 0);
        eclass_addmethod(c, (t_method) plane_mouseup,         "mouseup",          A_NULL, 0);
        
        CLASS_ATTR_DEFAULT              (c, "size", 0, "120 120");
        
        CLASS_ATTR_FLOAT_ARRAY          (c, "bounds", 0, t_plane, f_dummy, 4);
        CLASS_ATTR_LABEL                (c, "bounds", 0, "Boundaries");
        CLASS_ATTR_ACCESSORS			(c, "bounds", plane_bounds_get, plane_bounds_set);
        CLASS_ATTR_ORDER                (c, "bounds", 0, "2");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bounds", 0, "-1. -1. 1. 1.");
        
        CLASS_ATTR_FLOAT                (c, "ptsize", 0, t_plane, f_size);
        CLASS_ATTR_LABEL                (c, "ptsize", 0, "Point size");
        CLASS_ATTR_ORDER                (c, "ptsize", 0, "3");
        CLASS_ATTR_FILTER_CLIP          (c, "ptsize", 1., 50.f);
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "ptsize", 0, "5");
        CLASS_ATTR_STYLE                (c, "ptsize", 0, "number");
        CLASS_ATTR_STEP                 (c, "ptsize", 0.5);
        
        CLASS_ATTR_RGBA                 (c, "bgcolor", 0, t_plane, f_color_background);
        CLASS_ATTR_LABEL                (c, "bgcolor", 0, "Background Color");
        CLASS_ATTR_ORDER                (c, "bgcolor", 0, "1");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bgcolor", 0, "0.75 0.75 0.75 1.");
        CLASS_ATTR_STYLE                (c, "bgcolor", 0, "color");
        
        CLASS_ATTR_RGBA                 (c, "bdcolor", 0, t_plane, f_color_border);
        CLASS_ATTR_LABEL                (c, "bdcolor", 0, "Border Color");
        CLASS_ATTR_ORDER                (c, "bdcolor", 0, "2");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bdcolor", 0, "0.5 0.5 0.5 1.");
        CLASS_ATTR_STYLE                (c, "bdcolor", 0, "color");
        
        CLASS_ATTR_RGBA                 (c, "ptcolor", 0, t_plane, f_color_point);
        CLASS_ATTR_LABEL                (c, "ptcolor", 0, "Point Color");
        CLASS_ATTR_ORDER                (c, "ptcolor", 0, "3");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "ptcolor", 0, "0.5 0.5 0.5 1.");
        CLASS_ATTR_STYLE                (c, "ptcolor", 0, "color");
        
        eclass_register(CLASS_BOX, c);
        plane_class = c;
    }
}



