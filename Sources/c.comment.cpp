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

typedef struct  _comment
{
	t_ebox          j_box;
    t_etextlayouteditor*  f_editor;
    char*           f_text;
    t_efont         f_font;
	t_rgba          f_color_background;
	t_rgba          f_color_border;
	t_rgba          f_color_text;
} t_comment;

static t_eclass *comment_class;

static void comment_getdrawparams(t_comment *x, t_object *view, t_edrawparams *params)
{
    params->d_borderthickness   = 2;
    params->d_cornersize        = 2;
    params->d_bordercolor       = x->f_color_border;
    params->d_boxfillcolor      = x->f_color_background;
}

static void comment_oksize(t_comment *x, t_rect *newrect)
{
    newrect->width  = pd_clip_min(newrect->width, (x->f_font.size + 4.f) * 2.f);
    newrect->height = pd_clip_min(newrect->height, (x->f_font.size + 4.f));
}

static t_pd_err comment_notify(t_comment *x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
	if(msg == cream_sym_attr_modified)
	{
		if(s == cream_sym_bgcolor || s == cream_sym_bdcolor || s == cream_sym_textcolor ||
           s == cream_sym_font)
		{
			ebox_invalidate_layer((t_ebox *)x, NULL, cream_sym_background_layer);
		}
	}
	return 0;
}

static void comment_paint(t_comment *x, t_object *view)
{
	t_rect rect;
    ebox_getdrawbounds((t_ebox *)x, view,  &rect);

    t_elayer *g = ebox_start_layer((t_ebox *)x, view, cream_sym_background_layer, rect.width, rect.height);
    if(g && x->f_text)
    {
        t_etextlayout *jtl = etextlayout_new();
        if(jtl)
        {
            etextlayout_set(jtl, x->f_text, &x->f_font, 2.f, 2.f, rect.width - 4.f, rect.height - 4.f, ETEXT_TOPLEFT, ETEXT_WRAP);
            etextlayout_settextcolor(jtl, &x->f_color_text);
            etextlayout_draw(jtl, g);
            etextlayout_destroy(jtl);
        }
        ebox_end_layer((t_ebox*)x, view, cream_sym_background_layer);
        
    }
    ebox_paint_layer((t_ebox *)x, view, cream_sym_background_layer, 0., 0.);
}

static void comment_texteditor_keypress(t_comment *x, t_etextlayouteditor *editor, int key)
{
    if(editor && editor == x->f_editor)
    {
        ;
    }
}

static void comment_texteditor_focus(t_comment *x, t_etextlayouteditor *editor, int focus)
{
    if(editor && editor == x->f_editor && !focus)
    {
        if(x->f_text)
        {
            free(x->f_text);
            x->f_text = NULL;
        }
        etexteditor_gettext(editor, &x->f_text);
        etexteditor_destroy(editor);
        x->f_editor = NULL;
        ebox_invalidate_layer((t_ebox *)x, NULL, cream_sym_background_layer);
        ebox_redraw((t_ebox *)x);
    }
}

static void comment_texteditor_keyfilter(t_comment *x, t_etextlayouteditor *editor, ekey_flags key)
{
    if(editor && editor == x->f_editor)
    {
        if(x->f_text)
        {
            free(x->f_text);
            x->f_text = NULL;
        }
        if(key == EKEY_ESC)
        {
            etexteditor_gettext(editor, &x->f_text);
            etexteditor_destroy(editor);
            x->f_editor = NULL;
            ebox_invalidate_layer((t_ebox *)x, NULL, cream_sym_background_layer);
            ebox_redraw((t_ebox *)x);
        }
    }
}

static void comment_dblclick(t_comment *x, t_object *view, t_pt pt, long modifiers)
{
    t_rect rect;
    if(!x->f_editor)
    {
        x->f_editor = etexteditor_create((t_ebox *)x);
        if(x->f_editor)
        {
            ebox_getdrawbounds((t_ebox *)x, view,  &rect);
            etexteditor_setbackgroundcolor(x->f_editor, &x->f_color_background);
            etexteditor_settextcolor(x->f_editor, &x->f_color_text);
            etexteditor_setfont(x->f_editor, &x->f_font);
            etexteditor_setwrap(x->f_editor, 1);
            if(x->f_text)
            {
                etexteditor_settext(x->f_editor, x->f_text);
            }
            rect.x = 2.f;
            rect.y = 2.f;
            rect.width -= 2.f;
            rect.height -= 2.f;
            etexteditor_popup(x->f_editor,  &rect);
        }
        else
        {
            return;
        }
    }
    etexteditor_grabfocus(x->f_editor);
}

static void comment_save(t_comment* x, t_binbuf *b)
{
    if(x->f_text)
    {
        binbuf_text(b, (char *)"@text", 10);
        binbuf_text(b, x->f_text, strlen(x->f_text));
    }
}

static void comment_free(t_comment *x)
{
    if(x->f_editor)
    {
        etexteditor_destroy(x->f_editor);
    }
    if(x->f_text)
    {
        free(x->f_text);
    }
    ebox_free((t_ebox *)x);
}

static void *comment_new(t_symbol *s, int argc, t_atom *argv)
{
    t_comment *x = (t_comment *)eobj_new(comment_class);
    t_binbuf* d = binbuf_via_atoms(argc, argv);
    t_symbol* text;
    size_t lenght;
    if(x && d)
    {
        ebox_new((t_ebox *)x, 0 | EBOX_GROWINDI | EBOX_FONTSIZE | EBOX_DBLCLICK_EDIT);
        x->f_editor   = NULL;
        x->f_text     = NULL;
        if(!binbuf_get_attribute_symbol(d, gensym("@text"), &text))
        {
            if(text)
            {
                lenght = strlen(text->s_name);
                x->f_text = (char *)malloc((lenght + 1) * sizeof(char));
                if(x->f_text)
                {
                    memset(x->f_text, 0, (lenght + 1) * sizeof(char));
                    memcpy(x->f_text, text->s_name, lenght * sizeof(char));
                }
            }
        }
        eobj_attr_read(x, d);
        ebox_ready((t_ebox *)x);
        
        return x;
    }
    
    return NULL;
}

extern "C" void setup_c0x2ecomment(void)
{
    t_eclass *c = eclass_new("c.comment", (t_method)comment_new, (t_method)comment_free, sizeof(t_comment), CLASS_NOINLET, A_GIMME, 0);
    if(c)
    {
        eclass_guiinit(c, 0 | EBOX_TEXTFIELD);
        eclass_addmethod(c, (t_method) comment_paint,              "paint",            A_NULL, 0);
        eclass_addmethod(c, (t_method) comment_notify,             "notify",           A_NULL, 0);
        eclass_addmethod(c, (t_method) comment_getdrawparams,      "getdrawparams",    A_NULL, 0);
        eclass_addmethod(c, (t_method) comment_oksize,             "oksize",           A_NULL, 0);
        eclass_addmethod(c, (t_method) comment_save,               "save",             A_NULL, 0);
        
        eclass_addmethod(c, (t_method) comment_dblclick,            "dblclick",            A_NULL, 0);
        eclass_addmethod(c, (t_method) comment_texteditor_keypress, "texteditor_keypress", A_NULL, 0);
        eclass_addmethod(c, (t_method) comment_texteditor_keyfilter,"texteditor_keyfilter",A_NULL, 0);
        eclass_addmethod(c, (t_method) comment_texteditor_focus,    "texteditor_focus",    A_NULL, 0);
        
        CLASS_ATTR_DEFAULT              (c, "size", 0, "53 13");
        
        CLASS_ATTR_FONT                 (c, "font", 0, t_comment, f_font);
        CLASS_ATTR_LABEL                (c, "font", 0, "Font");
        CLASS_ATTR_SAVE                 (c, "font", 0);
        CLASS_ATTR_PAINT                (c, "font", 0);
        
        CLASS_ATTR_RGBA                 (c, "bgcolor", 0, t_comment, f_color_background);
        CLASS_ATTR_LABEL                (c, "bgcolor", 0, "Background Color");
        CLASS_ATTR_ORDER                (c, "bgcolor", 0, "1");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bgcolor", 0, "0.75 0.75 0.75 1.");
        CLASS_ATTR_STYLE                (c, "bgcolor", 0, "color");
        
        CLASS_ATTR_RGBA                 (c, "bdcolor", 0, t_comment, f_color_border);
        CLASS_ATTR_LABEL                (c, "bdcolor", 0, "Border Color");
        CLASS_ATTR_ORDER                (c, "bdcolor", 0, "2");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bdcolor", 0, "0.5 0.5 0.5 1.");
        CLASS_ATTR_STYLE                (c, "bdcolor", 0, "color");
        
        CLASS_ATTR_RGBA                 (c, "textcolor", 0, t_comment, f_color_text);
        CLASS_ATTR_LABEL                (c, "textcolor", 0, "Text Color");
        CLASS_ATTR_ORDER                (c, "textcolor", 0, "3");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "textcolor", 0, "0. 0. 0. 1.");
        CLASS_ATTR_STYLE                (c, "textcolor", 0, "color");
        
        eclass_register(CLASS_BOX, c);
        comment_class = c;
    }
}




