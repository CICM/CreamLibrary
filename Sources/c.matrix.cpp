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

#if (_MSC_VER >= 1800)
static double round(double val)
{
	return floor(val + 0.5);
}
#endif


typedef struct  _matrixctrl
{
	t_ebox      j_box;

    t_outlet*   f_out_cell;
    t_outlet*   f_out_colrow;

    char*       f_values;
    t_pt        f_size;
    t_pt        f_selected;

	t_rgba		f_color_background;
	t_rgba		f_color_border;
    t_rgba      f_color_on;

} t_matrixctrl;

t_eclass *matrixctrl_class;

void *matrixctrl_new(t_symbol *s, int argc, t_atom *argv);
void matrixctrl_free(t_matrixctrl *x);
void matrixctrl_assist(t_matrixctrl *x, void *b, long m, long a, char *s);

void matrixctrl_bang(t_matrixctrl *x);
void matrixctrl_clear(t_matrixctrl *x);
void matrixctrl_set(t_matrixctrl *x, t_symbol *s, int ac, t_atom *av);
void matrixctrl_list(t_matrixctrl *x, t_symbol *s, int ac, t_atom *av);
void matrixctrl_getrow(t_matrixctrl *x, float f);
void matrixctrl_getcolumn(t_matrixctrl *x, float f);
void matrixctrl_output(t_matrixctrl *x, int i, int j);

t_pd_err matrixctrl_notify(t_matrixctrl *x, t_symbol *s, t_symbol *msg, void *sender, void *data);
t_pd_err matrixctrl_matrix_set(t_matrixctrl *x, t_object *attr, int ac, t_atom *av);

void matrixctrl_getdrawparams(t_matrixctrl *x, t_object *patcherview, t_edrawparams *params);
void matrixctrl_oksize(t_matrixctrl *x, t_rect *newrect);

void matrixctrl_paint(t_matrixctrl *x, t_object *view);
void draw_background(t_matrixctrl *x,  t_object *view, t_rect *rect);

void matrixctrl_mousedown(t_matrixctrl *x, t_object *patcherview, t_pt pt, long modifiers);
void matrixctrl_mousedrag(t_matrixctrl *x, t_object *patcherview, t_pt pt, long modifiers);
void matrixctrl_mouseleave(t_matrixctrl *x, t_object *patcherview, t_pt pt, long modifiers);
void matrixctrl_preset(t_matrixctrl *x, t_binbuf *b);

extern "C" void setup_c0x2ematrix(void)
{
	t_eclass *c;

	c = eclass_new("c.matrix", (method)matrixctrl_new, (method)matrixctrl_free, (short)sizeof(t_matrixctrl), 0L, A_GIMME, 0);
	eclass_guiinit(c, 0);
    eclass_addmethod(c, (method) matrixctrl_assist,          "assist",           A_NULL, 0);
	eclass_addmethod(c, (method) matrixctrl_paint,           "paint",            A_NULL, 0);
	eclass_addmethod(c, (method) matrixctrl_notify,          "notify",           A_NULL, 0);
    eclass_addmethod(c, (method) matrixctrl_getdrawparams,   "getdrawparams",    A_NULL, 0);
    eclass_addmethod(c, (method) matrixctrl_oksize,          "oksize",           A_NULL, 0);
    eclass_addmethod(c, (method) matrixctrl_set,             "set",              A_GIMME,0);
    eclass_addmethod(c, (method) matrixctrl_list,            "list",             A_GIMME,0);
    eclass_addmethod(c, (method) matrixctrl_bang,            "bang",             A_NULL, 0);
    eclass_addmethod(c, (method) matrixctrl_clear,           "clear",            A_NULL, 0);
    eclass_addmethod(c, (method) matrixctrl_getrow,          "getrow",           A_FLOAT,0);
    eclass_addmethod(c, (method) matrixctrl_getcolumn,       "getcolumn",        A_FLOAT,0);

    eclass_addmethod(c, (method) matrixctrl_mousedown,       "mousedown",        A_NULL, 0);
    eclass_addmethod(c, (method) matrixctrl_mousedrag,       "mousedrag",        A_NULL, 0);
    eclass_addmethod(c, (method) matrixctrl_mouseleave,      "mouseleave",       A_NULL, 0);

    eclass_addmethod(c, (method) matrixctrl_preset,          "preset",           A_NULL, 0);

    CLASS_ATTR_INVISIBLE            (c, "fontname", 1);
    CLASS_ATTR_INVISIBLE            (c, "fontweight", 1);
    CLASS_ATTR_INVISIBLE            (c, "fontslant", 1);
    CLASS_ATTR_INVISIBLE            (c, "fontsize", 1);
	CLASS_ATTR_DEFAULT              (c, "size", 0, "105 53");

    CLASS_ATTR_FLOAT_ARRAY          (c, "matrix", 0, t_matrixctrl, f_size, 2);
	CLASS_ATTR_LABEL                (c, "matrix", 0, "Matrix Size");
    CLASS_ATTR_ACCESSORS			(c, "matrix", NULL, matrixctrl_matrix_set);
	CLASS_ATTR_ORDER                (c, "matrix", 0, "1");
	CLASS_ATTR_DEFAULT              (c, "matrix", 0, "8 4");
    CLASS_ATTR_SAVE                 (c, "matrix", 0);

    CLASS_ATTR_RGBA                 (c, "bgcolor", 0, t_matrixctrl, f_color_background);
	CLASS_ATTR_LABEL                (c, "bgcolor", 0, "Background Color");
	CLASS_ATTR_ORDER                (c, "bgcolor", 0, "1");
	CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bgcolor", 0, "0.75 0.75 0.75 1.");
    CLASS_ATTR_STYLE                (c, "bgcolor", 0, "color");

	CLASS_ATTR_RGBA                 (c, "bdcolor", 0, t_matrixctrl, f_color_border);
	CLASS_ATTR_LABEL                (c, "bdcolor", 0, "Border Color");
	CLASS_ATTR_ORDER                (c, "bdcolor", 0, "2");
	CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bdcolor", 0, "0.5 0.5 0.5 1.");
	CLASS_ATTR_STYLE                (c, "bdcolor", 0, "color");

    CLASS_ATTR_RGBA                 (c, "accolor", 0, t_matrixctrl, f_color_on);
	CLASS_ATTR_LABEL                (c, "accolor", 0, "Active Cell Color");
	CLASS_ATTR_ORDER                (c, "accolor", 0, "3");
	CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "accolor", 0, "0.5 0.5 0.5 1.");
	CLASS_ATTR_STYLE                (c, "accolor", 0, "color");

    eclass_register(CLASS_BOX, c);
	matrixctrl_class = c;
}

void *matrixctrl_new(t_symbol *s, int argc, t_atom *argv)
{
	t_matrixctrl *x =  NULL;
	t_binbuf* d;
    long flags;
	if (!(d = binbuf_via_atoms(argc,argv)))
		return NULL;

	x = (t_matrixctrl *)eobj_new(matrixctrl_class);

    x->f_size.x = 8;
    x->f_size.y = 4;
    x->f_values = (char *)malloc(x->f_size.x * x->f_size.y * sizeof(char));
    memset(x->f_values, 0, x->f_size.x * x->f_size.y * sizeof(char));

    x->f_selected.x = -1;
    x->f_selected.y = -1;

    flags = 0
    | EBOX_GROWINDI
    ;
	ebox_new((t_ebox *)x, flags);

    x->f_out_cell   = outlet_new((t_object *)x, &s_list);
    x->f_out_colrow = outlet_new((t_object *)x, &s_list);

	ebox_attrprocess_viabinbuf(x, d);
	ebox_ready((t_ebox *)x);
	return (x);
}

void matrixctrl_getdrawparams(t_matrixctrl *x, t_object *patcherview, t_edrawparams *params)
{
	params->d_borderthickness   = 2;
	params->d_cornersize        = 2;
    params->d_bordercolor       = x->f_color_border;
    params->d_boxfillcolor      = x->f_color_background;
}

void matrixctrl_oksize(t_matrixctrl *x, t_rect *newrect)
{
	float ratio;
    newrect->width = pd_clip_min(newrect->width, 30.);
    newrect->height = pd_clip_min(newrect->height, 10.);

    ratio = (newrect->width - 1.) / (float)x->f_size.x;
    if(ratio - (int)ratio != 0)
    {
        ratio = round(ratio);
        newrect->width = ratio * (float)x->f_size.x + 1.;
    }
    ratio = (newrect->height - 1.) / (float)x->f_size.y;
    if(ratio - (int)ratio != 0)
    {
        ratio = round(ratio);
        newrect->height = ratio * (float)x->f_size.y + 1.;
    }

    newrect->width = pd_clip_min(newrect->width, 30.);
    newrect->height = pd_clip_min(newrect->height, 10.);
}

void matrixctrl_bang(t_matrixctrl *x)
{
    for(long i = 0; i < (long)x->f_size.y; i++)
    {
        for(long j = 0; j < (long)x->f_size.x; j++)
        {
            matrixctrl_output(x, (int)j, (int)i);
        }
    }
}

void matrixctrl_clear(t_matrixctrl *x)
{
    for(int i = 0; i < x->f_size.y; i++)
    {
        for(int j = 0; j < x->f_size.x; j++)
        {
           if(x->f_values[i * (long)x->f_size.x + j])
           {
               x->f_values[i * (long)x->f_size.x + j] = 0;
               matrixctrl_output(x, j, i);
           }
        }
    }
    ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
    ebox_redraw((t_ebox *)x);
}

void matrixctrl_getrow(t_matrixctrl *x, float f)
{
    t_atom* av;
    if(f >= 0 && f < x->f_size.y)
    {
        av = (t_atom *)malloc(x->f_size.x * sizeof(t_atom));
        for(long i = 0; i < x->f_size.x; i++)
        {
            atom_setfloat(av+i, x->f_values[(long)f * (long)x->f_size.x + i]);
        }
        outlet_list(x->f_out_colrow, &s_list, (int)x->f_size.x, av);
        free(av);
    }
}

void matrixctrl_getcolumn(t_matrixctrl *x, float f)
{
    t_atom* av;
    if(f >= 0 && f < x->f_size.x)
    {
        av = (t_atom *)malloc(x->f_size.y * sizeof(t_atom));
        for(long i = 0; i < x->f_size.y; i++)
        {
            atom_setfloat(av+i, x->f_values[i * (long)x->f_size.x + (long)f]);
        }
        outlet_list(x->f_out_colrow, &s_list, (int)x->f_size.y, av);
        free(av);
    }
}

void matrixctrl_set(t_matrixctrl *x, t_symbol *s, int ac, t_atom *av)
{
    if(ac && av)
    {
        for(long i = 2; i < ac; i += 3)
        {
            if(atom_gettype(av+i-2) == A_FLOAT && atom_gettype(av+i-1) == A_FLOAT && atom_gettype(av+i) == A_FLOAT)
            {
                int column  = atom_getfloat(av+i-2);
                int row     = atom_getfloat(av+i-1);
                char value  = atom_getfloat(av+i);
                if(column < x->f_size.x && row < x->f_size.y)
                {
                    x->f_values[row * (long)x->f_size.x + column] = value;
                }
            }
        }

        ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
        ebox_redraw((t_ebox *)x);
    }
}

void matrixctrl_list(t_matrixctrl *x, t_symbol *s, int ac, t_atom *av)
{
    if(ac && av)
    {
        for(long i = 2; i < ac; i += 3)
        {
            if(atom_gettype(av+i-2) == A_FLOAT && atom_gettype(av+i-1) == A_FLOAT && atom_gettype(av+i) == A_FLOAT)
            {
                int column  = atom_getfloat(av+i-2);
                int row     = atom_getfloat(av+i-1);
                char value  = atom_getfloat(av+i);
                if(column < x->f_size.x && row < x->f_size.y)
                {
                    x->f_values[row * (long)x->f_size.x + column] = value;
                    matrixctrl_output(x, column, row);
                }
            }
        }

        ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
        ebox_redraw((t_ebox *)x);
    }
}

void matrixctrl_output(t_matrixctrl *x, int i, int j)
{
    t_atom av[3];
    if(i < x->f_size.x && j < x->f_size.y)
    {
        atom_setfloat(av, i);
        atom_setfloat(av+1, j);
        atom_setfloat(av+2,  x->f_values[j * (long)x->f_size.x + i]);
    }
    outlet_list(x->f_out_cell, &s_list, 3, av);
    if(ebox_getsender((t_ebox *) x))
        pd_list(ebox_getsender((t_ebox *) x), &s_list, 3, av);}

void matrixctrl_free(t_matrixctrl *x)
{
	ebox_free((t_ebox *)x);
    free(x->f_values);
}

void matrixctrl_assist(t_matrixctrl *x, void *b, long m, long a, char *s)
{
	;
}

t_pd_err matrixctrl_notify(t_matrixctrl *x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
	if (msg == cream_sym_attr_modified)
	{
        ebox_redraw((t_ebox *)x);
	}
	return 0;
}

void matrixctrl_paint(t_matrixctrl *x, t_object *view)
{
	t_rect rect;
	ebox_get_rect_for_view((t_ebox *)x, &rect);
    draw_background(x, view, &rect);
}

void draw_background(t_matrixctrl *x, t_object *view, t_rect *rect)
{
	t_elayer *g = ebox_start_layer((t_ebox *)x, cream_sym_background_layer, rect->width, rect->height);
    int i, j, incx, incY;
    int block_width = rect->width / x->f_size.x;
    int block_height = rect->height / x->f_size.y;
	if (g)
	{
        for(incx = 0, i = 0; i < x->f_size.x; i++, incx += block_width)
        {
            for(incY = 0, j = 0; j < x->f_size.y; j++, incY += block_height)
            {
                if(x->f_values[j * (long)x->f_size.x + i])
                {
                    egraphics_set_color_rgba(g, &x->f_color_on);
                    egraphics_rectangle_rounded(g, incx+1, incY+1, block_width-2, block_height-2, 1);
                    egraphics_fill(g);
                }
                egraphics_set_color_rgba(g, &x->f_color_border);
                egraphics_rectangle_rounded(g, incx+1, incY+1, block_width-2, block_height-2, 1);
                egraphics_stroke(g);
            }
        }

        ebox_end_layer((t_ebox*)x, cream_sym_background_layer);
	}
	ebox_paint_layer((t_ebox *)x, cream_sym_background_layer, 0, 0);
}

void matrixctrl_mousedown(t_matrixctrl *x, t_object *patcherview, t_pt pt, long modifiers)
{
    x->f_selected.x = (int)pd_clip_minmax((int)(pt.x / (x->j_box.b_rect.width / (float)x->f_size.x)), 0, x->f_size.x-1);
    x->f_selected.y = (int)pd_clip_minmax((int)(pt.y / (x->j_box.b_rect.height / (float)x->f_size.y)), 0, x->f_size.y-1);
    if(x->f_selected.x >= 0 && x->f_selected.x < x->f_size.x && x->f_selected.y >= 0 && x->f_selected.y < x->f_size.y)
    {
        x->f_values[(long)x->f_selected.y * (long)x->f_size.x + (long)x->f_selected.x] = !x->f_values[(long)x->f_selected.y * (long)x->f_size.x + (long)x->f_selected.x];
        ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
        ebox_redraw((t_ebox *)x);
        matrixctrl_output(x, x->f_selected.x, x->f_selected.y);
    }
}

void matrixctrl_mousedrag(t_matrixctrl *x, t_object *patcherview, t_pt pt, long modifiers)
{
    t_pt newpt;
    newpt.x = (int)pd_clip_minmax((int)(pt.x / (x->j_box.b_rect.width / (float)x->f_size.x)), 0, x->f_size.x-1);
    newpt.y = (int)pd_clip_minmax((int)(pt.y / (x->j_box.b_rect.height / (float)x->f_size.y)), 0, x->f_size.y-1);
    if(newpt.x != x->f_selected.x || newpt.y != x->f_selected.y)
    {
        x->f_selected.x = newpt.x;
        x->f_selected.y = newpt.y;
        if(x->f_selected.x >= 0 && x->f_selected.x < x->f_size.x && x->f_selected.y >= 0 && x->f_selected.y < x->f_size.y)
        {
            x->f_values[(long)x->f_selected.y * (long)x->f_size.x + (long)x->f_selected.x] = !x->f_values[(long)x->f_selected.y * (long)x->f_size.x + (long)x->f_selected.x];
            ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
            ebox_redraw((t_ebox *)x);
            matrixctrl_output(x, x->f_selected.x, x->f_selected.y);
        }
    }
}

void matrixctrl_mouseleave(t_matrixctrl *x, t_object *patcherview, t_pt pt, long modifiers)
{
    x->f_selected.x = -1;
    x->f_selected.y = -1;
}

void matrixctrl_preset(t_matrixctrl *x, t_binbuf *b)
{
    t_atom* av = (t_atom *)malloc(x->f_size.x * x->f_size.y * 3 * sizeof(t_atom));
    long ac = 0;
    for(long i = 0; i < (long)x->f_size.y; i++)
    {
        for(long j = 0; j < (long)x->f_size.x; j++)
        {
            atom_setfloat(av+ac, j);
            atom_setfloat(av+ac+1, i);
            atom_setfloat(av+ac+2, x->f_values[i * (long)x->f_size.x + j]);
            ac += 3;
        }
    }

    binbuf_addv(b, (char *)"s", gensym("list"));
    binbuf_add(b, x->f_size.x * x->f_size.y * 3, av);
    free(av);
}

t_pd_err matrixctrl_matrix_set(t_matrixctrl *x, t_object *attr, int ac, t_atom *av)
{
    if(ac > 1 && av && atom_gettype(av) == A_FLOAT && atom_gettype(av+1) == A_FLOAT)
    {
        if(x->f_values)
        {
            free(x->f_values);
        }
        x->f_size.x = (int)pd_clip_min(atom_getfloat(av), 1);
        x->f_size.y = (int)pd_clip_min(atom_getfloat(av+1), 1);
        x->f_values = (char *)malloc(x->f_size.x * x->f_size.y * sizeof(char));
        memset(x->f_values, 0, x->f_size.x * x->f_size.y * sizeof(char));
        ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
        ebox_redraw((t_ebox *)x);
    }
    return 0;
}




