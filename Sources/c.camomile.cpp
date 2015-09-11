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
    t_efont     f_font;
	t_rgba		f_color_background;
    t_rgba		f_color_border;
} t_camomile;

t_eclass *camomile_class;

static void camomile_getdrawparams(t_camomile *x, t_object *patcherview, t_edrawparams *params)
{
	params->d_borderthickness   = 2;
	params->d_cornersize        = 2;
    params->d_bordercolor       = x->f_color_border;
    params->d_boxfillcolor      = x->f_color_background;
}

static void camomile_oksize(t_camomile *x, t_rect *newrect)
{
    newrect->width = pd_clip_min(newrect->width, 200.f);
    newrect->height = pd_clip_min(newrect->height, 80.f);
}

static t_pd_err camomile_notify(t_camomile *x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
	if(msg == cream_sym_attr_modified && s == cream_sym_bgcolor)
	{
		ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
	}
	return 0;
}

static void camomile_paint(t_camomile *x, t_object *view)
{
    ;
}

static void *camomile_new(t_symbol *s, int argc, t_atom *argv)
{
    t_camomile *x = (t_camomile *)eobj_new(camomile_class);
    t_binbuf* d = binbuf_via_atoms(argc, argv);
    
    if(x && d)
    {
        ebox_new((t_ebox *)x, 0 | EBOX_GROWINDI | EBOX_IGNORELOCKCLICK);
        ebox_attrprocess_viabinbuf(x, d);
        ebox_ready((t_ebox *)x);
    }
    
    return (x);
}

extern "C" void setup_c0x2ecamomile(void)
{
    t_eclass *c;
    
    c = eclass_new("c.camomile", (method)camomile_new, (method)ebox_free, (short)sizeof(t_camomile), 0L, A_GIMME, 0);
    
    eclass_guiinit(c, 0);
    eclass_addmethod(c, (method) camomile_paint,           "paint",            A_NULL, 0);
    eclass_addmethod(c, (method) camomile_notify,          "notify",           A_NULL, 0);
    eclass_addmethod(c, (method) camomile_getdrawparams,   "getdrawparams",    A_NULL, 0);
    eclass_addmethod(c, (method) camomile_oksize,          "oksize",           A_NULL, 0);
    
    CLASS_ATTR_INVISIBLE            (c, "send", 1);
    CLASS_ATTR_INVISIBLE            (c, "receive", 1);
    CLASS_ATTR_DEFAULT              (c, "size", 0, "600. 400.");
    CLASS_ATTR_DEFAULT              (c, "pinned", 0, "1");
    CLASS_ATTR_INVISIBLE            (c, "pinned", 1);
    
    CLASS_ATTR_FONT                 (c, "font", 0, t_camomile, f_font);
    CLASS_ATTR_LABEL                (c, "font", 0, "Font");
    CLASS_ATTR_ORDER                (c, "font", 0, "1");
    CLASS_ATTR_PAINT                (c, "font", 0);
    
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
    
    eclass_register(CLASS_BOX, c);
    camomile_class = c;
}







