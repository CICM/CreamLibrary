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

typedef struct  _menu
{
	t_ebox      j_box;
    t_outlet*   f_out_index;
    t_outlet*   f_out_item;
    t_outlet*   f_out_infos;
    t_epopup*   f_popup;
    t_symbol*   f_items[CREAM_MAXITEMS];
    char        f_states[CREAM_MAXITEMS];
    long        f_nitems;
    long        f_hover;
    
    t_efont     f_font;
	t_rgba		f_color_background;
	t_rgba		f_color_border;
	t_rgba		f_color_text;
} t_menu;

static t_eclass *menu_class;

static t_symbol* menu_gensym(t_symbol* s, int argc, t_atom* argv)
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

static void menu_output(t_menu *x)
{
    if(x->f_nitems)
    {
        t_pd* send = ebox_getsender((t_ebox *) x);
        const float index = ebox_parameter_getvalue((t_ebox *)x, 1);
        outlet_float(x->f_out_index, index);
        outlet_symbol(x->f_out_item, x->f_items[(int)index]);
        if(send)
        {
            pd_float(ebox_getsender((t_ebox *) x), index);
        }
    }
}

static void menu_count(t_menu *x)
{
    t_atom av;
    atom_setfloat(&av, (float)x->f_nitems);
    outlet_anything(x->f_out_infos, gensym("count"), 1, &av);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//                                          Edition                                         //
//////////////////////////////////////////////////////////////////////////////////////////////

static void menu_append(t_menu *x, t_symbol *s, int argc, t_atom *argv)
{
    t_symbol* item = menu_gensym(NULL, argc, argv);
    if(x->f_nitems < CREAM_MAXITEMS - 1 && item)
    {
        x->f_items[x->f_nitems] = item;
        x->f_nitems++;
        ebox_parameter_setminmax((t_ebox *)x, 1, 0.f, (float)x->f_nitems - 1.f);
        ebox_parameter_setnstep((t_ebox *)x, 1, (int)x->f_nitems);
    }
}

static void menu_insert(t_menu *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, index;
    t_symbol* item;
    if(argc && argv && atom_gettype(argv) == A_FLOAT)
    {
        index   = (int)pd_clip(atom_getfloat(argv), 0.f, (float)x->f_nitems);
        item    = menu_gensym(NULL, argc-1, argv+1);
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
            ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
            ebox_redraw((t_ebox *)x);
        }
    }
}

static void menu_setitem(t_menu *x, t_symbol *s, int argc, t_atom *argv)
{
    int index;
    t_symbol* item;
    if(argc && argv && atom_gettype(argv) == A_FLOAT)
    {
        index   = (int)pd_clip(atom_getfloat(argv), 0.f, (float)x->f_nitems - 1.f);
        item    = menu_gensym(NULL, argc-1, argv+1);
        if(item)
        {
            x->f_items[index] = item;
            ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
            ebox_redraw((t_ebox *)x);
        }
    }
}

static void menu_delete(t_menu *x, float f)
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
        ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
        ebox_redraw((t_ebox *)x);
    }
}

static void menu_clear(t_menu *x)
{
    x->f_nitems = 0;
    ebox_parameter_setminmax((t_ebox *)x, 1, 0.f, 0.f);
    ebox_parameter_setnstep((t_ebox *)x, 1, 1);
    ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
    ebox_redraw((t_ebox *)x);
}

static void menu_state(t_menu *x, t_symbol *s, int argc, t_atom *argv)
{
    if(argc > 1 && argv && atom_gettype(argv) == A_FLOAT && atom_gettype(argv+1) == A_FLOAT)
    {
        const int index = atom_getfloat(argv);
        if(index >= 0 && index < x->f_nitems)
        {
            x->f_states[index] = atom_getfloat(argv+1) != 0.f ? 1 : 0;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
//                                          Selection                                       //
//////////////////////////////////////////////////////////////////////////////////////////////

static void menu_bang(t_menu *x)
{
    if(x->f_nitems)
    {
        ebox_parameter_notify_changes((t_ebox *)x, 1);
        menu_output(x);
    }
}

static void menu_next(t_menu *x)
{
    const float index = ebox_parameter_getvalue((t_ebox *)x, 1);
    ebox_parameter_setvalue((t_ebox *)x, 1, index+1, 1);
    menu_output(x);
    ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
    ebox_redraw((t_ebox *)x);
}

static void menu_prev(t_menu *x)
{
    const float index = ebox_parameter_getvalue((t_ebox *)x, 1);
    ebox_parameter_setvalue((t_ebox *)x, 1, index-1, 1);
    menu_output(x);
    ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
    ebox_redraw((t_ebox *)x);
}

static void menu_float(t_menu *x, t_floatarg f)
{
    ebox_parameter_setvalue((t_ebox *)x, 1, f, 1);
    menu_output(x);
    ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
    ebox_redraw((t_ebox *)x);
}

static void menu_anything(t_menu *x, t_symbol *s, int argc, t_atom *argv)
{
    int i;
    t_symbol* item = menu_gensym(s, argc, argv);
    if(item)
    {
        for(i = 0; i < x->f_nitems; i++)
        {
            if(x->f_items[i] == item)
            {
                ebox_parameter_setvalue((t_ebox *)x, 1, (float)i, 1);
                menu_output(x);
                ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
                ebox_redraw((t_ebox *)x);
                return;
            }
        }
    }
}

static void menu_set(t_menu *x, t_symbol *s, int argc, t_atom *argv)
{
    int i;
    t_symbol* item;
    if(argc && argv && atom_gettype(argv) == A_FLOAT)
    {
        ebox_parameter_setvalue((t_ebox *)x, 1, atom_getfloat(argv), 0);
        menu_output(x);
        ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
        ebox_redraw((t_ebox *)x);
    }
    else if(argc && argv && atom_gettype(argv) == A_SYMBOL)
    {
        item = menu_gensym(NULL, argc, argv);
        if(item)
        {
            for(i = 0; i < x->f_nitems; i++)
            {
                if(x->f_items[i] == item)
                {
                    ebox_parameter_setvalue((t_ebox *)x, 1, (float)i, 1);
                    menu_output(x);
                    ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
                    ebox_redraw((t_ebox *)x);
                    return;
                }
            }
        }
    }
}

static void menu_setsymbol(t_menu *x, t_symbol *s, int argc, t_atom *argv)
{
    int i;
    t_symbol* item = menu_gensym(NULL, argc, argv);
    if(item)
    {
        for(i = 0; i < x->f_nitems; i++)
        {
            if(x->f_items[i] == item)
            {
                ebox_parameter_setvalue((t_ebox *)x, 1, (float)i, 1);
                menu_output(x);
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

static void menu_getdrawparams(t_menu *x, t_object *patcherview, t_edrawparams *params)
{
	params->d_borderthickness   = 2;
	params->d_cornersize        = 2;
    params->d_bordercolor       = x->f_color_border;
    params->d_boxfillcolor      = x->f_color_background;
}

static void menu_oksize(t_menu *x, t_rect *newrect)
{
    newrect->width = pd_clip_min(newrect->width, x->f_font.size * 3 + 8);
    newrect->height = newrect->height = x->f_font.size + 4;
}

static t_pd_err menu_notify(t_menu *x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
	if (msg == cream_sym_attr_modified)
	{
		if(s == cream_sym_bgcolor || s == cream_sym_bdcolor || s == cream_sym_textcolor || s == cream_sym_font)
		{
			ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
		}
	}
    else if(msg == cream_sym_value_changed)
    {
        menu_output(x);
        ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
        ebox_redraw((t_ebox *)x);
    }
	return 0;
}

static void menu_paint(t_menu *x, t_object *view)
{
	t_rect rect;
	ebox_get_rect_for_view((t_ebox *)x, &rect);
    t_elayer *g = ebox_start_layer((t_ebox *)x, cream_sym_background_layer, rect.width, rect.height);
    if(g)
    {
        const int index = (int)ebox_parameter_getvalue((t_ebox *)x, 1);
        if(x->f_nitems && index < x->f_nitems)
        {
            t_etext *jtl = etext_layout_create();
            if(jtl)
            {
                etext_layout_set(jtl, x->f_items[index]->s_name, &x->f_font,
                             2.f, 0.f, rect.width - rect.height - 2.f, rect.height, ETEXT_CENTREDLEFT, ETEXT_NOWRAP);
                etext_layout_settextcolor(jtl, &x->f_color_text);
                etext_layout_draw(jtl, g);
                etext_layout_destroy(jtl);
            }
        }
        
        egraphics_set_color_rgba(g, &x->f_color_border);
        egraphics_set_line_width(g, 2);
        egraphics_line_fast(g, rect.width - rect.height, 0., rect.width - rect.height, rect.height);
        
        egraphics_move_to(g, rect.width - rect.height + 3.f, rect.height * 0.5f - 2.f);
        egraphics_line_to(g, rect.width - 2.f, rect.height * 0.5f - 2.f);
        egraphics_line_to(g, rect.width - rect.height * 0.5f + 1.f, 2.f);
        egraphics_close_path(g);
        egraphics_fill(g);
        
        egraphics_move_to(g, rect.width - rect.height + 3.f, rect.height * 0.5f + 2.f);
        egraphics_line_to(g, rect.width - 2.f, rect.height * 0.5f + 2.f);
        egraphics_line_to(g, rect.width - rect.height * 0.5f + 1.f, rect.height - 2.f);
        egraphics_close_path(g);
        egraphics_fill(g);
        
        ebox_end_layer((t_ebox*)x, cream_sym_background_layer);
    }
    ebox_paint_layer((t_ebox *)x, cream_sym_background_layer,  0., 0.);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//                                          Interactions                                    //
//////////////////////////////////////////////////////////////////////////////////////////////

static void menu_popup(t_menu *x, t_epopup *popup, long itemid)
{
    if(x->f_popup && popup == x->f_popup)
    {
        ebox_parameter_setvalue((t_ebox *)x, 1, (float)itemid, 1);
        menu_output(x);
        ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
        ebox_redraw((t_ebox *)x);
        epopupmenu_destroy(x->f_popup);
        x->f_popup = NULL;
    }
}

static void menu_mousedown(t_menu *x, t_object *patcherview, t_pt pt, long modifiers)
{
    if(!x->f_hover)
    {
        if(x->f_popup)
        {
            epopupmenu_destroy(x->f_popup);
            x->f_popup = NULL;
        }
        x->f_popup = epopupmenu_create((t_eobj  *)x);
        if(x->f_popup)
        {
            t_rect rect;
            ebox_get_rect_for_view((t_ebox *)x, &rect);
            const int index = (int)ebox_parameter_getvalue((t_ebox *)x, 1);
            for(long i = 0; i < x->f_nitems; i++)
            {
                epopupmenu_additem(x->f_popup, (int)i, x->f_items[i]->s_name, (char)(index == (int)i), x->f_states[i]);
            }
            epopupmenu_setfont(x->f_popup, &x->f_font);
            epopupmenu_setbackgroundcolor(x->f_popup, &x->f_color_background);
            epopupmenu_settextcolor(x->f_popup, &x->f_color_text);
            epopupmenu_popup(x->f_popup, &rect);
        }
    }
}

static void menu_mouseenter(t_menu *x, t_object *patcherview, t_pt pt, long modifiers)
{
    if(x->f_hover)
    {
        if(x->f_popup)
        {
            epopupmenu_destroy(x->f_popup);
            x->f_popup = NULL;
        }
        x->f_popup = epopupmenu_create((t_eobj  *)x);
        if(x->f_popup)
        {
            t_rect rect;
            ebox_get_rect_for_view((t_ebox *)x, &rect);
            const int index = (int)ebox_parameter_getvalue((t_ebox *)x, 1);
            for(long i = 0; i < x->f_nitems; i++)
            {
                epopupmenu_additem(x->f_popup, (int)i, x->f_items[i]->s_name, (char)(index == (int)i), x->f_states[i]);
            }
            epopupmenu_setfont(x->f_popup, &x->f_font);
            epopupmenu_setbackgroundcolor(x->f_popup, &x->f_color_background);
            epopupmenu_settextcolor(x->f_popup, &x->f_color_text);
            epopupmenu_popup(x->f_popup, &rect);
        }
    }
}

static t_pd_err menu_states_set(t_menu *x, t_object *attr, int ac, t_atom *av)
{
    int i;
    if(ac && av)
    {
        for(i = 0; i < x->f_nitems; i++)
        {
            if(i < ac && atom_gettype(av+i) == A_FLOAT && atom_getfloat(av+i) != 0)
                x->f_states[i] = 1;
            else
                x->f_states[i] = 0;
        }
    }
    return 0;
}

static t_pd_err menu_items_set(t_menu *x, t_object *attr, int ac, t_atom *av)
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

static void menu_setter_t(t_menu *x, int index, char const* text)
{
    int i;
    t_symbol* s = gensym(text);
    for(i = 0; i < x->f_nitems; i++)
    {
        if(x->f_items[i] == s)
        {
            ebox_parameter_setvalue((t_ebox *)x, 1, (float)i, 0);
            menu_output(x);
            ebox_invalidate_layer((t_ebox *)x, cream_sym_items_layer);
            ebox_redraw((t_ebox *)x);
        }
    }
}

static void menu_getter_t(t_menu *x, int index, char* text)
{
    const int _index = (int)ebox_parameter_getvalue((t_ebox *)x, 1);
    sprintf(text, "%s", x->f_items[_index]->s_name);
}

static void *menu_new(t_symbol *s, int argc, t_atom *argv)
{
    t_menu *x= (t_menu *)eobj_new(menu_class);
    t_binbuf* d = binbuf_via_atoms(argc,argv);
    if(x && d)
    {
        ebox_new((t_ebox *)x, 0 | EBOX_GROWINDI | EBOX_FONTSIZE);
        ebox_parameter_create((t_ebox *)x, 1);
        ebox_parameter_setsettergetter_text((t_ebox *)x, 1,
                                            (t_param_setter_t)menu_setter_t,
                                            (t_param_getter_t)menu_getter_t);
        
        x->f_out_index      = outlet_new((t_object *)x, &s_float);
        x->f_out_item       = outlet_new((t_object *)x, &s_list);
        x->f_out_infos      = outlet_new((t_object *)x, &s_anything);
        x->f_nitems     = 0;
        x->f_popup          = NULL;
        ebox_attrprocess_viabinbuf(x, d);
        ebox_ready((t_ebox *)x);
        
        return (x);
    }
    return NULL;
}

extern "C" void setup_c0x2emenu(void)
{
    t_eclass *c;
    
    c = eclass_new("c.menu", (method)menu_new, (method)ebox_free, (short)sizeof(t_menu), 0L, A_GIMME, 0);
    
    eclass_guiinit(c, 0);
    eclass_addmethod(c, (method) menu_paint,           "paint",            A_NULL, 0);
    eclass_addmethod(c, (method) menu_notify,          "notify",           A_NULL, 0);
    eclass_addmethod(c, (method) menu_getdrawparams,   "getdrawparams",    A_NULL, 0);
    eclass_addmethod(c, (method) menu_oksize,          "oksize",           A_NULL, 0);
    
    eclass_addmethod(c, (method) menu_append,          "append",           A_GIMME,0);
    eclass_addmethod(c, (method) menu_insert,          "insert",           A_GIMME,0);
    eclass_addmethod(c, (method) menu_setitem,         "setitem",          A_GIMME,0);
    eclass_addmethod(c, (method) menu_delete,          "delete",           A_FLOAT,0);
    eclass_addmethod(c, (method) menu_clear,           "clear",            A_NULL, 0);
    eclass_addmethod(c, (method) menu_state,           "state",            A_GIMME,0);
    
    eclass_addmethod(c, (method) menu_float,           "float",            A_FLOAT,0);
    eclass_addmethod(c, (method) menu_anything,        "anything",         A_GIMME,0);
    eclass_addmethod(c, (method) menu_set,             "set",              A_GIMME,0);
    eclass_addmethod(c, (method) menu_setsymbol,       "setsymbol",        A_GIMME,0);
    eclass_addmethod(c, (method) menu_bang,            "bang",             A_NULL, 0);
    eclass_addmethod(c, (method) menu_next,            "next",             A_NULL, 0);
    eclass_addmethod(c, (method) menu_prev,            "prev",             A_NULL, 0);
    
    eclass_addmethod(c, (method) menu_count,            "count",           A_NULL, 0);
    
    eclass_addmethod(c, (method) menu_mousedown,        "mousedown",       A_NULL, 0);
    eclass_addmethod(c, (method) menu_mouseenter,       "mouseenter",      A_NULL, 0);
    eclass_addmethod(c, (method) menu_popup,            "popup",           A_NULL, 0);
    
    CLASS_ATTR_DEFAULT              (c, "size", 0, "100 13");
    
    CLASS_ATTR_LONG                 (c, "hover", 0, t_menu, f_hover);
    CLASS_ATTR_LABEL                (c, "hover", 0, "Hover Mode");
    CLASS_ATTR_ORDER                (c, "hover", 0, "1");
    CLASS_ATTR_FILTER_CLIP          (c, "hover", 0, 1);
    CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "hover", 0, "0");
    CLASS_ATTR_STYLE                (c, "hover", 0, "onoff");
    
    CLASS_ATTR_SYMBOL_VARSIZE       (c, "items", 0, t_menu, f_items, f_nitems, CREAM_MAXITEMS);
    CLASS_ATTR_LABEL                (c, "items", 0, "Items");
    CLASS_ATTR_ACCESSORS            (c, "items", NULL, menu_items_set);
    CLASS_ATTR_ORDER                (c, "items", 0, "1");
    CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "items", 0, "");
    
    CLASS_ATTR_CHAR_VARSIZE         (c, "states", 0, t_menu, f_states, f_nitems, CREAM_MAXITEMS);
    CLASS_ATTR_LABEL                (c, "states", 0, "Items Disable State");
    CLASS_ATTR_ACCESSORS            (c, "states", NULL, menu_states_set);
    CLASS_ATTR_ORDER                (c, "states", 0, "1");
    CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "states", 0, "")
    
    CLASS_ATTR_FONT                 (c, "font", 0, t_menu, f_font);
    CLASS_ATTR_LABEL                (c, "font", 0, "Font");
    CLASS_ATTR_SAVE                 (c, "font", 0);
    CLASS_ATTR_PAINT                (c, "font", 0);;
    
    CLASS_ATTR_RGBA                 (c, "bgcolor", 0, t_menu, f_color_background);
    CLASS_ATTR_LABEL                (c, "bgcolor", 0, "Background Color");
    CLASS_ATTR_ORDER                (c, "bgcolor", 0, "1");
    CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bgcolor", 0, "0.75 0.75 0.75 1.");
    CLASS_ATTR_STYLE                (c, "bgcolor", 0, "color");
    
    CLASS_ATTR_RGBA                 (c, "bdcolor", 0, t_menu, f_color_border);
    CLASS_ATTR_LABEL                (c, "bdcolor", 0, "Border Color");
    CLASS_ATTR_ORDER                (c, "bdcolor", 0, "2");
    CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bdcolor", 0, "0.5 0.5 0.5 1.");
    CLASS_ATTR_STYLE                (c, "bdcolor", 0, "color");
    
    CLASS_ATTR_RGBA                 (c, "textcolor", 0, t_menu, f_color_text);
    CLASS_ATTR_LABEL                (c, "textcolor", 0, "Text Color");
    CLASS_ATTR_ORDER                (c, "textcolor", 0, "3");
    CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "textcolor", 0, "0. 0. 0. 1.");
    CLASS_ATTR_STYLE                (c, "textcolor", 0, "color");
    
    eclass_register(CLASS_BOX, c);
    menu_class = c;
}


