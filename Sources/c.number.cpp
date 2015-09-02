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
#include <float.h>
typedef struct  _number
{
	t_ebox          j_box;
    t_outlet*       f_outlet;
    t_outlet*       f_outtab;
    t_etexteditor*  f_editor;
    char            f_firstchar;

    float           f_refvalue;
    float           f_deriv;
    float           f_inc;
    int             f_ndecimal;
    t_efont         f_font;
	t_rgba          f_color_background;
	t_rgba          f_color_border;
	t_rgba          f_color_text;
} t_number;

static t_eclass *number_class;

static void number_output(t_number *x)
{
    t_pd* send = ebox_getsender((t_ebox *)x);
    const float val = ebox_parameter_getvalue((t_ebox *)x, 1);
    outlet_float(x->f_outlet, val);
    if(send)
    {
        pd_float(send, val);
    }
}

static void number_float(t_number *x, float f)
{
    ebox_parameter_setvalue((t_ebox *)x, 1, f, 1);
    number_output(x);
    ebox_invalidate_layer((t_ebox *)x, cream_sym_value_layer);
    ebox_redraw((t_ebox *)x);
}

static void number_set(t_number *x, float f)
{
    ebox_parameter_setvalue((t_ebox *)x , 1, f, 0);
    ebox_invalidate_layer((t_ebox *)x, cream_sym_value_layer);
    ebox_redraw((t_ebox *)x);
}

static void number_bang(t_number *x, float f)
{
    ebox_parameter_notify_changes((t_ebox *)x, 1);
    number_output(x);
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
    newrect->width  = pd_clip_min(newrect->width, (x->f_font.size + 4.f) * 2.f);
    newrect->height = x->f_font.size + 4.f;
}

static t_pd_err number_notify(t_number *x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
	if(msg == cream_sym_attr_modified)
	{
		if(s == cream_sym_bgcolor || s == cream_sym_bdcolor || s == cream_sym_textcolor ||
           s == cream_sym_font || s == cream_sym_decimal)
		{
			ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
			ebox_invalidate_layer((t_ebox *)x, cream_sym_value_layer);
		}
	}
    else if(msg == cream_sym_param_changed)
    {
        number_output(x);
        ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
        ebox_redraw((t_ebox *)x);
    }
	return 0;
}

static void draw_background(t_number *x, t_object *view, t_rect *rect)
{
    t_elayer *g = ebox_start_layer((t_ebox *)x, cream_sym_background_layer, rect->width, rect->height);
    if(g)
    {
        const float width = x->f_font.size + 4;
        t_etext *jtl = etext_layout_create();
        if(jtl)
        {
            etext_layout_set(jtl, "©", &x->f_font, 0.f, 0.f, width, rect->height, ETEXT_CENTRED, ETEXT_NOWRAP);
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
            const float width = x->f_font.size + 8;
            const float val = ebox_parameter_getvalue((t_ebox *)x, 1);
            char number[512];
            memset(number, 0, sizeof(char) * 512);
            if(!x->f_ndecimal)
                sprintf(number, "%i", (int)val);
            else if(x->f_ndecimal == 1)
                sprintf(number, "%.1g", val);
            else if(x->f_ndecimal == 2)
                sprintf(number, "%.2g", val);
            else if(x->f_ndecimal == 3)
                sprintf(number, "%.3g", val);
            else if(x->f_ndecimal == 4)
                sprintf(number, "%.4g", val);
            else if(x->f_ndecimal == 5)
                sprintf(number, "%.5g", val);
            else
                sprintf(number, "%.6g", val);
            
            etext_layout_settextcolor(jtl, &x->f_color_text);
            etext_layout_set(jtl, number, &x->f_font, width, 0.f, rect->width - width, rect->height, ETEXT_CENTREDLEFT, ETEXT_NOWRAP);
            
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
        if(key == EKEY_RETURN)
        {
            etexteditor_gettext(editor, &text);
            if(text && isdigit(text[0]))
            {
                ebox_parameter_setvalue((t_ebox *)x, 1, atof(text), 1);
                number_output(x);
                free(text);
            }
        }
        else if(key == EKEY_TAB)
        {
            etexteditor_gettext(editor, &text);
            if(text && isdigit(text[0]))
            {
                ebox_parameter_setvalue((t_ebox *)x, 1, atof(text), 1);
                number_output(x);
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
            etexteditor_setfont(x->f_editor, &x->f_font);
            etexteditor_setwrap(x->f_editor, 0);
            rect.x = x->f_font.size + 5;
            rect.y = 0;
            rect.width -= x->f_font.size + 5;
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
	const float text_width = x->f_font.size * 2. / 3.;
    ebox_parameter_begin_changes((t_ebox *)x, 1);
    if(pt.x >= text_width + 8)
    {
        int i = 1;
        int n_integer = 1;
        float pos = pt.x - text_width + 8 / text_width;
        x->f_deriv = pt.y;
        
        x->f_refvalue = ebox_parameter_getvalue((t_ebox *)x, 1);
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
        x->f_inc = pd_clip(x->f_inc, -100., -0.000001);
    }
}

static void number_mousedrag(t_number *x, t_object *patcherview, t_pt pt, long modifiers)
{
    ebox_set_cursor((t_ebox *)x, 2);
    ebox_parameter_setvalue((t_ebox *)x, 1, x->f_refvalue + (pt.y - x->f_deriv) * x->f_inc * 0.5, 0);
    number_output(x);
    ebox_invalidate_layer((t_ebox *)x, cream_sym_value_layer);
    ebox_redraw((t_ebox *)x);
}

static void number_mouseup(t_number *x, t_object *patcherview, t_pt pt, long modifiers)
{
    ebox_parameter_end_changes((t_ebox *)x, 1);
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
    t_binbuf* d = binbuf_via_atoms(argc, argv);
    
    if(x && d)
    {
        efont_init(&x->f_font, gensym("DejaVu"), 0, 0, 11);
        ebox_new((t_ebox *)x, 0 | EBOX_GROWINDI | EBOX_FONTSIZE);
        ebox_parameter_create((t_ebox *)x, 1);
        ebox_parameter_setmin((t_ebox *)x, 1, -FLT_MAX);
        ebox_parameter_setmax((t_ebox *)x, 1, FLT_MAX);
        ebox_parameter_setflags((t_ebox *)x, 1, 0 | EPARAM_STATIC_INVERTED);
        
        x->f_outlet   = outlet_new((t_object *)x, &s_float);
        x->f_outtab   = outlet_new((t_object *)x, &s_bang);
        x->f_editor   = NULL;
        
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
        eclass_addmethod(c, (method) number_paint,              "paint",            A_NULL, 0);
        eclass_addmethod(c, (method) number_notify,             "notify",           A_NULL, 0);
        eclass_addmethod(c, (method) number_getdrawparams,      "getdrawparams",    A_NULL, 0);
        eclass_addmethod(c, (method) number_oksize,             "oksize",           A_NULL, 0);
        eclass_addmethod(c, (method) number_float,              "float",            A_FLOAT,0);
        eclass_addmethod(c, (method) number_set,                "set",              A_FLOAT,0);
        eclass_addmethod(c, (method) number_bang,               "bang",             A_NULL, 0);
        
        eclass_addmethod(c, (method) number_mousedown,          "mousedown",        A_NULL, 0);
        eclass_addmethod(c, (method) number_mousedrag,          "mousedrag",        A_NULL, 0);
        eclass_addmethod(c, (method) number_mouseup,            "mouseup",          A_NULL, 0);
        
        eclass_addmethod(c, (method) number_dblclick,            "dblclick",            A_NULL, 0);
        eclass_addmethod(c, (method) number_dblclick,            "select",              A_NULL, 0);
        eclass_addmethod(c, (method) number_texteditor_keypress, "texteditor_keypress", A_NULL, 0);
        eclass_addmethod(c, (method) number_texteditor_keyfilter,"texteditor_keyfilter",A_NULL, 0);
        
        CLASS_ATTR_DEFAULT              (c, "size", 0, "53 13");
        
        CLASS_ATTR_INT                  (c, "decimal", 0, t_number, f_ndecimal);
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
        
        CLASS_ATTR_FONT                 (c, "font", 0, t_number, f_font);
        CLASS_ATTR_LABEL                (c, "font", 0, "Font");
        CLASS_ATTR_SAVE                 (c, "font", 0);
        CLASS_ATTR_PAINT                (c, "font", 0);
        
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




