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

typedef struct _camomile
{
	t_ebox      j_box;
    t_symbol*   f_name;
    t_efont     f_font;
    int         f_bdsize;
	t_rgba		f_color_background;
    t_rgba		f_color_border;
    t_rgba		f_color_txt;
} t_camomile;

static t_eclass *camomile_class;

static void camomile_getdrawparams(t_camomile *x, t_object *view, t_edrawparams *params)
{
	params->d_borderthickness   = x->f_bdsize;
	params->d_cornersize        = 2;
    params->d_bordercolor       = x->f_color_border;
    params->d_boxfillcolor      = x->f_color_background;
}

static void camomile_oksize(t_camomile *x, t_rect *newrect)
{
    newrect->width = pd_clip_min(newrect->width, 22.f);
    newrect->height = pd_clip_min(newrect->height, 44.f);
}

static t_pd_err camomile_notify(t_camomile *x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
	if(msg == cream_sym_attr_modified &&
       (s == cream_sym_bgcolor || s == cream_sym_bdcolor || s == cream_sym_bdsize || s == cream_sym_name))
	{
		ebox_invalidate_layer((t_ebox *)x, NULL, cream_sym_background_layer);
	}
	return 0;
}

static void camomile_paint(t_camomile *x, t_object *view)
{
    t_rect rect;
    ebox_getdrawbounds((t_ebox *)x, view,  &rect);
    t_elayer *g = ebox_start_layer((t_ebox *)x, view, cream_sym_background_layer, rect.width, rect.height);
    if(g)
    {
        if(is_valid_symbol(x->f_name))
        {
            t_etextlayout *jtl = etextlayout_new();
            if(jtl)
            {
                etextlayout_settextcolor(jtl, &x->f_color_txt);
                etextlayout_set(jtl, x->f_name->s_name, &x->f_font, 22.f, 0.f, rect.width - 22.f, 20.f,  ETEXT_CENTRED, ETEXT_NOWRAP);
                etextlayout_draw(jtl, g);
                etextlayout_destroy(jtl);
            }
        }
        elayer_set_color_rgba(g, &x->f_color_border);
        elayer_set_line_width(g, x->f_bdsize);
        elayer_line(g, 0., 20.f, rect.width, 20.f);
        elayer_stroke(g);
        ebox_end_layer((t_ebox*)x, view, cream_sym_background_layer);
    }
    ebox_paint_layer((t_ebox *)x, view, cream_sym_background_layer, 0., 0.);
}

static void *camomile_new(t_symbol *s, int argc, t_atom *argv)
{
    t_camomile *x = (t_camomile *)eobj_new(camomile_class);
    t_binbuf* d = binbuf_via_atoms(argc, argv);
    
    if(x && d)
    {
        ebox_new((t_ebox *)x, 0 | EBOX_GROWINDI | EBOX_IGNORELOCKCLICK | EBOX_FONTSIZE);
        eobj_attr_read(x, d);
        ebox_ready((t_ebox *)x);
    }
    
    return (x);
}

extern "C" void setup_c0x2ecamomile(void)
{
    t_eclass *c;
    
    c = eclass_new("c.camomile", (t_method)camomile_new, (t_method)ebox_free, (short)sizeof(t_camomile), 0L, A_GIMME, 0);
    
    eclass_guiinit(c, 0);
    eclass_addmethod(c, (t_method) camomile_paint,           "paint",            A_NULL, 0);
    eclass_addmethod(c, (t_method) camomile_notify,          "notify",           A_NULL, 0);
    eclass_addmethod(c, (t_method) camomile_getdrawparams,   "getdrawparams",    A_NULL, 0);
    eclass_addmethod(c, (t_method) camomile_oksize,          "oksize",           A_NULL, 0);
    
    CLASS_ATTR_INVISIBLE            (c, "send", 1);
    CLASS_ATTR_INVISIBLE            (c, "receive", 1);
    CLASS_ATTR_DEFAULT              (c, "size", 0, "22. 44.");
    CLASS_ATTR_DEFAULT              (c, "pinned", 0, "1");
    CLASS_ATTR_INVISIBLE            (c, "pinned", 1);
    
    CLASS_ATTR_SYMBOL               (c, "name", 0, t_camomile, f_name);
    CLASS_ATTR_LABEL                (c, "name", 0, "Name");
    CLASS_ATTR_ORDER                (c, "name", 0, "1");
    CLASS_ATTR_PAINT                (c, "name", 0);
    CLASS_ATTR_SAVE                 (c, "name", 0);
    CLASS_ATTR_DEFAULT              (c, "name", 0, "");
    
    CLASS_ATTR_FONT                 (c, "font", 0, t_camomile, f_font);
    CLASS_ATTR_LABEL                (c, "font", 0, "Font");
    CLASS_ATTR_ORDER                (c, "font", 0, "1");
    CLASS_ATTR_PAINT                (c, "font", 0);
    CLASS_ATTR_SAVE                 (c, "font", 0);
    
    CLASS_ATTR_INT                  (c, "bdsize", 0, t_camomile, f_bdsize);
    CLASS_ATTR_LABEL                (c, "bdsize", 0, "Border Size");
    CLASS_ATTR_ORDER                (c, "bdsize", 0, "1");
    CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bdsize", 0, "2");
    CLASS_ATTR_FILTER_CLIP          (c, "bdsize", 0, 4);
    CLASS_ATTR_STYLE                (c, "bdsize", 0, "number");
    
    CLASS_ATTR_RGBA                 (c, "bgcolor", 0, t_camomile, f_color_background);
    CLASS_ATTR_LABEL                (c, "bgcolor", 0, "Background Color");
    CLASS_ATTR_ORDER                (c, "bgcolor", 0, "1");
    CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bgcolor", 0, "0.75 0.75 0.75 1.");
    CLASS_ATTR_STYLE                (c, "bgcolor", 0, "color");
    
    CLASS_ATTR_RGBA                 (c, "bdcolor", 0, t_camomile, f_color_border);
    CLASS_ATTR_LABEL                (c, "bdcolor", 0, "Border Color");
    CLASS_ATTR_ORDER                (c, "bdcolor", 0, "2");
    CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bdcolor", 0, "0.5 0.5 0.5 1.");
    CLASS_ATTR_STYLE                (c, "bdcolor", 0, "color");
    
    CLASS_ATTR_RGBA                 (c, "txtcolor", 0, t_camomile, f_color_txt);
    CLASS_ATTR_LABEL                (c, "txtcolor", 0, "Text Color");
    CLASS_ATTR_ORDER                (c, "txtcolor", 0, "3");
    CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "txtcolor", 0, "0.f 0.f 0.f 1.");
    CLASS_ATTR_STYLE                (c, "txtcolor", 0, "color");
    
    eclass_register(CLASS_BOX, c);
    camomile_class = c;
}







