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
    t_clock*    f_clock;
    
    t_symbol*   f_items[CREAM_MAXITEMS];
    long        f_nitems;
    int         f_item_hover;

    char        f_toggle;
    char        f_orientation;
    
    t_efont     f_font;
	t_rgba		f_color_background;
	t_rgba		f_color_border;
	t_rgba		f_color_text;
    t_rgba		f_color_hover;
    t_rgba		f_color_select;
} t_tab;

static t_eclass *tab_class;

static t_symbol* tab_parseatoms(t_symbol* s, int argc, t_atom* argv)
{
    char buffer[MAXPDSTRING], temp[128];
    const t_symbol* sym;
    int lenght, buflen, i, j, k;
    memset(buffer, '\0', MAXPDSTRING * sizeof(char));
    if(is_valid_symbol(s))
    {
        sprintf(buffer, "%s", s->s_name);
    }
    for(i = 0; i < argc; i++)
    {
        if(atom_gettype(argv+i) == A_SYMBOL) // To clean
        {
            sym = atom_getsymbol(argv+i);
            lenght = (int)strlen(s->s_name);
            buflen = (int)strlen(buffer);
            if(buflen)
            {
                buffer[buflen++] = ' ';
            }
            for(j = 0, k = buflen; j < lenght && k < MAXPDSTRING - 1; j++)
            {
                if(s->s_name[j] != '"' && s->s_name[j] != '\'')
                {
                    buffer[k++] = s->s_name[j];
                }
            }
            buffer[MAXPDSTRING - 1] = '\0';
        }
        else if(atom_gettype(argv+i) == A_FLOAT)
        {
            sprintf(temp, "%g", atom_getfloat(argv+i));
            strncat(buffer, temp, (size_t)lenght);
        }
    }
    return gensym(buffer);
}

static int tab_getindex(t_tab *x, t_symbol* s)
{
    if(s)
    {
        for(int i = 0; i < x->f_nitems; i++)
        {
            if(s == x->f_items[i])
            {
                return i;
            }
        }
    }
    return -1;
}

static void tab_append(t_tab *x, t_symbol *s, int argc, t_atom *argv)
{
    int i;
    t_atom* av = NULL;
    if(argc && argv)
    {
        av = (t_atom *)malloc((size_t)x->f_nitems + 1);
        if(av)
        {
            for(i = 0; i < x->f_nitems; i++)
            {
                atom_setsym(av+i, x->f_items[i]);
            }
            atom_setsym(av+x->f_nitems, tab_parseatoms(NULL, argc, argv));
            pd_typedmess((t_pd *)x, cream_sym_items, (int)x->f_nitems + 1, av);
            free(av);
        }
    }
}

static void tab_insert(t_tab *x, t_symbol *s, int argc, t_atom *argv)
{
    /*
    int i;
    t_atom* av = NULL;
    if(argc > 1 && argv && atom_gettype(argv) == A_FLOAT)
    {
        const int index = atom_getfloat(argv);
        if(index >= 0 && index < x->f_nitems && index < CREAM_MAXITEMS)
        {
            t_symbol* item = tab_parseatoms(argc - 1, argv + 1);
            if(item && tab_getindex(x, item) == -1)
            {
                for(int i = (int)x->f_nitems; i > index; i--)
                {
                    x->f_items[i] = x->f_items[i-1];
                }
                x->f_items[index] = item;
                x->f_nitems++;
                
                ebox_invalidate_layer((t_ebox *)x, cream_sym_text_layer);
                ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
                ebox_invalidate_layer((t_ebox *)x, cream_sym_selection_layer);
                ebox_redraw((t_ebox *)x);
            }
        }
        else if(index >= 0)
        {
            t_symbol* item = tab_parseatoms(argc - 1, argv + 1);
            if(item && tab_getindex(x, item) == -1)
            {
                x->f_items[x->f_nitems] = item;
                x->f_nitems++;
                
                ebox_invalidate_layer((t_ebox *)x, cream_sym_text_layer);
                ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
                ebox_invalidate_layer((t_ebox *)x, cream_sym_selection_layer);
                ebox_redraw((t_ebox *)x);
            }
        }
    }
     */
}

static void tab_setitem(t_tab *x, t_symbol *s, int argc, t_atom *argv)
{
    /*
    if(argc > 1 && argv && atom_gettype(argv) == A_FLOAT)
    {
        const int index = atom_getfloat(argv);
        if(index >= 0 && index < x->f_nitems)
        {
            t_symbol* item = tab_parseatoms(argc - 1, argv + 1);
            if(item && tab_getindex(x, item) == -1)
            {
                x->f_items[index] = item;
                ebox_invalidate_layer((t_ebox *)x, cream_sym_text_layer);
                ebox_invalidate_layer((t_ebox *)x, cream_sym_selection_layer);
                ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
                ebox_redraw((t_ebox *)x);
            }
        }
    }
     */
}

static void tab_delete(t_tab *x, t_symbol *s, int argc, t_atom *argv)
{
    /*
    if(argc && argv)
    {
        if(atom_gettype(argv) == A_FLOAT)
        {
            const int index = (int)atom_getfloat(argv);
            if(index >= 0 && index < x->f_nitems)
            {
                for(int i = index; i < x->f_nitems - 1; i++)
                {
                    x->f_items[i] = x->f_items[i+1];
                }
                x->f_nitems--;
                
                ebox_invalidate_layer((t_ebox *)x, cream_sym_text_layer);
                ebox_invalidate_layer((t_ebox *)x, cream_sym_selection_layer);
                ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
                ebox_redraw((t_ebox *)x);
            }
        }
        else
        {
            const int index = tab_getindex(x, tab_parseatoms(argc, argv));
            if(index >= 0 && index < x->f_nitems)
            {
                for(int i = index; i < x->f_nitems - 1; i++)
                {
                    x->f_items[i] = x->f_items[i+1];
                }
                x->f_nitems--;
                
                ebox_invalidate_layer((t_ebox *)x, cream_sym_text_layer);
                ebox_invalidate_layer((t_ebox *)x, cream_sym_selection_layer);
                ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
                ebox_redraw((t_ebox *)x);
            }
        }
    }
     */
}

static void tab_clear(t_tab *x, t_symbol *s, int argc, t_atom *argv)
{
    pd_typedmess((t_pd *)x, cream_sym_items, 0, NULL);
}

static void tab_output(t_tab *x)
{
    t_pd* send = ebox_getsender((t_ebox *) x);
    const float val = ebox_parameter_getvalue((t_ebox *)x, 1);
    outlet_float(x->f_out_index, val);
    if(send)
    {
        pd_float(send, val);
    }
}

static void tab_symbol(t_tab *x, t_symbol *s, int argc, t_atom *argv)
{
    int index;
    if(x->f_nitems && argc && argv)
    {
        index = tab_getindex(x, tab_parseatoms(s, argc, argv));
        index = (index < 0 || index > x->f_nitems - 1) ? -1 : index;
        ebox_parameter_setvalue((t_ebox *)x, 1, index, 0);
        tab_output(x);
        ebox_invalidate_layer((t_ebox *)x, cream_sym_selection_layer);
        ebox_redraw((t_ebox *)x);
        if(!x->f_toggle)
        {
            clock_delay(x->f_clock, 20.);
        }
    }
}

static void tab_float(t_tab *x, t_floatarg f)
{
    const int index = ((int)f < 0 || (int)f > x->f_nitems - 1) ? -1 : (int)f;
    if(x->f_nitems)
    {
        ebox_parameter_setvalue((t_ebox *)x, 1, index, 0);
        tab_output(x);
        ebox_invalidate_layer((t_ebox *)x, cream_sym_selection_layer);
        ebox_redraw((t_ebox *)x);
        if(!x->f_toggle)
        {
            clock_delay(x->f_clock, 20.);
        }
    }
}

static void tab_set(t_tab *x, t_symbol *s, int argc, t_atom *argv)
{
    int index = -1;
    if(x->f_nitems && argc && argv)
    {
        if(atom_gettype(argv) == A_FLOAT)
        {
            index = (int)atom_getfloat(argv);
        }
        else if(atom_gettype(argv) == A_SYMBOL)
        {
            index = tab_getindex(x, tab_parseatoms(NULL, argc, argv));
        }
        index = (index < 0 || index > x->f_nitems - 1) ? -1 : index;
        ebox_parameter_setvalue((t_ebox *)x, 1, index, 0);
        ebox_invalidate_layer((t_ebox *)x, cream_sym_selection_layer);
        ebox_redraw((t_ebox *)x);
        if(!x->f_toggle)
        {
            clock_delay(x->f_clock, 20.);
        }
    }
}

static void tab_clock(t_tab *x, t_symbol *s, int argc, t_atom *argv)
{
    const int index = ebox_parameter_getvalue((t_ebox *)x, 1);
    if(index != -1)
    {
        ebox_parameter_setvalue((t_ebox *)x, 1, -1, 0);
        ebox_invalidate_layer((t_ebox *)x, cream_sym_selection_layer);
        ebox_redraw((t_ebox *)x);
    }
}

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
           s == cream_sym_fontsize ||
           s == cream_sym_fontname ||
           s == cream_sym_fontweight ||
           s == cream_sym_fontslant)
		{
            ebox_invalidate_layer((t_ebox *)x, cream_sym_selection_layer);
            ebox_invalidate_layer((t_ebox *)x, cream_sym_text_layer);
			ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
		}
        else if(s == cream_sym_fontsize ||
                s == cream_sym_orientation ||
                s == cream_sym_items)
        {
            ebox_notify((t_ebox *)x, s_cream_size, cream_sym_attr_modified, NULL, NULL);
        }
        // Change the size /enable state /etc...
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
            if(selec != -1)
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
            if(selec != -1)
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
                                         rect->width * 0.5,
                                         ratio * (i + 0.5),
                                         rect->width,
                                         0, ETEXT_CENTRED, ETEXT_NOWRAP);
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
                                         ratio * i,
                                         rect->height * 0.5,
                                         ratio - 2,
                                         0, ETEXT_CENTRED, ETEXT_WRAP);
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
        ebox_get_rect_for_view((t_ebox *)x, &rect);
        index = x->f_orientation ? (pt.y / (rect.height / (float)x->f_nitems)) : (pt.x / (rect.width / (float)x->f_nitems));
        ebox_parameter_setvalue((t_ebox *)x, 1, index, 0);
        tab_output(x);
        ebox_invalidate_layer((t_ebox *)x, cream_sym_selection_layer);
        ebox_redraw((t_ebox *)x);
        if(!x->f_toggle)
        {
            clock_delay(x->f_clock, 20.);
        }
    }
}

static void tab_mouseup(t_tab *x, t_object *patcherview, t_pt pt, long modifiers)
{
    if(!x->f_toggle)
    {
        ebox_parameter_setvalue((t_ebox *)x, 1, 0, 0);
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

static void tab_free(t_tab *x)
{
    clock_free(x->f_clock);
    ebox_free((t_ebox *)x);
}

static void *tab_new(t_symbol *s, int argc, t_atom *argv)
{
    t_tab *x = (t_tab *)eobj_new(tab_class);
    t_binbuf* d = binbuf_via_atoms(argc,argv);
    
    if(x && d)
    {
        ebox_new((t_ebox *)x, 0 | EBOX_GROWINDI);
        ebox_parameter_create((t_ebox *)x, 1);
        ebox_parameter_setflags((t_ebox *)x, 1, 0 | EPARAM_STATIC_MIN | EPARAM_STATIC_MAX |
                                EPARAM_STATIC_INVERTED | EPARAM_STATIC_NSTEPS);
        x->f_out_index  = outlet_new((t_object *)x, &s_float);
        x->f_out_item   = outlet_new((t_object *)x, &s_list);
        x->f_out_hover  = outlet_new((t_object *)x, &s_float);
        x->f_clock      = clock_new(x, (t_method)tab_clock);
        x->f_item_hover    = -1;
        x->f_nitems = 0;
        ebox_attrprocess_viabinbuf(x, d);
        ebox_ready((t_ebox *)x);
    }
    
    return (x);
}

extern "C" void setup_c0x2etab(void)
{
    t_eclass *c = eclass_new("c.tab", (method)tab_new, (method)tab_free, (short)sizeof(t_tab), 0L, A_GIMME, 0);
    
    if(c)
    {
        eclass_guiinit(c, 0);
        eclass_addmethod(c, (method) tab_paint,           "paint",            A_NULL, 0);
        eclass_addmethod(c, (method) tab_notify,          "notify",           A_NULL, 0);
        eclass_addmethod(c, (method) tab_getdrawparams,   "getdrawparams",    A_NULL, 0);
        eclass_addmethod(c, (method) tab_oksize,          "oksize",           A_NULL, 0);
        
        eclass_addmethod(c, (method) tab_append,          "append",           A_GIMME,0);
        eclass_addmethod(c, (method) tab_insert,          "insert",           A_GIMME,0);
        eclass_addmethod(c, (method) tab_setitem,         "setitem",          A_GIMME,0);
        eclass_addmethod(c, (method) tab_delete,          "delete",           A_GIMME,0);
        eclass_addmethod(c, (method) tab_clear,           "clear",            A_GIMME,0);
        
        eclass_addmethod(c, (method) tab_float,           "float",            A_FLOAT,0);
        eclass_addmethod(c, (method) tab_symbol,          "anything",         A_GIMME,0);
        eclass_addmethod(c, (method) tab_set,             "set",              A_GIMME,0);
        eclass_addmethod(c, (method) tab_output,          "bang",             A_NULL, 0);
        
        eclass_addmethod(c, (method) tab_mousedown,        "mousedown",       A_NULL, 0);
        eclass_addmethod(c, (method) tab_mouseup,          "mouseup",         A_NULL, 0);
        eclass_addmethod(c, (method) tab_mousemove,        "mousemove",       A_NULL, 0);
        eclass_addmethod(c, (method) tab_mouseleave,       "mouseleave",      A_NULL, 0);
        
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

