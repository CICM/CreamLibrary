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
    t_epopup*   f_popup;
    t_symbol*   f_items[CREAM_MAXITEMS];
    long        f_states[CREAM_MAXITEMS];
    long        f_items_size;
    long        f_states_size;
    long        f_item_selected;
    long        f_hover;
	t_rgba		f_color_background;
	t_rgba		f_color_border;
	t_rgba		f_color_text;
} t_menu;

static t_eclass *menu_class;


static void menu_output(t_menu *x)
{
    if(x->f_items_size > 0)
    {
        t_pd* send = ebox_getsender((t_ebox *) x);
        outlet_float(x->f_out_index, x->f_item_selected);
        outlet_symbol(x->f_out_item, x->f_items[x->f_item_selected]);
        if(send)
        {
            pd_float(ebox_getsender((t_ebox *) x), (float)x->f_item_selected);
        }
    }
}

static t_symbol* menu_atoms_to_sym(t_atom* argv, long argc)
{
    int i;
    size_t length;
    char temp[MAXPDSTRING];
    char text[MAXPDSTRING];
    atom_string(argv, text, MAXPDSTRING);
    for(i = 1; i < argc; i++)
    {
        atom_string(argv+i, temp, MAXPDSTRING);
        length = strlen(temp);
        strncat(text, " ", 1);
        strncat(text, temp, length);
    }
    return gensym(text);
}

static long menu_symbol_exist(t_menu *x, t_symbol* s)
{
    long i;
    long j = -1;
    for(i = 0; i < x->f_items_size; i++)
    {
        if(!strcmp(s->s_name, x->f_items[i]->s_name))
        {
            j = i;
            break;
        }
    }
    return j;
}

static void menu_append(t_menu *x, t_symbol *s, int argc, t_atom *argv)
{
    t_symbol* item = menu_atoms_to_sym(argv, argc);
    
    if(argc && argv && menu_symbol_exist(x, item) == -1)
    {
        x->f_items[x->f_items_size] = item;
        x->f_items_size++;
        ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
        ebox_redraw((t_ebox *)x);
    }
}

static void menu_insert(t_menu *x, t_symbol *s, int argc, t_atom *argv)
{
    int i;
    t_symbol* item = menu_atoms_to_sym(argv+1, argc-1);
    
    if(argc > 1 && argv && atom_gettype(argv) == A_FLOAT && atom_getfloat(argv) >= 0 && menu_symbol_exist(x, item) == -1)
    {
        if(atom_getfloat(argv) >= x->f_items_size)
        {
            x->f_items[x->f_items_size] = item;
        }
        else
        {
            for(i = (int)x->f_items_size - 1; i >= atom_getfloat(argv); i--)
            {
                x->f_items[i+1] = x->f_items[i];
            }
            x->f_items[atom_getint(argv)] = item;
        }
        x->f_items_size++;
        ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
        ebox_redraw((t_ebox *)x);
    }
}

static void menu_setitem(t_menu *x, t_symbol *s, int argc, t_atom *argv)
{
    t_symbol* item = menu_atoms_to_sym(argv+1, argc-1);
    if(argc > 1 && argv && atom_gettype(argv) == A_FLOAT && menu_symbol_exist(x, item) == -1)
    {
        if(atom_getfloat(argv) >= 0 && atom_getfloat(argv) < x->f_items_size)
            x->f_items[atom_getint(argv)] = item;
        
        ebox_invalidate_layer((t_ebox *)x, gensym("list_layer"));
        
        ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
        ebox_redraw((t_ebox *)x);
    }
}

void menu_delete(t_menu *x, t_symbol *s, int argc, t_atom *argv)
{
    int i;
    if(argc > 0 && argv && atom_gettype(argv) == A_FLOAT)
    {
        if(atom_getfloat(argv) >= 0 && atom_getfloat(argv) < x->f_items_size)
        {
            for(i = atom_getfloat(argv); i < x->f_items_size - 1; i++)
                x->f_items[i] = x->f_items[i+1];
            x->f_items_size--;
            for(i = (int)x->f_items_size; i < CREAM_MAXITEMS; i++)
                x->f_items[i] = NULL;
            
            ebox_invalidate_layer((t_ebox *)x, gensym("list_layer"));
            
            ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
            ebox_redraw((t_ebox *)x);
        }
    }
}

void menu_clear(t_menu *x, t_symbol *s, int argc, t_atom *argv)
{
    int i;
    for(i = 0; i < CREAM_MAXITEMS; i++)
    {
        x->f_items[i] = NULL;
    }
    x->f_items_size = 0;
    ebox_invalidate_layer((t_ebox *)x, gensym("list_layer"));
    
    ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
    ebox_redraw((t_ebox *)x);
}

void menu_state(t_menu *x, t_symbol *s, int argc, t_atom *argv)
{
    if(argc > 1 && argv && atom_gettype(argv) == A_FLOAT)
    {
        if(atom_getfloat(argv) >= 0 && atom_getfloat(argv) < x->f_items_size)
        {
            if(atom_gettype(argv+1) == A_FLOAT && atom_getfloat(argv+1) != 0)
                x->f_states[(int)atom_getfloat(argv)] = 1;
            else
                x->f_states[(int)atom_getfloat(argv)] = 0;
            
            ebox_invalidate_layer((t_ebox *)x, gensym("list_layer"));
            
            ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
            ebox_redraw((t_ebox *)x);
        }
    }
}

void menu_clean(t_menu *x)
{
    int i, j;
    for(i = 0; i < x->f_items_size; i++)
    {
        if(x->f_items[i] == NULL)
        {
            for(j = i; j < x->f_items_size; j++)
                x->f_items[j] = x->f_items[j+1];
            x->f_items_size--;
            i--;
        }
    }
    x->f_states_size = x->f_items_size;
}

static void menu_setfloat(t_menu *x, t_floatarg f)
{
    if(f >= 0 && f < x->f_items_size)
    {
        x->f_item_selected = f;
        ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
        ebox_redraw((t_ebox *)x);
    }
}

static void menu_setsymbol(t_menu *x, t_symbol* s)
{
    const long i = menu_symbol_exist(x, s);
    if(i != -1)
    {
        x->f_item_selected = i;
    }
    ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
    ebox_redraw((t_ebox *)x);
}

void menu_float(t_menu *x, t_floatarg f)
{
    menu_setfloat(x, f);
    menu_output(x);
}

void menu_symbol(t_menu *x, t_symbol *s, int argc, t_atom *argv)
{
    int i;
    t_atom* av = (t_atom *)calloc((size_t)(argc + 1), sizeof(t_atom));
    atom_setsym(av, s);
    for(i = 0; i < argc; i++)
        av[i+1] = argv[i];

    menu_setsymbol(x, menu_atoms_to_sym(av, argc+1));
    menu_output(x);
}

void menu_set(t_menu *x, t_symbol *s, int argc, t_atom *argv)
{
    if(argc && argv)
    {
        if(atom_gettype(argv) == A_FLOAT)
            menu_setfloat(x, atom_getfloat(argv));
        else if (atom_gettype(argv) == A_SYMBOL)
            menu_setsymbol(x, menu_atoms_to_sym(argv, argc));
    }
}

static void menu_getdrawparams(t_menu *x, t_object *patcherview, t_edrawparams *params)
{
	params->d_borderthickness   = 2;
	params->d_cornersize        = 2;
    params->d_bordercolor       = x->f_color_border;
    params->d_boxfillcolor      = x->f_color_background;
}

static void menu_oksize(t_menu *x, t_rect *newrect)
{
    newrect->width = pd_clip_min(newrect->width, ebox_getfontsize((t_ebox *)x) * 3 + 8);
    newrect->height = newrect->height = ebox_getfontsize((t_ebox *)x) + 4;
}

static t_pd_err menu_notify(t_menu *x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
	if (msg == cream_sym_attr_modified)
	{
		if(s == cream_sym_bgcolor || s == cream_sym_bdcolor || s == cream_sym_textcolor || s == cream_sym_fontsize || s == cream_sym_fontname || s == cream_sym_fontweight || s == cream_sym_fontslant)
		{
			ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
		}
        if(s == cream_sym_fontsize || s == cream_sym_items)
        {
            eobj_attr_setvalueof(x, s_cream_size, 0, NULL);
        }
	}
	return 0;
}

static void menu_paint(t_menu *x, t_object *view)
{
	t_rect rect;
	ebox_get_rect_for_view((t_ebox *)x, &rect);
    int todo;
    menu_clean(x);
 
    t_elayer *g = ebox_start_layer((t_ebox *)x, cream_sym_background_layer, rect.width, rect.height);
    if(g)
    {
        
        if(x->f_items_size && x->f_item_selected < x->f_items_size)
        {
            t_etext *jtl = etext_layout_create();
            if(jtl)
            {
                etext_layout_set(jtl, x->f_items[x->f_item_selected]->s_name, ebox_getfont((t_ebox *)x),
                             1.5f, 0, rect.width - rect.height - 2.f, rect.height, ETEXT_CENTREDLEFT, ETEXT_NOWRAP);
                etext_layout_settextcolor(jtl, &x->f_color_text);
                etext_layout_draw(jtl, g);
                etext_layout_destroy(jtl);
            }
        }
        
        // Separation
        egraphics_set_color_rgba(g, &x->f_color_border);
        egraphics_set_line_width(g, 2);
        egraphics_line_fast(g, rect.width - rect.height, 0., rect.width - rect.height, rect.height);
        
        // Arraw Up
        egraphics_move_to(g, rect.width - rect.height + 2.f, rect.height * 0.5f - 2.f);
        egraphics_line_to(g, rect.width - 2.f, rect.height * 0.5f - 2.f);
        egraphics_line_to(g, rect.width - rect.height * 0.5, 2.f);
        egraphics_close_path(g);
        egraphics_fill(g);
        
        // Arraw Down
        egraphics_move_to(g, rect.width - rect.height + 2.f, rect.height * 0.5f + 2.f);
        egraphics_line_to(g, rect.width - 2.f, rect.height * 0.5f + 2.f);
        egraphics_line_to(g, rect.width - rect.height * 0.5, rect.height - 2.f);
        egraphics_close_path(g);
        egraphics_fill(g);
        
        ebox_end_layer((t_ebox*)x, cream_sym_background_layer);
    }
    ebox_paint_layer((t_ebox *)x, cream_sym_background_layer,  0., 0.);
}

static void menu_popup(t_menu *x, t_epopup *popup, long itemid)
{
    if(x->f_popup && popup == x->f_popup)
    {
        menu_float(x, (float)itemid);
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
            t_canvas* cnv = eobj_getcanvas(x);
            if(cnv)
            {
                ebox_get_rect_for_view((t_ebox *)x, &rect);
                for(long i = 0; i < x->f_items_size; i++)
                {
                    epopupmenu_additem(x->f_popup, (int)i, x->f_items[i]->s_name,
                                       (char)(x->f_item_selected == i), x->f_states[i]);
                }
                epopupmenu_setfont(x->f_popup, ebox_getfont((t_ebox *)x));
                epopupmenu_setbackgroundcolor(x->f_popup, &x->f_color_background);
                epopupmenu_settextcolor(x->f_popup, &x->f_color_text);
                epopupmenu_popup(x->f_popup, &rect);
            }
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
            t_canvas* cnv = eobj_getcanvas(x);
            if(cnv)
            {
                ebox_get_rect_for_view((t_ebox *)x, &rect);
                for(long i = 0; i < x->f_items_size; i++)
                {
                    epopupmenu_additem(x->f_popup, (int)i, x->f_items[i]->s_name,
                                       (char)(x->f_item_selected == i), x->f_states[i]);
                }
                epopupmenu_setfont(x->f_popup, ebox_getfont((t_ebox *)x));
                epopupmenu_setbackgroundcolor(x->f_popup, &x->f_color_background);
                epopupmenu_settextcolor(x->f_popup, &x->f_color_text);
                epopupmenu_popup(x->f_popup, &rect);
            }
        }
    }
}

static t_pd_err menu_states_set(t_menu *x, t_object *attr, int ac, t_atom *av)
{
    int i;
    if(ac && av)
    {
        x->f_states_size = x->f_items_size;
        for(i = 0; i < ac && i < x->f_items_size; i++)
        {
            if(atom_gettype(av+i) == A_FLOAT && atom_getfloat(av+i) != 0)
                x->f_states[i] = 1;
            else
                x->f_states[i] = 0;
        }
    }
    return 0;
}

static t_pd_err menu_items_set(t_menu *x, t_object *attr, int ac, t_atom *av)
{
    char text[MAXPDSTRING];
    menu_clear(x, NULL, 0, NULL);
    if(ac && av)
    {
        for(int i = 0; i < ac; i++)
        {
            atom_string(av+i, text, MAXPDSTRING);
            x->f_items[i] = gensym(text);
        }
    }
    x->f_items_size = ac;
    return 0;
}

static t_pd_err menu_items_get(t_menu *x, t_object *attr, int* ac, t_atom **av)
{
    int i;
    *ac = (int)x->f_items_size;
    *av = (t_atom *)malloc((size_t)*ac * sizeof(t_atom));
    if(*av)
    {
        for(i = 0; i < *ac; i++)
        {
            atom_setsym(*av+i, x->f_items[i]);
        }
    }
    else
    {
        *ac = 0;
        *av = NULL;
    }
    return 0;
}

static void menu_preset(t_menu *x, t_binbuf *b)
{
    binbuf_addv(b, (char *)"sf", &s_float, (float)x->f_item_selected);
}

static void *menu_new(t_symbol *s, int argc, t_atom *argv)
{
    t_menu *x= (t_menu *)eobj_new(menu_class);
    t_binbuf* d = binbuf_via_atoms(argc,argv);
    if(x && d)
    {
        ebox_new((t_ebox *)x, 0 | EBOX_GROWINDI);
        
        x->f_out_index      = outlet_new((t_object *)x, &s_float);
        x->f_out_item       = outlet_new((t_object *)x, &s_list);
        x->f_item_selected  = 0;
        x->f_items_size     = 0;
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
    eclass_addmethod(c, (method) menu_delete,          "delete",           A_GIMME,0);
    eclass_addmethod(c, (method) menu_clear,           "clear",            A_GIMME,0);
    eclass_addmethod(c, (method) menu_state,           "state",            A_GIMME,0);
    
    eclass_addmethod(c, (method) menu_float,           "float",            A_FLOAT,0);
    eclass_addmethod(c, (method) menu_symbol,          "anything",         A_GIMME,0);
    eclass_addmethod(c, (method) menu_set,             "set",              A_GIMME,0);
    eclass_addmethod(c, (method) menu_output,          "bang",             A_NULL, 0);
    
    eclass_addmethod(c, (method) menu_mousedown,        "mousedown",       A_NULL, 0);
    eclass_addmethod(c, (method) menu_mouseenter,       "mouseenter",      A_NULL, 0);
    eclass_addmethod(c, (method) menu_popup,            "popup",           A_NULL, 0);
    eclass_addmethod(c, (method) menu_preset,           "preset",          A_NULL, 0);
    
    CLASS_ATTR_DEFAULT              (c, "size", 0, "100 13");
    
    CLASS_ATTR_LONG                 (c, "hover", 0, t_menu, f_hover);
    CLASS_ATTR_LABEL                (c, "hover", 0, "Hover Mode");
    CLASS_ATTR_ORDER                (c, "hover", 0, "1");
    CLASS_ATTR_FILTER_CLIP          (c, "hover", 0, 1);
    CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "hover", 0, "0");
    CLASS_ATTR_STYLE                (c, "hover", 0, "onoff");
    
    CLASS_ATTR_SYMBOL_VARSIZE       (c, "items", 0, t_menu, f_items, f_items_size, CREAM_MAXITEMS);
    CLASS_ATTR_LABEL                (c, "items", 0, "Items");
    CLASS_ATTR_ACCESSORS            (c, "items", menu_items_get, menu_items_set);
    CLASS_ATTR_ORDER                (c, "items", 0, "1");
    CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "items", 0, " ");
    
    CLASS_ATTR_LONG_VARSIZE         (c, "states", 0, t_menu, f_states, f_states_size, CREAM_MAXITEMS);
    CLASS_ATTR_LABEL                (c, "states", 0, "Items Disable State");
    CLASS_ATTR_ACCESSORS            (c, "states", NULL, menu_states_set);
    CLASS_ATTR_ORDER                (c, "states", 0, "1");
    CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "states", 0, " ");
    
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


