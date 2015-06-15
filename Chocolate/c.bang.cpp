/*
 * CicmWrapper
 *
 * A wrapper for Pure Data
 *
 * Copyright (C) 2013 Pierre Guillot, CICM - Université Paris 8
 * All rights reserved.
 *
 * Website  : http://www.mshparisnord.fr/HoaLibrary/
 * Contacts : cicm.mshparisnord@gmail.com
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published
 * by the Free Software Foundation; either version 2 of the License.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

/*!
 * \example c.bang.cpp
 * \brief This is an example of how to use the create a basic GUI.
 * \details This example shows how to initialize a GUI class with methods for painting and mouse interractions and attributes.
 */

#include "../c.library.h"

/**
 * @example c.bang.cpp
 * @struct t_bang
 * @brief The example GUI bang structure.
 * @details It is a basic GUI struture with attributes and basic Pd stuffs.
 */
typedef struct t_bang
{
	t_ebox      b_box;              /*!< The t_ebox that allows to create a GUI. */
    t_outlet*   b_out;              /*!< The t_outlet of the object. */
	t_rgba		b_color_background; /*!< The struture for the t_eattr background color. */
	t_rgba		b_color_border;     /*!< The struture for the t_eattr border color. */
	t_rgba		b_color_bang;       /*!< The struture for the t_eattr bang color. */
    t_clock*    b_clock;            /*!< The t_clock of the object. */
    char        b_active;           /*!< If the object is performming a bang. */
    
} t_bang;

/*!< The example t_eclass for the t_bang. */
static t_eclass *bang_class;

static void bang_getdrawparams(t_bang *x, t_object *patcherview, t_edrawparams *params)
{
	params->d_borderthickness   = 2;
	params->d_cornersize        = 2;
    params->d_bordercolor       = x->b_color_border;
    params->d_boxfillcolor      = x->b_color_border;
}

static void bang_oksize(t_bang *x, t_rect *newrect)
{
    newrect->width = pd_clip_min(newrect->width, 16.);
    newrect->height = pd_clip_min(newrect->height, 16.);
    if((int)newrect->width % 2 == 0)
        newrect->width++;
    if((int)newrect->height % 2 == 0)
        newrect->height++;
}

static void bang_output(t_bang *x, t_symbol* s, int argc, t_atom *argv)
{
    x->b_active = 1;
    ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
    ebox_redraw((t_ebox *)x);
    outlet_bang(x->b_out);
    if(ebox_getsender((t_ebox *) x))
        pd_bang(ebox_getsender((t_ebox *) x));
    
    clock_delay(x->b_clock, 100);
}

static t_pd_err bang_notify(t_bang *x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
	if(msg == cream_sym_attr_modified)
	{
		if(s == cream_sym_bgcolor || s == cream_sym_bdcolor || s == cream_sym_bacolor)
		{
			ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
		}
        ebox_redraw((t_ebox *)x);
	}
	return 0;
}

static void draw_background(t_bang *x, t_object *view, t_rect *rect)
{
    float size;
	t_elayer *g = ebox_start_layer((t_ebox *)x, cream_sym_background_layer, rect->width, rect->height);
	if (g)
	{
        size = rect->width * 0.5;
        if(x->b_active)
        {
            egraphics_set_color_rgba(g, &x->b_color_bang);
        }
        else
        {
            egraphics_set_color_rgba(g, &x->b_color_background);
        }
        egraphics_circle(g, floor(size + 0.5), floor(size+ 0.5), size * 0.9);
        egraphics_fill(g);
        ebox_end_layer((t_ebox*)x, cream_sym_background_layer);
	}
	ebox_paint_layer((t_ebox *)x, cream_sym_background_layer, 0., 0.);
}

static void bang_paint(t_bang *x, t_object *view)
{
    t_rect rect;
    ebox_get_rect_for_view((t_ebox *)x, &rect);
    draw_background(x, view, &rect);
}

static void bang_mousedown(t_bang *x, t_object *patcherview, t_pt pt, long modifiers)
{
    x->b_active = 1;
    ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
    ebox_redraw((t_ebox *)x);
    outlet_bang(x->b_out);
    if(ebox_getsender((t_ebox *) x))
        pd_bang(ebox_getsender((t_ebox *) x));
}

static void bang_mouseup(t_bang *x, t_object *patcherview, t_pt pt, long modifiers)
{
    x->b_active = 0;
    ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
    ebox_redraw((t_ebox *)x);
}

static void *bang_new(t_symbol *s, int argc, t_atom *argv)
{
    t_bang *x  = (t_bang *)eobj_new(bang_class);
    t_binbuf* d = binbuf_via_atoms(argc,argv);
    
    if(x && d)
    {
        ebox_new((t_ebox *)x, 0 | EBOX_GROWLINK);
        x->b_out = outlet_new((t_object *)x, &s_bang);
        x->b_active = 0;
        x->b_clock          = clock_new(x,(t_method)bang_mouseup);
        ebox_attrprocess_viabinbuf(x, d);
        ebox_ready((t_ebox *)x);
    }
    
    return (x);
}

/*!
 * \fn          static void bang_free(t_bang *x)
 * \brief       Frees the t_bang structure.
 * \details     The function just calls ebox_free() and frees b_clock.
 * \param x     The t_bang pointer.
 */
static void bang_free(t_bang *x)
{
    ebox_free((t_ebox *)x);
    clock_free(x->b_clock);
}

/*!
 * \fn          extern "C" void setup_c0x2ebang(void)
 * \brief       Setups the bang_class for GUI behavior.
 * \details     ...
 */
extern "C" void setup_c0x2ebang(void)
{
    // Creation of the t_eclass for the t_bang structure.
    t_eclass *c = eclass_new("c.bang", (method)bang_new, (method)bang_free, (short)sizeof(t_bang), 0L, A_GIMME, 0);
    if(c)
    {
        eclass_guiinit(c, 0);
        
        eclass_addmethod(c, (method) bang_paint,           "paint",            A_NULL, 0);
        eclass_addmethod(c, (method) bang_notify,          "notify",           A_NULL, 0);
        eclass_addmethod(c, (method) bang_getdrawparams,   "getdrawparams",    A_NULL, 0);
        eclass_addmethod(c, (method) bang_oksize,          "oksize",           A_NULL, 0);
        eclass_addmethod(c, (method) bang_output,          "float",            A_FLOAT,0);
        eclass_addmethod(c, (method) bang_output,          "bang",             A_NULL, 0);
        eclass_addmethod(c, (method) bang_output,          "list",             A_GIMME,0);
        eclass_addmethod(c, (method) bang_output,          "anything",         A_GIMME,0);
        eclass_addmethod(c, (method) bang_mousedown,       "mousedown",        A_NULL, 0);
        eclass_addmethod(c, (method) bang_mouseup,         "mouseup",          A_NULL, 0);
        
        CLASS_ATTR_INVISIBLE            (c, "fontname", 1);
        CLASS_ATTR_INVISIBLE            (c, "fontweight", 1);
        CLASS_ATTR_INVISIBLE            (c, "fontslant", 1);
        CLASS_ATTR_INVISIBLE            (c, "fontsize", 1);
        CLASS_ATTR_DEFAULT              (c, "size", 0, "16. 16.");
        
        CLASS_ATTR_RGBA                 (c, "bgcolor", 0, t_bang, b_color_background);
        CLASS_ATTR_LABEL                (c, "bgcolor", 0, "Background Color");
        CLASS_ATTR_ORDER                (c, "bgcolor", 0, "1");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bgcolor", 0, "0.75 0.75 0.75 1.");
        CLASS_ATTR_STYLE                (c, "bgcolor", 0, "color");
        
        CLASS_ATTR_RGBA                 (c, "bdcolor", 0, t_bang, b_color_border);
        CLASS_ATTR_LABEL                (c, "bdcolor", 0, "Border Color");
        CLASS_ATTR_ORDER                (c, "bdcolor", 0, "2");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bdcolor", 0, "0.5 0.5 0.5 1.");
        CLASS_ATTR_STYLE                (c, "bdcolor", 0, "color");
        
        CLASS_ATTR_RGBA                 (c, "bacolor", 0, t_bang, b_color_bang);
        CLASS_ATTR_LABEL                (c, "bacolor", 0, "Bang Color");
        CLASS_ATTR_ORDER                (c, "bacolor", 0, "3");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bacolor", 0, "0. 0. 0. 1.");
        CLASS_ATTR_STYLE                (c, "bacolor", 0, "color");
        
        bang_class = c;
    }
}





