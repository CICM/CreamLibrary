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
#include <stdlib.h>
typedef struct  _number
{
	t_ebox          j_box;
    t_outlet*       f_outlet;
    t_outlet*       f_outtab;
    t_etexteditor*  f_editor;
    char            f_mode;
    char            f_firstchar;

    float       f_value;
    float       f_refvalue;
    float       f_deriv;
    float       f_inc;

    long        f_ndecimal;
    t_atom      f_min;
    t_atom      f_max;

	t_rgba		f_color_background;
	t_rgba		f_color_border;
	t_rgba		f_color_text;
} t_number;

static t_eclass *number_class;

static void number_output(t_number *x)
{
    t_pd* send = ebox_getsender((t_ebox *) x);
    outlet_float(x->f_outlet, x->f_value);
    if(send)
    {
        pd_float(send, x->f_value);
    }
}

static void number_float(t_number *x, float f)
{
    if(atom_gettype(&x->f_max) == A_FLOAT && atom_gettype(&x->f_min) == A_FLOAT)
    {
        x->f_value = f = pd_clip_minmax(f, atom_getfloat(&x->f_min), atom_getfloat(&x->f_max));
    }
    if(atom_gettype(&x->f_max) == A_FLOAT)
    {
        x->f_value = pd_clip_max(f, atom_getfloat(&x->f_max));
    }
    if(atom_gettype(&x->f_min) == A_FLOAT)
    {
        x->f_value = pd_clip_min(f, atom_getfloat(&x->f_min));
    }
    else
    {
        x->f_value = f;
    }
    
    number_output(x);
    ebox_invalidate_layer((t_ebox *)x, cream_sym_value_layer);
    ebox_redraw((t_ebox *)x);
}

static void number_set(t_number *x, float f)
{
    if(atom_gettype(&x->f_max) == A_FLOAT && atom_gettype(&x->f_min) == A_FLOAT)
    {
        x->f_value = f = pd_clip_minmax(f, atom_getfloat(&x->f_min), atom_getfloat(&x->f_max));
    }
    if(atom_gettype(&x->f_max) == A_FLOAT)
    {
        x->f_value = pd_clip_max(f, atom_getfloat(&x->f_max));
    }
    if(atom_gettype(&x->f_min) == A_FLOAT)
    {
        x->f_value = pd_clip_min(f, atom_getfloat(&x->f_min));
    }
    else
    {
        x->f_value = f;
    }
    
    ebox_invalidate_layer((t_ebox *)x, cream_sym_value_layer);
    ebox_redraw((t_ebox *)x);
}

static t_pd_err number_notify(t_number *x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
	if(msg == cream_sym_attr_modified)
	{
		if(s == cream_sym_bgcolor || s == cream_sym_bdcolor || s == cream_sym_textcolor || s == cream_sym_fontsize ||
           s == cream_sym_fontname || s == cream_sym_fontweight || s == cream_sym_fontslant || s == cream_sym_decimal)
		{
			ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
			ebox_invalidate_layer((t_ebox *)x, cream_sym_value_layer);
		}
        if(s == cream_sym_fontsize || s == cream_sym_fontname)
        {
            ebox_notify((t_ebox *)x, s_cream_size, cream_sym_attr_modified, NULL, NULL);
        }
	}
	return 0;
}

static void number_getdrawparams(t_number *x, t_object *patcherview, t_edrawparams *params)
{
    params->d_borderthickness   = 2;
    params->d_cornersize        = 2;
    params->d_bordercolor       = x->f_color_border;
    params->d_boxfillcolor      = x->f_color_background;
}

static void number_oksize(t_number *x, t_rect *newrect)
{
    const float size = ebox_getfontsize((t_ebox *)x);
    newrect->width  = pd_clip_min(newrect->width, (size + 4.f) * 2.f);
    newrect->height = size + 4.f;
}

static void draw_background(t_number *x, t_object *view, t_rect *rect)
{
    t_elayer *g = ebox_start_layer((t_ebox *)x, cream_sym_background_layer, rect->width, rect->height);
    if(g)
    {
        const float width = ebox_getfontsize((t_ebox *)x) + 4;
        t_etext *jtl = etext_layout_create();
        if(jtl)
        {
            etext_layout_set(jtl, "©", &x->j_box.b_font, 0.f, 0.f, width, rect->height, ETEXT_CENTRED, ETEXT_NOWRAP);
            etext_layout_settextcolor(jtl, &x->f_color_text);
            etext_layout_draw(jtl, g);
        }
        egraphics_set_line_width(g, 2.f);
        egraphics_set_color_rgba(g, &x->f_color_border);
        egraphics_move_to(g, width, 0);
        egraphics_line_to(g, width,  rect->height);
        egraphics_stroke(g);
        
        ebox_end_layer((t_ebox*)x, cream_sym_background_layer);
        etext_layout_destroy(jtl);
    }
    ebox_paint_layer((t_ebox *)x, cream_sym_background_layer, 0., 0.);
}

static void draw_value_drag(t_number *x, t_object *view, t_rect *rect)
{
    t_elayer *g = ebox_start_layer((t_ebox *)x, cream_sym_value_layer, rect->width, rect->height);
    if(g)
    {
        t_etext *jtl = etext_layout_create();
        if(jtl)
        {
            const float width = ebox_getfontsize((t_ebox *)x) + 8;
            char number[512];
            memset(number, 0, sizeof(char) * 512);
            if(!x->f_ndecimal)
                sprintf(number, "%i", (int)x->f_value);
            else if(x->f_ndecimal == 1)
                sprintf(number, "%.1g", x->f_value);
            else if(x->f_ndecimal == 2)
                sprintf(number, "%.2g", x->f_value);
            else if(x->f_ndecimal == 3)
                sprintf(number, "%.3g", x->f_value);
            else if(x->f_ndecimal == 4)
                sprintf(number, "%.4g", x->f_value);
            else if(x->f_ndecimal == 5)
                sprintf(number, "%.5g", x->f_value);
            else
                sprintf(number, "%.6g", x->f_value);
            
            etext_layout_settextcolor(jtl, &x->f_color_text);
            etext_layout_set(jtl, number, &x->j_box.b_font, width, 0.f, rect->width - width, rect->height, ETEXT_CENTREDLEFT, ETEXT_NOWRAP);
            
            etext_layout_draw(jtl, g);
            etext_layout_destroy(jtl);
        }
        ebox_end_layer((t_ebox*)x, cream_sym_value_layer);
    }
    ebox_paint_layer((t_ebox *)x, cream_sym_value_layer, 0., 0.);
}

static void number_paint(t_number *x, t_object *view)
{
	t_rect rect;
    ebox_get_rect_for_view((t_ebox *)x, &rect);

    draw_background(x, view, &rect);
    draw_value_drag(x, view, &rect);
}

static void number_texteditor_keypress(t_number *x, t_etexteditor *editor, int key)
{
    if(editor && editor == x->f_editor && !x->f_firstchar && !isdigit((char)key))
    {
        etexteditor_clear(x->f_editor);
    }
    else
    {
        x->f_firstchar = 1;
    }
}

static void number_texteditor_keyfilter(t_number *x, t_etexteditor *editor, ekey_flags key)
{
    char* text = NULL;
    if(editor && editor == x->f_editor)
    {
        if(key == EKEY_ENTER)
        {
            etexteditor_gettext(editor, &text);
            if(text && isdigit(text[0]))
            {
                x->f_value = atof(text);
                free(text);
            }
        }
        else if(key == EKEY_TAB)
        {
            etexteditor_gettext(editor, &text);
            if(text && isdigit(text[0]))
            {
                x->f_value = atof(text);
                free(text);
            }
            outlet_bang(x->f_outtab);
        }
        
        etexteditor_destroy(editor);
        x->f_editor = NULL;
        
        ebox_invalidate_layer((t_ebox *)x, cream_sym_value_layer);
        ebox_redraw((t_ebox *)x);
    }
}

static void number_dblclick(t_number *x, t_object *patcherview, t_pt pt, long modifiers)
{
    t_rect rect;
    if(!x->f_editor)
    {
        x->f_editor = etexteditor_create((t_ebox *)x);
        if(x->f_editor)
        {
            ebox_get_rect_for_view((t_ebox *)x, &rect);
            etexteditor_setbackgroundcolor(x->f_editor, &x->f_color_background);
            etexteditor_settextcolor(x->f_editor, &x->f_color_text);
            etexteditor_setfont(x->f_editor, ebox_getfont((t_ebox *)x));
            etexteditor_setwrap(x->f_editor, 0);
            rect.x = ebox_getfontsize((t_ebox *)x) + 5;
            rect.y = 0;
            rect.width -= ebox_getfontsize((t_ebox *)x) + 5;
            etexteditor_popup(x->f_editor,  &rect);
            x->f_firstchar = 0;
        }
        else
        {
            return;
        }
    }
    etexteditor_grabfocus(x->f_editor);
}

static void number_mousedown(t_number *x, t_object *patcherview, t_pt pt, long modifiers)
{
	float text_width = ebox_getfontsize((t_ebox *)x) * 2. / 3.;
    x->f_mode = 0;
    if(pt.x >= text_width + 8)
    {
        int i = 1;
        int n_integer = 1;
        float pos = pt.x - text_width + 8 / text_width;
        x->f_deriv = pt.y;
        x->f_refvalue = x->f_value;
        while(fabs(x->f_refvalue) >= powf(10, n_integer))
            n_integer++;

        while(text_width + 6 + i * text_width < pos)
            i++;

        if(x->f_refvalue < 0) // due to "-" offset
        {
            if(i < n_integer)
                x->f_inc = -powf(10, (n_integer - i));
            else
                x->f_inc = -1. / powf(10, (i - n_integer - 1));
        }
        else
        {
            if(i < n_integer + 2)
                x->f_inc = -powf(10, (n_integer - i));
            else
                x->f_inc = -1. / powf(10, (i - n_integer - 1));
        }
        x->f_inc = pd_clip_minmax(x->f_inc, -100., -0.000001);
    }
}

static void number_mousedrag(t_number *x, t_object *patcherview, t_pt pt, long modifiers)
{
    x->f_mode = 0;
    ebox_set_cursor((t_ebox *)x, 2);
    const float value = x->f_refvalue + (pt.y - x->f_deriv) * x->f_inc * 0.5;
    if(PD_BADFLOAT(value) || PD_BIGORSMALL(value))
        return;

    x->f_value = value;
    number_output(x);
    ebox_invalidate_layer((t_ebox *)x, cream_sym_value_layer);
    ebox_redraw((t_ebox *)x);
}

static void number_preset(t_number *x, t_binbuf *b)
{
    binbuf_addv(b, (char *)"sf", &s_float, x->f_value);
}

static t_pd_err number_min_set(t_number *x, t_object *attr, int ac, t_atom *av)
{
    if(ac && av && atom_gettype(av) == A_FLOAT)
    {
        x->f_min = av[0];

        if(atom_gettype(&x->f_max) == A_FLOAT && atom_getfloat(&x->f_max) < atom_getfloat(&x->f_min))
        {
            x->f_min = x->f_max;
            x->f_max = av[0];
        }

        if(atom_gettype(&x->f_max) == A_FLOAT)
            x->f_value = pd_clip_max(x->f_value, atom_getfloat(&x->f_max));
        x->f_value = pd_clip_min(x->f_value, atom_getfloat(&x->f_min));
    }
    else
    {
        atom_setsym(&x->f_min, s_cream_empty);
    }
    ebox_invalidate_layer((t_ebox *)x, cream_sym_value_layer);
    ebox_redraw((t_ebox *)x);
    return 0;
}

static t_pd_err number_max_set(t_number *x, t_object *attr, int ac, t_atom *av)
{
    if(ac && av && atom_gettype(av) == A_FLOAT)
    {
        x->f_max = av[0];
        if(atom_gettype(&x->f_min) == A_FLOAT && atom_getfloat(&x->f_max) < atom_getfloat(&x->f_min))
        {
            x->f_max = x->f_min;
            x->f_min = av[0];
        }

        if(atom_gettype(&x->f_min) == A_FLOAT)
            x->f_value = pd_clip_min(x->f_value, atom_getfloat(&x->f_min));
        x->f_value = pd_clip_max(x->f_value, atom_getfloat(&x->f_max));
    }
    else
    {
        atom_setsym(&x->f_max, s_cream_empty);
    }

    ebox_invalidate_layer((t_ebox *)x, cream_sym_value_layer);
    ebox_redraw((t_ebox *)x);
    return 0;
}

static void number_free(t_number *x)
{
    if(x->f_editor)
    {
        etexteditor_destroy(x->f_editor);
    }
    ebox_free((t_ebox *)x);
}

static void *number_new(t_symbol *s, int argc, t_atom *argv)
{
    t_number *x = (t_number *)eobj_new(number_class);
    t_binbuf* d = binbuf_via_atoms(argc,argv);
    
    if(x && d)
    {
        ebox_new((t_ebox *)x, 0 | EBOX_GROWINDI);
        x->f_outlet   = outlet_new((t_object *)x, &s_float);
        x->f_outtab   = outlet_new((t_object *)x, &s_bang);
        x->f_editor   = NULL;
        x->f_mode     = 0.f;
        x->f_value    = 0;
        
        ebox_attrprocess_viabinbuf(x, d);
        ebox_ready((t_ebox *)x);
        
        return x;
    }
    
    return NULL;
}

extern "C" void setup_c0x2enumber(void)
{
    t_eclass *c = eclass_new("c.number", (method)number_new, (method)number_free, (short)sizeof(t_number), 0L, A_GIMME, 0);
    
    if(c)
    {
        eclass_guiinit(c, 0 | EBOX_TEXTFIELD);
        eclass_addmethod(c, (method) number_paint,           "paint",            A_NULL, 0);
        eclass_addmethod(c, (method) number_notify,          "notify",           A_NULL, 0);
        eclass_addmethod(c, (method) number_getdrawparams,   "getdrawparams",    A_NULL, 0);
        eclass_addmethod(c, (method) number_oksize,          "oksize",           A_NULL, 0);
        eclass_addmethod(c, (method) number_float,           "float",            A_FLOAT,0);
        eclass_addmethod(c, (method) number_set,             "set",              A_FLOAT,0);
        eclass_addmethod(c, (method) number_output,          "bang",             A_NULL, 0);
        
        eclass_addmethod(c, (method) number_mousedown,        "mousedown",       A_NULL, 0);
        eclass_addmethod(c, (method) number_mousedrag,        "mousedrag",       A_NULL, 0);
        
        eclass_addmethod(c, (method) number_dblclick,            "dblclick",            A_NULL, 0);
        eclass_addmethod(c, (method) number_dblclick,            "select",              A_NULL, 0);
        eclass_addmethod(c, (method) number_texteditor_keypress, "texteditor_keypress", A_NULL, 0);
        eclass_addmethod(c, (method) number_texteditor_keyfilter,"texteditor_keyfilter",A_NULL, 0);
        
        eclass_addmethod(c, (method) number_preset,           "preset",          A_NULL, 0);
        
        CLASS_ATTR_DEFAULT			(c, "size", 0, "53 13");
        
        CLASS_ATTR_ATOM                 (c, "min", 0, t_number, f_min);
        CLASS_ATTR_ORDER                (c, "min", 0, "3");
        CLASS_ATTR_LABEL                (c, "min", 0, "Min Value");
        CLASS_ATTR_DEFAULT              (c, "min", 0, "empty");
        CLASS_ATTR_ACCESSORS            (c, "min", NULL, number_min_set);
        CLASS_ATTR_SAVE                 (c, "min", 1);
        
        CLASS_ATTR_ATOM                 (c, "max", 0, t_number, f_min);
        CLASS_ATTR_ORDER                (c, "max", 0, "3");
        CLASS_ATTR_LABEL                (c, "max", 0, "Max Value");
        CLASS_ATTR_DEFAULT              (c, "max", 0, "empty");
        CLASS_ATTR_ACCESSORS            (c, "max", NULL, number_max_set);
        CLASS_ATTR_SAVE                 (c, "max", 1);
        
        CLASS_ATTR_LONG                 (c, "decimal", 0, t_number, f_ndecimal);
        CLASS_ATTR_ORDER                (c, "decimal", 0, "3");
        CLASS_ATTR_LABEL                (c, "decimal", 0, "Number of decimal");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "decimal", 0, "4");
        CLASS_ATTR_FILTER_MIN           (c, "decimal", 0);
        CLASS_ATTR_FILTER_MAX           (c, "decimal", 6);
        CLASS_ATTR_STYLE                (c, "decimal", 0, "number");
        
        CLASS_ATTR_RGBA                 (c, "bgcolor", 0, t_number, f_color_background);
        CLASS_ATTR_LABEL                (c, "bgcolor", 0, "Background Color");
        CLASS_ATTR_ORDER                (c, "bgcolor", 0, "1");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bgcolor", 0, "0.75 0.75 0.75 1.");
        CLASS_ATTR_STYLE                (c, "bgcolor", 0, "color");
        
        CLASS_ATTR_RGBA                 (c, "bdcolor", 0, t_number, f_color_border);
        CLASS_ATTR_LABEL                (c, "bdcolor", 0, "Border Color");
        CLASS_ATTR_ORDER                (c, "bdcolor", 0, "2");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bdcolor", 0, "0.5 0.5 0.5 1.");
        CLASS_ATTR_STYLE                (c, "bdcolor", 0, "color");
        
        CLASS_ATTR_RGBA                 (c, "textcolor", 0, t_number, f_color_text);
        CLASS_ATTR_LABEL                (c, "textcolor", 0, "Text Color");
        CLASS_ATTR_ORDER                (c, "textcolor", 0, "3");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "textcolor", 0, "0. 0. 0. 1.");
        CLASS_ATTR_STYLE                (c, "textcolor", 0, "color");
        
        eclass_register(CLASS_BOX, c);
        number_class = c;
    }
}




