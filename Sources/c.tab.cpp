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

typedef struct  _tab
{
	t_ebox      j_box;
    t_outlet*   f_out_index;
    t_outlet*   f_out_item;
    t_outlet*   f_out_hover;
    t_symbol*   f_items[CREAM_MAXITEMS];
    long        f_nitems;
    int         f_item_hover;
    char        f_toggle;
    char        f_off;
    char        f_orientation;
    t_efont     f_font;
	t_rgba		f_color_background;
	t_rgba		f_color_border;
	t_rgba		f_color_text;
    t_rgba		f_color_hover;
    t_rgba		f_color_select;
} t_tab;

static t_eclass *tab_class;

static t_symbol* tab_gensym(t_symbol* s, int argc, t_atom* argv)
{
    int i, ac;
    t_atom* av, *avt;
    char text[MAXPDSTRING], temp[MAXPDSTRING];
    if(s && argc && argv)
    {
        avt = (t_atom *)malloc((size_t)(argc + 1) * sizeof(t_atom));
        if(avt)
        {
            atom_setsym(avt, s);
            memcpy(avt+1, argv, (size_t)(argc) * sizeof(t_atom));
            unparse_atoms(argc+1, avt, &ac, &av);
            if(ac && av)
            {
                atom_string(av, text, MAXPDSTRING);
                for(i = 1; i < ac; i++)
                {
                    atom_string(av+i, temp, MAXPDSTRING);
                    strncat(text, " ", 1);
                    strncat(text, temp, MAXPDSTRING);
                }
                free(avt);
                free(av);
                return gensym(text);
            }
            free(avt);
        }
    }
    else if(argc && argv)
    {
        unparse_atoms(argc, argv, &ac, &av);
        if(ac && av)
        {
            atom_string(av, text, MAXPDSTRING);
            for(i = 1; i < ac; i++)
            {
                atom_string(av+i, temp, MAXPDSTRING);
                strncat(text, " ", 1);
                strncat(text, temp, MAXPDSTRING);
            }
            free(av);
            return gensym(text);
        }
    }
    return NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//                                          Output                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

static void tab_output(t_tab *x)
{
    t_pd* send = ebox_getsender((t_ebox *) x);
    const float index = ebox_parameter_getvalue((t_ebox *)x, 1);
    outlet_float(x->f_out_index, index);
    outlet_symbol(x->f_out_item, x->f_items[(int)index]);
    if(send)
    {
        pd_float(send, index);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
//                                          Edition                                         //
//////////////////////////////////////////////////////////////////////////////////////////////

static void tab_append(t_tab *x, t_symbol *s, int argc, t_atom *argv)
{
    t_symbol* item = tab_gensym(NULL, argc, argv);
    if(x->f_nitems < CREAM_MAXITEMS - 1 && item)
    {
        x->f_items[x->f_nitems] = item;
        x->f_nitems++;
        ebox_parameter_setminmax((t_ebox *)x, 1, 0.f, (float)x->f_nitems - 1.f);
        ebox_parameter_setnstep((t_ebox *)x, 1, (int)x->f_nitems);
        ebox_notify((t_ebox *)x, s_cream_size, cream_sym_attr_modified, NULL, NULL);
    }
}

static void tab_insert(t_tab *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, index;
    t_symbol* item;
    if(argc && argv && atom_gettype(argv) == A_FLOAT)
    {
        index   = (int)pd_clip(atom_getfloat(argv), 0.f, (float)x->f_nitems);
        item    = tab_gensym(NULL, argc-1, argv+1);
        if(item)
        {
            for(i = (int)x->f_nitems; i > index; i--)
            {
                x->f_items[i] = x->f_items[i-1];
            }
            x->f_items[index] = item;
            x->f_nitems++;
            ebox_parameter_setminmax((t_ebox *)x, 1, 0.f, (float)x->f_nitems - 1.f);
            ebox_parameter_setnstep((t_ebox *)x, 1, (int)x->f_nitems);
            ebox_notify((t_ebox *)x, s_cream_size, cream_sym_attr_modified, NULL, NULL);
        }
    }
}

static void tab_setitem(t_tab *x, t_symbol *s, int argc, t_atom *argv)
{
    int index;
    t_symbol* item;
    if(argc && argv && atom_gettype(argv) == A_FLOAT)
    {
        index   = (int)pd_clip(atom_getfloat(argv), 0.f, (float)x->f_nitems - 1.f);
        item    = tab_gensym(NULL, argc-1, argv+1);
        if(item)
        {
            x->f_items[index] = item;
            ebox_invalidate_layer((t_ebox *)x, cream_sym_text_layer);
            ebox_redraw((t_ebox *)x);
        }
    }
}

static void tab_delete(t_tab *x, float f)
{
    int i;
    const int index = (int)f;
    if(index >= 0 && index < x->f_nitems)
    {
        for(i = index; i < x->f_nitems - 1; i++)
            x->f_items[i] = x->f_items[i+1];
        x->f_nitems--;
        ebox_parameter_setminmax((t_ebox *)x, 1, 0.f, (float)x->f_nitems);
        ebox_parameter_setnstep((t_ebox *)x, 1, (int)x->f_nitems);
        ebox_notify((t_ebox *)x, s_cream_size, cream_sym_attr_modified, NULL, NULL);
    }
}

static void tab_clear(t_tab *x)
{
    x->f_nitems = 0;
    ebox_parameter_setminmax((t_ebox *)x, 1, 0.f, 0.f);
    ebox_parameter_setnstep((t_ebox *)x, 1, 1);
    ebox_notify((t_ebox *)x, s_cream_size, cream_sym_attr_modified, NULL, NULL);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//                                          Selection                                       //
//////////////////////////////////////////////////////////////////////////////////////////////

static void tab_bang(t_tab *x)
{
    if(x->f_nitems)
    {
        ebox_parameter_notify_changes((t_ebox *)x, 1);
        tab_output(x);
    }
}

static void tab_next(t_tab *x)
{
    const float index = ebox_parameter_getvalue((t_ebox *)x, 1);
    ebox_parameter_setvalue((t_ebox *)x, 1, index+1, 1);
    tab_output(x);
    ebox_invalidate_layer((t_ebox *)x, cream_sym_selection_layer);
    ebox_redraw((t_ebox *)x);
}

static void tab_prev(t_tab *x)
{
    const float index = ebox_parameter_getvalue((t_ebox *)x, 1);
    ebox_parameter_setvalue((t_ebox *)x, 1, index-1, 1);
    tab_output(x);
    ebox_invalidate_layer((t_ebox *)x, cream_sym_selection_layer);
    ebox_redraw((t_ebox *)x);
}

static void tab_float(t_tab *x, t_floatarg f)
{
    ebox_parameter_setvalue((t_ebox *)x, 1, f, 1);
    tab_output(x);
    ebox_invalidate_layer((t_ebox *)x, cream_sym_selection_layer);
    ebox_redraw((t_ebox *)x);
}

static void tab_anything(t_tab *x, t_symbol *s, int argc, t_atom *argv)
{
    int i;
    t_symbol* item = tab_gensym(s, argc, argv);
    if(item)
    {
        for(i = 0; i < x->f_nitems; i++)
        {
            if(x->f_items[i] == item)
            {
                ebox_parameter_setvalue((t_ebox *)x, 1, (float)i, 1);
                tab_output(x);
                ebox_invalidate_layer((t_ebox *)x, cream_sym_selection_layer);
                ebox_redraw((t_ebox *)x);
                return;
            }
        }
    }
}

static void tab_set(t_tab *x, t_symbol *s, int argc, t_atom *argv)
{
    int i;
    t_symbol* item;
    if(argc && argv && atom_gettype(argv) == A_FLOAT)
    {
        ebox_parameter_setvalue((t_ebox *)x, 1, atom_getfloat(argv), 0);
        ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
        ebox_redraw((t_ebox *)x);
    }
    else if(argc && argv && atom_gettype(argv) == A_SYMBOL)
    {
        item = tab_gensym(NULL, argc, argv);
        if(item)
        {
            for(i = 0; i < x->f_nitems; i++)
            {
                if(x->f_items[i] == item)
                {
                    ebox_parameter_setvalue((t_ebox *)x, 1, (float)i, 0);
                    ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
                    ebox_redraw((t_ebox *)x);
                    return;
                }
            }
        }
    }
}

static void tab_setsymbol(t_tab *x, t_symbol *s, int argc, t_atom *argv)
{
    int i;
    t_symbol* item = tab_gensym(NULL, argc, argv);
    if(item)
    {
        for(i = 0; i < x->f_nitems; i++)
        {
            if(x->f_items[i] == item)
            {
                ebox_parameter_setvalue((t_ebox *)x, 1, (float)i, 0);
                ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
                ebox_redraw((t_ebox *)x);
                return;
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
//                                          Drawing                                         //
//////////////////////////////////////////////////////////////////////////////////////////////

static void tab_getdrawparams(t_tab *x, t_object *patcherview, t_edrawparams *params)
{
	params->d_borderthickness   = 2;
	params->d_cornersize        = 2;
    params->d_bordercolor       = x->f_color_border;
    params->d_boxfillcolor      = x->f_color_background;
}

static void tab_oksize(t_tab *x, t_rect *newrect)
{
    if(x->f_orientation)
    {
        newrect->width = pd_clip_min(newrect->width, sys_fontwidth(x->f_font.size) * 3);
        newrect->height = pd_clip_min(newrect->height, (x->f_font.size + 4) * pd_clip_min(x->f_nitems, 1));
    }
    else
    {
        newrect->width = pd_clip_min(newrect->width, sys_fontwidth(x->f_font.size) * 3 * pd_clip_min(x->f_nitems, 1));
        newrect->height = pd_clip_min(newrect->height, x->f_font.size + 4);
    }
}

static t_pd_err tab_notify(t_tab *x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
	if (msg == cream_sym_attr_modified)
	{
		if(s == cream_sym_bgcolor ||
           s == cream_sym_bdcolor ||
           s == cream_sym_textcolor ||
           s == cream_sym_hocolor ||
           s == cream_sym_secolor ||
           s == cream_sym_orientation ||
           s == cream_sym_font)
		{
            ebox_invalidate_layer((t_ebox *)x, cream_sym_selection_layer);
            ebox_invalidate_layer((t_ebox *)x, cream_sym_text_layer);
			ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
		}
        else if(s == cream_sym_orientation ||
                s == cream_sym_items)
        {
            ebox_notify((t_ebox *)x, s_cream_size, cream_sym_attr_modified, NULL, NULL);
        }
	}
    else if(msg == cream_sym_value_changed)
    {
        tab_output(x);
        ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
        ebox_redraw((t_ebox *)x);
    }
	return 0;
}

static void draw_background(t_tab *x, t_object *view, t_rect *rect)
{
    int i;
	t_elayer *g = ebox_start_layer((t_ebox *)x, cream_sym_background_layer, rect->width, rect->height);
	if(g)
	{
        egraphics_set_color_rgba(g, &x->f_color_border);
        egraphics_set_line_width(g, 2);
        if(x->f_orientation)
        {
            const float ratio = rect->height / (float)x->f_nitems;
            for(i = 1; i < x->f_nitems; i++)
            {
                egraphics_line_fast(g, 0, ratio * i, rect->width, ratio * i);
            }
        }
        else
        {
            const float ratio = rect->width / (float)x->f_nitems;
            for(i = 1; i < x->f_nitems; i++)
            {
                egraphics_line_fast(g, ratio * i, 0, ratio * i, rect->height);
            }
        }
		ebox_end_layer((t_ebox*)x, cream_sym_background_layer);
	}
	ebox_paint_layer((t_ebox *)x, cream_sym_background_layer,  0., 0.);
}

static void draw_selection(t_tab *x, t_object *view, t_rect *rect)
{
    t_elayer *g = ebox_start_layer((t_ebox *)x, cream_sym_selection_layer, rect->width, rect->height);
    if(g)
    {
        const int selec = (int)ebox_parameter_getvalue((t_ebox *)x, 1);
        egraphics_set_line_width(g, 2);
        if(x->f_orientation)
        {
            const float ratio = rect->height / (float)x->f_nitems;
            if(x->f_item_hover != -1)
            {
                egraphics_set_color_rgba(g, &x->f_color_hover);
                egraphics_rectangle(g, 0, ratio * x->f_item_hover, rect->width, ratio);
                egraphics_fill(g);
            }
            if(x->f_toggle || !x->f_off)
            {
                egraphics_set_color_rgba(g, &x->f_color_select);
                egraphics_rectangle(g, 0, ratio * selec, rect->width, ratio);
                egraphics_fill(g);
            }
        }
        else
        {
            const float ratio = rect->width / (float)x->f_nitems;
            if(x->f_item_hover != -1)
            {
                egraphics_set_color_rgba(g, &x->f_color_hover);
                egraphics_rectangle(g, ratio * x->f_item_hover, 0, ratio, rect->height);
                egraphics_fill(g);
            }
            if(x->f_toggle || !x->f_off)
            {
                egraphics_set_color_rgba(g, &x->f_color_select);
                egraphics_rectangle(g, ratio * selec, 0, ratio, rect->height);
                egraphics_fill(g);
            }
        }
        ebox_end_layer((t_ebox*)x, cream_sym_selection_layer);
    }
    ebox_paint_layer((t_ebox *)x, cream_sym_selection_layer,  0., 0.);
}

static void draw_text(t_tab *x, t_object *view, t_rect *rect)
{
    int i;
    t_elayer *g = ebox_start_layer((t_ebox *)x, cream_sym_text_layer, rect->width, rect->height);
	if(g)
	{
        t_etext *jtl = etext_layout_create();
        if(jtl)
        {
            if(x->f_orientation)
            {
                const float ratio =  rect->height / (float)x->f_nitems;
                for(i = 0; i < x->f_nitems; i++)
                {
                    if(x->f_items[i])
                    {
                        etext_layout_set(jtl, x->f_items[i]->s_name, &x->f_font,
                                         0.f, ratio * (i + 0.5f) + (i ? 2.f : 0.f), rect->width, 0.f, ETEXT_CENTRED, ETEXT_NOWRAP);
                        etext_layout_draw(jtl, g);
                    }
                }
            }
            else
            {
                const float ratio = rect->width / (float)x->f_nitems;
                for(i = 0; i < x->f_nitems; i++)
                {
                    if(x->f_items[i])
                    {
                        etext_layout_settextcolor(jtl, &x->f_color_text);
                        etext_layout_set(jtl, x->f_items[i]->s_name, &x->f_font,
                                         ratio * i, rect->height * 0.5f, ratio - 2.f, 0.f, ETEXT_CENTRED, ETEXT_WRAP);
                        etext_layout_draw(jtl, g);
                    }
                }
            }
            etext_layout_destroy(jtl);
        }
        ebox_end_layer((t_ebox*)x, cream_sym_text_layer);
	}
	ebox_paint_layer((t_ebox *)x, cream_sym_text_layer, 0., 0.);
}

static void tab_paint(t_tab *x, t_object *view)
{
    t_rect rect;
    ebox_get_rect_for_view((t_ebox *)x, &rect);
    
    draw_selection(x, view, &rect);
    draw_background(x, view, &rect);
    draw_text(x, view, &rect);
}

static void tab_mousedown(t_tab *x, t_object *patcherview, t_pt pt, long modifiers)
{
    t_rect rect;
    int index;
    if(x->f_nitems)
    {
        x->f_off = 0;
        ebox_get_rect_for_view((t_ebox *)x, &rect);
        index = x->f_orientation ? (pt.y / (rect.height / (float)x->f_nitems)) : (pt.x / (rect.width / (float)x->f_nitems));
        ebox_parameter_setvalue((t_ebox *)x, 1, index, 1);
        tab_output(x);
        ebox_invalidate_layer((t_ebox *)x, cream_sym_selection_layer);
        ebox_redraw((t_ebox *)x);
    }
}

static void tab_mouseup(t_tab *x, t_object *patcherview, t_pt pt, long modifiers)
{
    if(!x->f_toggle)
    {
        x->f_off = 1;
        ebox_invalidate_layer((t_ebox *)x, cream_sym_selection_layer);
        ebox_invalidate_layer((t_ebox *)x, cream_sym_text_layer);
        ebox_redraw((t_ebox *)x);
    }
}

static void tab_mouseleave(t_tab *x, t_object *patcherview, t_pt pt, long modifiers)
{
    if(x->f_nitems)
    {
        x->f_item_hover = -1;
        outlet_float(x->f_out_hover, x->f_item_hover);
        ebox_invalidate_layer((t_ebox *)x, cream_sym_selection_layer);
        ebox_redraw((t_ebox *)x);
    }
}

static void tab_mousemove(t_tab *x, t_object *patcherview, t_pt pt, long modifiers)
{
    t_rect rect;
    int index;
    if(x->f_nitems)
    {
        ebox_get_rect_for_view((t_ebox *)x, &rect);
        index = x->f_orientation ? (pt.y / (rect.height / (float)x->f_nitems)) : (pt.x / (rect.width / (float)x->f_nitems));
        x->f_item_hover = pd_clip(index, 0, x->f_nitems - 1);
        
        outlet_float(x->f_out_hover, x->f_item_hover);
        ebox_invalidate_layer((t_ebox *)x, cream_sym_selection_layer);
        ebox_redraw((t_ebox *)x);
    }
}

static t_pd_err tab_items_set(t_tab *x, t_object *attr, int ac, t_atom *av)
{
    int i;
    char text[MAXPDSTRING];
    if(ac && av)
    {
        for(i = 0; i < ac; i++)
        {
            atom_string(av+i, text, MAXPDSTRING);
            x->f_items[i] = gensym(text);
        }
    }
    ebox_parameter_setvalue((t_ebox *)x, 1, 0.f, 1);
    ebox_parameter_setminmax((t_ebox *)x, 1, 0, ac ? (float)(ac - 1) : 0.f);
    ebox_parameter_setnstep((t_ebox *)x, 1, ac);
    x->f_nitems = ac;
    ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
    ebox_redraw((t_ebox *)x);
    return 0;
}

static void tab_setter_t(t_tab *x, int index, char const* text)
{
    int i;
    t_symbol* s = gensym(text);
    for(i = 0; i < x->f_nitems; i++)
    {
        if(x->f_items[i] == s)
        {
            ebox_parameter_setvalue((t_ebox *)x, 1, (float)i, 0);
            tab_output(x);
            ebox_invalidate_layer((t_ebox *)x, cream_sym_items_layer);
            ebox_redraw((t_ebox *)x);
        }
    }
}

static void tab_getter_t(t_tab *x, int index, char* text)
{
    const int _index = (int)ebox_parameter_getvalue((t_ebox *)x, 1);
    sprintf(text, "%s", x->f_items[_index]->s_name);
}

static void *tab_new(t_symbol *s, int argc, t_atom *argv)
{
    t_tab *x = (t_tab *)eobj_new(tab_class);
    t_binbuf* d = binbuf_via_atoms(argc,argv);
    
    if(x && d)
    {
        ebox_new((t_ebox *)x, 0 | EBOX_GROWINDI | EBOX_FONTSIZE);
        ebox_parameter_create((t_ebox *)x, 1);
        ebox_parameter_setsettergetter_text((t_ebox *)x, 1,
                                            (t_param_setter_t)tab_setter_t,
                                            (t_param_getter_t)tab_getter_t);
        x->f_out_index  = outlet_new((t_object *)x, &s_float);
        x->f_out_item   = outlet_new((t_object *)x, &s_list);
        x->f_out_hover  = outlet_new((t_object *)x, &s_float);
        
        x->f_item_hover = -1;
        x->f_off        = 1;
        ebox_attrprocess_viabinbuf(x, d);
        ebox_ready((t_ebox *)x);
    }
    
    return (x);
}

extern "C" void setup_c0x2etab(void)
{
    t_eclass *c = eclass_new("c.tab", (method)tab_new, (method)ebox_free, (short)sizeof(t_tab), 0L, A_GIMME, 0);
    
    if(c)
    {
        eclass_guiinit(c, 0);
        eclass_addmethod(c, (method) tab_paint,             "paint",            A_NULL, 0);
        eclass_addmethod(c, (method) tab_notify,            "notify",           A_NULL, 0);
        eclass_addmethod(c, (method) tab_getdrawparams,     "getdrawparams",    A_NULL, 0);
        eclass_addmethod(c, (method) tab_oksize,            "oksize",           A_NULL, 0);
        
        eclass_addmethod(c, (method) tab_append,            "append",           A_GIMME,0);
        eclass_addmethod(c, (method) tab_insert,            "insert",           A_GIMME,0);
        eclass_addmethod(c, (method) tab_setitem,           "setitem",          A_GIMME,0);
        eclass_addmethod(c, (method) tab_delete,            "delete",           A_FLOAT,0);
        eclass_addmethod(c, (method) tab_clear,             "clear",            A_NULL,0);
        
        eclass_addmethod(c, (method) tab_float,             "float",            A_FLOAT,0);
        eclass_addmethod(c, (method) tab_anything,          "anything",         A_GIMME,0);
        eclass_addmethod(c, (method) tab_set,               "set",              A_GIMME,0);
        eclass_addmethod(c, (method) tab_setsymbol,         "setsymbol",        A_GIMME,0);
        eclass_addmethod(c, (method) tab_bang,              "bang",             A_NULL, 0);
        eclass_addmethod(c, (method) tab_next,              "next",             A_NULL, 0);
        eclass_addmethod(c, (method) tab_prev,              "prev",             A_NULL, 0);
        
        eclass_addmethod(c, (method) tab_mousedown,         "mousedown",       A_NULL, 0);
        eclass_addmethod(c, (method) tab_mouseup,           "mouseup",         A_NULL, 0);
        eclass_addmethod(c, (method) tab_mousemove,         "mousemove",       A_NULL, 0);
        eclass_addmethod(c, (method) tab_mouseleave,        "mouseleave",      A_NULL, 0);
        
        CLASS_ATTR_DEFAULT              (c, "size", 0, "100 13");
        
        CLASS_ATTR_CHAR                 (c, "orientation", 0, t_tab, f_orientation);
        CLASS_ATTR_LABEL                (c, "orientation", 0, "Vertical Orientation");
        CLASS_ATTR_ORDER                (c, "orientation", 0, "1");
        CLASS_ATTR_FILTER_CLIP          (c, "orientation", 0, 1);
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "orientation", 0, "0");
        CLASS_ATTR_STYLE                (c, "orientation", 0, "onoff");
        
        CLASS_ATTR_CHAR                 (c, "toggle", 0, t_tab, f_toggle);
        CLASS_ATTR_LABEL                (c, "toggle", 0, "Toggle Mode");
        CLASS_ATTR_ORDER                (c, "toggle", 0, "1");
        CLASS_ATTR_FILTER_CLIP          (c, "toggle", 0, 1);
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "toggle", 0, "0");
        CLASS_ATTR_STYLE                (c, "toggle", 0, "onoff");
        
        CLASS_ATTR_SYMBOL_VARSIZE       (c, "items", 0, t_tab, f_items, f_nitems, CREAM_MAXITEMS);
        CLASS_ATTR_LABEL                (c, "items", 0, "Items");
        CLASS_ATTR_ORDER                (c, "items", 0, "1");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "items", 0, "");
        CLASS_ATTR_ACCESSORS            (c, "items", NULL, tab_items_set);
        
        CLASS_ATTR_FONT                 (c, "font", 0, t_tab, f_font);
        CLASS_ATTR_LABEL                (c, "font", 0, "Font");
        CLASS_ATTR_SAVE                 (c, "font", 0);
        CLASS_ATTR_PAINT                (c, "font", 0);
        
        CLASS_ATTR_RGBA                 (c, "bgcolor", 0, t_tab, f_color_background);
        CLASS_ATTR_LABEL                (c, "bgcolor", 0, "Background Color");
        CLASS_ATTR_ORDER                (c, "bgcolor", 0, "1");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bgcolor", 0, "0.75 0.75 0.75 1.");
        CLASS_ATTR_STYLE                (c, "bgcolor", 0, "color");
        
        CLASS_ATTR_RGBA                 (c, "bdcolor", 0, t_tab, f_color_border);
        CLASS_ATTR_LABEL                (c, "bdcolor", 0, "Border Color");
        CLASS_ATTR_ORDER                (c, "bdcolor", 0, "2");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bdcolor", 0, "0.5 0.5 0.5 1.");
        CLASS_ATTR_STYLE                (c, "bdcolor", 0, "color");
        
        CLASS_ATTR_RGBA                 (c, "textcolor", 0, t_tab, f_color_text);
        CLASS_ATTR_LABEL                (c, "textcolor", 0, "Text Color");
        CLASS_ATTR_ORDER                (c, "textcolor", 0, "3");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "textcolor", 0, "0. 0. 0. 1.");
        CLASS_ATTR_STYLE                (c, "textcolor", 0, "color");
        
        CLASS_ATTR_RGBA                 (c, "hocolor", 0, t_tab, f_color_hover);
        CLASS_ATTR_LABEL                (c, "hocolor", 0, "Hover Color");
        CLASS_ATTR_ORDER                (c, "hocolor", 0, "4");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "hocolor", 0, "0.5 0.5 0.5 1.");
        CLASS_ATTR_STYLE                (c, "hocolor", 0, "color");
        
        CLASS_ATTR_RGBA                 (c, "secolor", 0, t_tab, f_color_select);
        CLASS_ATTR_LABEL                (c, "secolor", 0, "Selection Color");
        CLASS_ATTR_ORDER                (c, "secolor", 0, "5");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "secolor", 0, "0.35 0.35 0.35 1.");
        CLASS_ATTR_STYLE                (c, "secolor", 0, "color");
        
        eclass_register(CLASS_BOX, c);
        tab_class = c;
        
    }
}

