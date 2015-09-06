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

typedef struct _radio
{
	t_ebox      j_box;
    t_outlet*   f_out_list;
    t_outlet*   f_out_flag;
	t_rgba		f_color_background;
	t_rgba		f_color_border;
	t_rgba		f_color_item;
    char        f_direction;
    int         f_nitems;
    int         f_checklist;
} t_radio;

static t_eclass *radio_class;

static void radio_output(t_radio *x)
{
    int i;
    t_atom argv[CREAM_MAXITEMS];
    t_pd* send = ebox_getsender((t_ebox *) x);
    const int flags = (int)ebox_parameter_getvalue((t_ebox *)x, 1);
    if(x->f_checklist)
    {
        for(i = 0; i < x->f_nitems; i++)
        {
            atom_setfloat(argv+i, (flags & (1<<i) ? 1 : 0));
        }
        outlet_list(x->f_out_list, &s_list, x->f_nitems, argv);
        if(send)
        {
            pd_list(send, &s_list, x->f_nitems, argv);
        }
    }
    else
    {
        for(i = 0; i < x->f_nitems; i++)
        {
            if(flags & (1<<i))
            {
                outlet_float(x->f_out_list, (float)i);
                if(send)
                {
                    pd_float(send, (float)i);
                }
                break;
            }
        }
    }
    outlet_float(x->f_out_flag, (float)flags);
}

static void radio_float(t_radio *x, float f)
{
    if(x->f_checklist)
    {
        const int flags = ebox_parameter_getvalue((t_ebox *)x, 1);
        if(f)
        {
            if(flags & (1<<0))
            {
                ebox_parameter_notify_changes((t_ebox *)x, 1);
            }
            else
            {
                ebox_parameter_setvalue((t_ebox *)x, 1, (float)(flags | (1<<0)), 1);
            }
        }
        else
        {
            if(flags & (1<<0))
            {
                ebox_parameter_setvalue((t_ebox *)x, 1, (float)(flags & ~(1<<0)), 1);
            }
            else
            {
                ebox_parameter_notify_changes((t_ebox *)x, 1);
            }
        }
    }
    else
    {
        int index = (int)pd_clip(f, 0.f, (float)x->f_nitems - 1);
        ebox_parameter_setvalue((t_ebox *)x, 1, (float)(1<<index), 1);
    }
    
    radio_output(x);
    ebox_invalidate_layer((t_ebox *)x, cream_sym_items_layer);
    ebox_redraw((t_ebox *)x);
}

static void radio_flags(t_radio *x, float f)
{
    int i, flags = 0;
    if(x->f_checklist)
    {
        for(i = 0; i < x->f_nitems; i++)
        {
            if((int)(f) & (1<<i))
            {
                flags |= (1<<i);
            }
        }
    }
    else
    {
        flags = 1<<0;
        for(i = 0; i < x->f_nitems; i++)
        {
            if((int)(f) < (1<<i))
            {
                break;
            }
            flags = (1<<i);
        }
    }
    ebox_parameter_setvalue((t_ebox *)x, 1, flags, 1);
    radio_output(x);
    ebox_invalidate_layer((t_ebox *)x, cream_sym_items_layer);
    ebox_redraw((t_ebox *)x);
}

static void radio_set(t_radio *x, t_symbol* s, int argc, t_atom *argv)
{
    int i;
    if(argc && argv)
    {
        if(x->f_checklist)
        {
            int flags = (int)ebox_parameter_getvalue((t_ebox *)x, 1);
            for(i = 0; i < x->f_nitems && i < argc; i++)
            {
                if(atom_gettype(argv+i) == A_FLOAT)
                {
                    if(atom_getfloat(argv+i) != 0 && !(flags & (1<<i)))
                    {
                        flags = flags | (1<<i);
                    }
                    else if(atom_getfloat(argv+i) == 0.f && (flags & (1<<i)))
                    {
                        flags = flags & ~(1<<i);
                    }
                }
            }
            ebox_parameter_setvalue((t_ebox *)x, 1, flags, 0);
        }
        else if(atom_gettype(argv) == A_FLOAT)
        {
            int index = (int)pd_clip(atom_getfloat(argv), 0.f, (float)x->f_nitems - 1);
            ebox_parameter_setvalue((t_ebox *)x, 1, (float)(1<<index), 0);
        }
        
        ebox_invalidate_layer((t_ebox *)x, cream_sym_items_layer);
        ebox_redraw((t_ebox *)x);
    }
}

static void radio_list(t_radio *x, t_symbol* s, int argc, t_atom *argv)
{
    int i;
    if(argc && argv)
    {
        if(x->f_checklist)
        {
            int flags = (int)ebox_parameter_getvalue((t_ebox *)x, 1);
            for(i = 0; i < x->f_nitems && i < argc; i++)
            {
                if(atom_gettype(argv+i) == A_FLOAT)
                {
                    if(atom_getfloat(argv+i) != 0 && !(flags & (1<<i)))
                    {
                        flags = flags | (1<<i);
                    }
                    else if(atom_getfloat(argv+i) == 0.f && (flags & (1<<i)))
                    {
                        flags = flags & ~(1<<i);
                    }
                }
            }
            ebox_parameter_setvalue((t_ebox *)x, 1, flags, 1);
        }
        else if(atom_gettype(argv) == A_FLOAT)
        {
            int index = (int)pd_clip(atom_getfloat(argv), 0.f, (float)x->f_nitems - 1);
            ebox_parameter_setvalue((t_ebox *)x, 1, (float)(1<<index), 0);
        }
        
        radio_output(x);
        ebox_invalidate_layer((t_ebox *)x, cream_sym_items_layer);
        ebox_redraw((t_ebox *)x);
    }
}

static void radio_setter_t(t_radio *x, int index, char const* text);

static void radio_bang(t_radio *x)
{
    ebox_parameter_notify_changes((t_ebox *)x, 1);
    radio_output(x);
}

static t_pd_err radio_notify(t_radio *x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
	if(msg == cream_sym_attr_modified)
	{
		if(s == cream_sym_bgcolor || s == cream_sym_bdcolor || s == cream_sym_itcolor)
		{
			ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
            ebox_invalidate_layer((t_ebox *)x, cream_sym_items_layer);
		}
        else if(s == cream_sym_checklist)
        {
            ebox_parameter_setvalue((t_ebox *)x, 1, 0, 1);
            ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
            ebox_invalidate_layer((t_ebox *)x, cream_sym_items_layer);
        }
	}
    else if(msg == cream_sym_value_changed)
    {
        radio_output(x);
        ebox_invalidate_layer((t_ebox *)x, cream_sym_points_layer);
        ebox_redraw((t_ebox *)x);
    }
	return 0;
}

static void radio_getdrawparams(t_radio *x, t_object *patcherview, t_edrawparams *params)
{
    params->d_borderthickness   = 2;
    params->d_cornersize        = 2;
    params->d_bordercolor       = x->f_color_border;
    params->d_boxfillcolor      = x->f_color_background;
}

static void radio_oksize(t_radio *x, t_rect *newrect)
{
    newrect->width = pd_clip_min(newrect->width, 15.);
    newrect->height = pd_clip_min(newrect->height, 15.);
    x->f_direction = (newrect->width > newrect->height) ? 1 : 0;
    if(x->f_direction)
    {
        newrect->width = pd_clip_min(newrect->width, x->f_nitems * newrect->height);
    }
    else
    {
        newrect->height = pd_clip_min(newrect->height, x->f_nitems * newrect->width);
    }
}


static void draw_background(t_radio *x, t_object *view, t_rect *rect)
{
	int i;
	t_elayer *g = ebox_start_layer((t_ebox *)x, cream_sym_background_layer, rect->width, rect->height);
    
	if (g)
	{
        egraphics_set_color_rgba(g, &x->f_color_border);
        egraphics_set_line_width(g, 2.f);
        const float ratio = x->f_direction ? ((rect->width - 4.f) / x->f_nitems) : (rect->height - 4.f) / x->f_nitems;
        if(x->f_checklist)
        {
            const float offset = x->f_direction ? (rect->height * 0.5f - 2.f) : (rect->width * 0.5f - 2.f);
            const float dist = offset * 2.f;
            if(x->f_direction)
            {
                for(i = 0; i < x->f_nitems; i++)
                {
                    egraphics_rectangle(g, (i + 0.5f) * ratio - offset + 2.f, 2.f, dist, dist);
                    egraphics_stroke(g);
                }
            }
            else
            {
                for(i = 0; i < x->f_nitems; i++)
                {
                    egraphics_rectangle(g, 2.f, (i + 0.5) * ratio - offset + 2.f, dist, dist);
                    egraphics_stroke(g);
                }
            }
        }
        else
        {
            if(x->f_direction)
            {
                const float height = rect->height * 0.5f;
                for(i = 0; i < x->f_nitems; i++)
                {
                    egraphics_circle(g, (i + 0.5f) * ratio + 2.f, height, height - 2.f);
                    egraphics_stroke(g);
                }
            }
            else
            {
                const float width = rect->width * 0.5f;
                for(i = 0; i < x->f_nitems; i++)
                {
                    egraphics_circle(g, width, (i + 0.5f) * ratio + 2.f, width - 2.f);
                    egraphics_stroke(g);
                }
            }
        }
    
        ebox_end_layer((t_ebox*)x, cream_sym_background_layer);
	}
	ebox_paint_layer((t_ebox *)x, cream_sym_background_layer, 0., 0.);
}

static void draw_items(t_radio *x, t_object *view, t_rect *rect)
{
	int i;
	t_elayer *g = ebox_start_layer((t_ebox *)x, cream_sym_items_layer, rect->width, rect->height);
	if (g)
	{
        egraphics_set_color_rgba(g, &x->f_color_item);
        const int   flags = (int)ebox_parameter_getvalue((t_ebox *)x, 1);
        const float ratio = x->f_direction ? ((rect->width - 4.f) / x->f_nitems) : (rect->height - 4.f) / x->f_nitems;
        if(x->f_checklist)
        {
            egraphics_set_line_width(g, 2.f);
            const float offset = x->f_direction ? (rect->height * 0.5f - 4.f) : (rect->width * 0.5f - 4.f);
            const float dist = offset * 2.f;
            if(x->f_direction)
            {
                for(i = 0; i < x->f_nitems; i++)
                {
                    if(flags & (1<<i))
                    {
                        const float val = (i + 0.5f) * ratio - offset + 2.f;
                        egraphics_line_fast(g, val, 4.f, val + dist, 4.f + dist);
                        egraphics_line_fast(g, val, 4.f  + dist, val + dist, 4.f);
                    }
                }
            }
            else
            {
                for(i = 0; i < x->f_nitems; i++)
                {
                    if(flags & (1<<i))
                    {
                        const float val = (i + 0.5f) * ratio - offset + 2.f;
                        egraphics_line_fast(g, 4.f, val, 4.f + dist, val + dist);
                        egraphics_line_fast(g, 4.f + dist, val, 4.f, val + dist);
                    }
                }
            }
        }
        else
        {
            if(x->f_direction)
            {
                const float height = rect->height * 0.5f;
                for(i = 0; i < x->f_nitems; i++)
                {
                    if(flags & (1<<i))
                    {
                        egraphics_circle(g, (i + 0.5f) * ratio + 2.f, height, height - 4.f);
                        egraphics_fill(g);
                        break;
                    }
                }
            }
            else
            {
                const float width = rect->width * 0.5f;
                for(i = 0; i < x->f_nitems; i++)
                {
                    if(flags & (1<<i))
                    {
                        egraphics_circle(g, width, (i + 0.5f) * ratio + 2.f, width - 4.f);
                        egraphics_fill(g);
                        break;
                    }
                }
            }
        }
        
        ebox_end_layer((t_ebox*)x, cream_sym_items_layer);
	}
	ebox_paint_layer((t_ebox *)x, cream_sym_items_layer, 0., 0.);
}

static void radio_paint(t_radio *x, t_object *view)
{
    t_rect rect;
    ebox_get_rect_for_view((t_ebox *)x, &rect);
    draw_background(x, view, &rect);
    draw_items(x, view, &rect);
}

static void radio_mousedown(t_radio *x, t_object *patcherview, t_pt pt, long modifiers)
{
    t_rect rect;
    ebox_get_rect_for_view((t_ebox *)x, &rect);
    const float rel = x->f_direction ? ((pt.x - 2.f) / (rect.width - 4.f)) : ((pt.y - 2.f) / (rect.height - 4.f));
    const int index = (int)pd_clip(rel * (float)x->f_nitems, 0.f, (float)x->f_nitems - 1.f);
    if(index >= 0 && index < x->f_nitems)
    {
        ebox_parameter_begin_changes((t_ebox *)x, 1);
        if(x->f_checklist)
        {
            const int flags = ebox_parameter_getvalue((t_ebox *)x, 1);
            if(flags & (1<<index))
            {
                ebox_parameter_setvalue((t_ebox *)x, 1, (float)(flags & ~(1<<index)), 1);
            }
            else
            {
                ebox_parameter_setvalue((t_ebox *)x, 1, (float)(flags | 1<<index), 1);
            }
        }
        else
        {
            ebox_parameter_setvalue((t_ebox *)x, 1, (float)(0 | 1<<index), 1);
        }
        ebox_parameter_end_changes((t_ebox *)x, 1);
        radio_output(x);
        ebox_invalidate_layer((t_ebox *)x, cream_sym_items_layer);
        ebox_redraw((t_ebox *)x);
    }
}

static t_pd_err radio_nitems_set(t_radio *x, t_object *attr, int ac, t_atom *av)
{
    if(ac && av && atom_gettype(av) == A_FLOAT)
    {
        x->f_nitems = pd_clip(atom_getfloat(av), 1, CREAM_MAXITEMS);
        ebox_parameter_setvalue((t_ebox *)x, 1, 0, 0);
        ebox_parameter_setminmax((t_ebox *)x, 1, 0.f, powf(2., (float)(x->f_nitems)) - 1.f);
        ebox_parameter_setnstep((t_ebox *)x, 1,  (int)powf(2., (float)(x->f_nitems)));
        ebox_notify((t_ebox *)x, s_cream_size, cream_sym_attr_modified, NULL, NULL);
    }
    return 0;
}

static void radio_setter_t(t_radio *x, int index, char const* text)
{
    if(x->f_checklist)
    {
        int i = 0;
        int value;
        int flags = (int)ebox_parameter_getvalue((t_ebox *)x, 1);
        size_t pos, len = strlen(text);
        pos = strcspn(text, "01");
        while(pos < len && i < x->f_nitems)
        {
            value = atoi(text+pos);
            if(value && !(flags & (1<<i)))
            {
                flags = flags | (1<<i);
            }
            else if(!value && (flags & (1<<i)))
            {
               flags = flags & ~(1<<i);
            }
            i++;
            pos += strcspn(text+pos+1, "01") + 1;
        }
        ebox_parameter_setvalue((t_ebox *)x, 1, flags, 0);
        radio_output(x);
        ebox_invalidate_layer((t_ebox *)x, cream_sym_items_layer);
        ebox_redraw((t_ebox *)x);
    }
    else if(isdigit(text[0]))
    {
        const int _index = (int)pd_clip((float)atof(text), 0.f, (float)x->f_nitems - 1);
        ebox_parameter_setvalue((t_ebox *)x, 1, (float)(1<<_index), 0);
        
        radio_output(x);
        ebox_invalidate_layer((t_ebox *)x, cream_sym_items_layer);
        ebox_redraw((t_ebox *)x);
    }
}

static void radio_getter_t(t_radio *x, int index, char* text)
{
    int i;
    char temp[2];
    const int flags = ebox_parameter_getvalue((t_ebox *)x, index);
    if(x->f_checklist)
    {
        sprintf(text, "");
        for(i = 0; i < x->f_nitems; i++)
        {
            sprintf(temp, "%i ", (flags & (1<<i)) ? 1 : 0);
            strncat(text, temp, 2);
        }
    }
    else
    {
        for(i = 0; i < x->f_nitems; i++)
        {
            if(flags & (1<<i))
            {
                sprintf(text, "%i",i);
                break;
            }
        }
    }
}

static void *radio_new(t_symbol *s, int argc, t_atom *argv)
{
    t_radio *x = (t_radio *)eobj_new(radio_class);
    t_binbuf* d = binbuf_via_atoms(argc,argv);
    
    if(x && d)
    {
        ebox_new((t_ebox *)x, 0 | EBOX_GROWINDI);
        ebox_parameter_create((t_ebox *)x, 1);
        ebox_parameter_setsettergetter_text((t_ebox *)x, 1,
                                            (t_param_setter_t)radio_setter_t,
                                            (t_param_getter_t)radio_getter_t);
        x->f_out_list = outlet_new((t_object *)x, &s_anything);
        x->f_out_flag = outlet_new((t_object *)x, &s_float);

        ebox_attrprocess_viabinbuf(x, d);
        ebox_ready((t_ebox *)x);
        return x;
    }
    return NULL;
}

extern "C" void setup_c0x2eradio(void)
{
    t_eclass *c = eclass_new("c.radio", (method)radio_new, (method)ebox_free, (short)sizeof(t_radio), 0L, A_GIMME, 0);
    if(c)
    {
        eclass_guiinit(c, 0);
        eclass_addmethod(c, (method) radio_paint,           "paint",            A_NULL, 0);
        eclass_addmethod(c, (method) radio_notify,          "notify",           A_NULL, 0);
        eclass_addmethod(c, (method) radio_getdrawparams,   "getdrawparams",    A_NULL, 0);
        eclass_addmethod(c, (method) radio_oksize,          "oksize",           A_NULL, 0);
        
        eclass_addmethod(c, (method) radio_set,             "set",              A_GIMME,0);
        eclass_addmethod(c, (method) radio_list,            "list",             A_GIMME,0);
        eclass_addmethod(c, (method) radio_float,           "float",            A_FLOAT,0);
        eclass_addmethod(c, (method) radio_bang,            "bang",             A_NULL, 0);
        eclass_addmethod(c, (method) radio_flags,           "flags",            A_FLOAT, 0);
        
        eclass_addmethod(c, (method) radio_mousedown,       "mousedown",        A_NULL, 0);
        
        CLASS_ATTR_DEFAULT              (c, "size", 0, "15. 120.");
        
        CLASS_ATTR_INT                  (c, "nitems", 0, t_radio, f_nitems);
        CLASS_ATTR_LABEL                (c, "nitems", 0, "Number of Items");
        CLASS_ATTR_ACCESSORS			(c, "nitems", NULL, radio_nitems_set);
        CLASS_ATTR_ORDER                (c, "nitems", 0, "1");
        CLASS_ATTR_FILTER_CLIP          (c, "nitems", 1, CREAM_MAXITEMS);
        CLASS_ATTR_DEFAULT              (c, "nitems", 0, "8");
        CLASS_ATTR_SAVE                 (c, "nitems", 1);
        CLASS_ATTR_STYLE                (c, "nitems", 0, "number");
        
        CLASS_ATTR_INT                  (c, "checklist", 0, t_radio, f_checklist);
        CLASS_ATTR_LABEL                (c, "checklist", 0, "Check List Mode");
        CLASS_ATTR_ACCESSORS			(c, "checklist", NULL, NULL);
        CLASS_ATTR_ORDER                (c, "checklist", 0, "1");
        CLASS_ATTR_FILTER_CLIP          (c, "checklist", 0, 1);
        CLASS_ATTR_DEFAULT              (c, "checklist", 0, "0");
        CLASS_ATTR_SAVE                 (c, "checklist", 1);
        CLASS_ATTR_STYLE                (c, "checklist", 0, "onoff");
        CLASS_ATTR_PAINT                (c, "checklist", 0);
        
        CLASS_ATTR_RGBA                 (c, "bgcolor", 0, t_radio, f_color_background);
        CLASS_ATTR_LABEL                (c, "bgcolor", 0, "Background Color");
        CLASS_ATTR_ORDER                (c, "bgcolor", 0, "1");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bgcolor", 0, "0.75 0.75 0.75 1.");
        CLASS_ATTR_STYLE                (c, "bgcolor", 0, "color");
        
        CLASS_ATTR_RGBA                 (c, "bdcolor", 0, t_radio, f_color_border);
        CLASS_ATTR_LABEL                (c, "bdcolor", 0, "Border Color");
        CLASS_ATTR_ORDER                (c, "bdcolor", 0, "2");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bdcolor", 0, "0.5 0.5 0.5 1.");
        CLASS_ATTR_STYLE                (c, "bdcolor", 0, "color");
        
        CLASS_ATTR_RGBA                 (c, "itcolor", 0, t_radio, f_color_item);
        CLASS_ATTR_LABEL                (c, "itcolor", 0, "Item Color");
        CLASS_ATTR_ORDER                (c, "itcolor", 0, "3");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "itcolor", 0, "0.5 0.5 0.5 1.");
        CLASS_ATTR_STYLE                (c, "itcolor", 0, "color");
        
        eclass_register(CLASS_BOX, c);
        radio_class = c;
    }
}




