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

typedef struct  _matrixctrl
{
	t_ebox      j_box;
    t_outlet*   f_out_cell;
    t_outlet*   f_out_colrow;
    int         f_size[2];
    int         f_selected;
	t_rgba		f_color_background;
	t_rgba		f_color_border;
    t_rgba      f_color_on;

} t_matrixctrl;

static t_eclass *matrixctrl_class;

static void matrixctrl_bang(t_matrixctrl *x)
{
    int i, j, index;
    t_atom av[3];
    t_pd* send = ebox_getsender((t_ebox *) x);
    const int flags = ebox_parameter_getvalue((t_ebox *)x, 1);
    ebox_parameter_notify_changes((t_ebox *)x, 1);
    for(i = 0; i < x->f_size[0]; i++)
    {
        for(j = 0; j < x->f_size[1]; j++)
        {
            index = i * x->f_size[1] + j;
            atom_setfloat(av, (float)i);
            atom_setfloat(av+1, (float)j);
            atom_setfloat(av+2,  (float)(flags & (1<<index)));
            outlet_list(x->f_out_cell, &s_list, 3, av);
            if(send)
            {
                pd_list(send, &s_list, 3, av);
            }
        }
    }
}

static void matrixctrl_getrow(t_matrixctrl *x, float f)
{
    int i, index;
    t_atom* av;
    const int j = (int)f;
    const int flags = ebox_parameter_getvalue((t_ebox *)x, 1);
    if(j >= 0 && j < x->f_size[1])
    {
        av = (t_atom *)malloc((size_t)x->f_size[0] * sizeof(t_atom));
        if(av)
        {
            for(i = 0; i < x->f_size[0]; i++)
            {
                index = i * x->f_size[1] + j;
                atom_setfloat(av+i, (flags & (1<<index)) ? 1.f : 0.f);
            }
            outlet_list(x->f_out_colrow, &s_list, x->f_size[0], av);
            free(av);
        }
    }

}

static void matrixctrl_getcolumn(t_matrixctrl *x, float f)
{
    int j, index;
    t_atom* av;
    const int i = (int)f;
    const int flags = ebox_parameter_getvalue((t_ebox *)x, 1);
    if(i >= 0 && i < x->f_size[0])
    {
        av = (t_atom *)malloc((size_t)x->f_size[1] * sizeof(t_atom));
        if(av)
        {
            for(j = 0; j < x->f_size[1]; j++)
            {
                index = i * x->f_size[1] + j;
                atom_setfloat(av+j, (flags & (1<<index)) ? 1.f : 0.f);
            }
            outlet_list(x->f_out_colrow, &s_list, x->f_size[1], av);
            free(av);
        }
    }
}

static void matrixctrl_clear(t_matrixctrl *x)
{
    int i, j, index;
    t_atom av[3];
    t_pd* send = ebox_getsender((t_ebox *) x);
    int flags = ebox_parameter_getvalue((t_ebox *)x, 1);
    for(i = 0; i < x->f_size[0]; i++)
    {
        for(j = 0; j < x->f_size[1]; j++)
        {
            index = i * x->f_size[1] + j;
            if(flags & (1<<index))
            {
                flags = flags & ~(1<<index);
                atom_setfloat(av, (float)i);
                atom_setfloat(av+1, (float)j);
                atom_setfloat(av+2,  0.f);
                outlet_list(x->f_out_cell, &s_list, 3, av);
                if(send)
                {
                    pd_list(send, &s_list, 3, av);
                }
            }
        }
    }
    ebox_parameter_setvalue((t_ebox *)x, 1, (float)flags, 1);
    ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
    ebox_redraw((t_ebox *)x);
}

static void matrixctrl_list(t_matrixctrl *x, t_symbol *s, int argc, t_atom *argv)
{
    t_atom av[3];
    int i, j, index;
    char val;
    t_pd* send = ebox_getsender((t_ebox *) x);
    int flags = ebox_parameter_getvalue((t_ebox *)x, 1);
    if(argc > 2 && argv && atom_gettype(argv) == A_FLOAT && atom_gettype(argv+1) == A_FLOAT && atom_gettype(argv+2) == A_FLOAT)
    {
        i   = (int)atom_getfloat(argv);
        j   = (int)atom_getfloat(argv+1);
        val = (atom_getfloat(argv+2) == 0.f) ? 0 : 1;
        index = i * x->f_size[1] + j;
        if(flags & (1<<index) && !val)
        {
            flags = flags & ~(1<<index);
            ebox_parameter_setvalue((t_ebox *)x, 1, (float)flags, 1);
            atom_setfloat(av, (float)i);
            atom_setfloat(av+1, (float)j);
            atom_setfloat(av+2, 0.f);
            outlet_list(x->f_out_cell, &s_list, 3, av);
            if(send)
            {
                pd_list(send, &s_list, 3, av);
            }
            ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
            ebox_redraw((t_ebox *)x);
        }
        else if(!(flags & (1<<index)) && val)
        {
            flags = flags | (1<<index);
            ebox_parameter_setvalue((t_ebox *)x, 1, (float)flags, 1);
            atom_setfloat(av, (float)i);
            atom_setfloat(av+1, (float)j);
            atom_setfloat(av+2, 1.f);
            outlet_list(x->f_out_cell, &s_list, 3, av);
            if(send)
            {
                pd_list(send, &s_list, 3, av);
            }
            ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
            ebox_redraw((t_ebox *)x);
        }
    }
}

static void matrixctrl_set(t_matrixctrl *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, index;
    char val;
    int flags = ebox_parameter_getvalue((t_ebox *)x, 1);
    if(argc > 2 && argv && atom_gettype(argv) == A_FLOAT && atom_gettype(argv+1) == A_FLOAT && atom_gettype(argv+2) == A_FLOAT)
    {
        i   = (int)atom_getfloat(argv);
        j   = (int)atom_getfloat(argv+1);
        val = (atom_getfloat(argv+2) == 0.f) ? 0 : 1;
        index = i * x->f_size[1] + j;
        if(flags & (1<<index) && !val)
        {
            flags = flags & ~(1<<index);
            ebox_parameter_setvalue((t_ebox *)x, 1, (float)flags, 0);
            ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
            ebox_redraw((t_ebox *)x);
        }
        else if(!(flags & (1<<index)) && val)
        {
            flags = flags | (1<<index);
            ebox_parameter_setvalue((t_ebox *)x, 1, (float)flags, 0);
            ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
            ebox_redraw((t_ebox *)x);
        }
    }
}

static void matrixctrl_getdrawparams(t_matrixctrl *x, t_object *patcherview, t_edrawparams *params)
{
    params->d_borderthickness   = 2;
    params->d_cornersize        = 2;
    params->d_bordercolor       = x->f_color_border;
    params->d_boxfillcolor      = x->f_color_background;
}

static void matrixctrl_oksize(t_matrixctrl *x, t_rect *newrect)
{
    newrect->width = pd_clip_min(newrect->width, x->f_size[0] * 17.f);
    newrect->height = pd_clip_min(newrect->height, x->f_size[1] * 17.f);
}

static t_pd_err matrixctrl_notify(t_matrixctrl *x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
	if(msg == cream_sym_attr_modified)
	{
        if(s == cream_sym_bgcolor || s  == cream_sym_bdcolor || s == cream_sym_accolor)
        {
            ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
        }
	}
    else if(msg == cream_sym_value_changed)
    {
        matrixctrl_bang(x);
        ebox_invalidate_layer((t_ebox *)x, cream_sym_points_layer);
        ebox_redraw((t_ebox *)x);
    }
	return 0;
}

static void matrixctrl_paint(t_matrixctrl *x, t_object *view)
{
	t_rect rect;
	ebox_get_rect_for_view((t_ebox *)x, &rect);
    
    t_elayer *g = ebox_start_layer((t_ebox *)x, cream_sym_background_layer, rect.width, rect.height);
    if(g)
    {
        const float block_width = rect.width / float(x->f_size[0]);
        const float block_height = rect.height / float(x->f_size[1]);
        const int flags = (int)ebox_parameter_getvalue((t_ebox *)x, 1);
        egraphics_set_line_width(g, 2.f);
        
        float incx = 0.f;
        for(int i = 0; i < x->f_size[0]; i++)
        {
            float incY = 0.f;
            for(int j = 0; j < x->f_size[1]; j++)
            {
                const int index = i * x->f_size[1] + j;
                if(flags & (1<<index))
                {
                    egraphics_rectangle(g, incx + 4.f, incY + 4.f, block_width - 8.f, block_height - 8.f);
                    egraphics_set_color_rgba(g, &x->f_color_on);
                    egraphics_fill(g);
                }
                egraphics_rectangle(g, incx + 2.f, incY + 2.f, block_width - 4.f, block_height - 4.f);
                egraphics_set_color_rgba(g, &x->f_color_border);
                egraphics_stroke(g);
                incY += block_height;
            }
            incx += block_width;
        }
        
        ebox_end_layer((t_ebox*)x, cream_sym_background_layer);
    }
    ebox_paint_layer((t_ebox *)x, cream_sym_background_layer, 0, 0);
}

static void matrixctrl_mousedown(t_matrixctrl *x, t_object *patcherview, t_pt pt, long modifiers)
{
    int todo_clean_last_column;
    t_atom av[3];
    t_pd* send = ebox_getsender((t_ebox *) x);
    t_rect rect;
    ebox_get_rect_for_view((t_ebox *)x, &rect);
    const int i = (int)pd_clip(pt.x / (rect.width / (float)x->f_size[0]), 0.f, (float)x->f_size[0] - 1.f);
    const int j = (int)pd_clip(pt.y / (rect.height / (float)x->f_size[1]), 0.f, (float)x->f_size[1] - 1.f);
    const int flags = (int)ebox_parameter_getvalue((t_ebox *)x, 1);
    ebox_parameter_begin_changes((t_ebox *)x, 1);
    x->f_selected = i * x->f_size[1] + j;
    atom_setfloat(av, (float)i);
    atom_setfloat(av+1, (float)j);
    if(flags & (1<<x->f_selected))
    {
        ebox_parameter_setvalue((t_ebox *)x, 1, (float)(flags & ~(1<<x->f_selected)), 1);
        atom_setfloat(av+2,  0.f);
    }
    else
    {
        ebox_parameter_setvalue((t_ebox *)x, 1, (float)(flags | (1<<x->f_selected)), 1);
        atom_setfloat(av+2,  1.f);
    }
    outlet_list(x->f_out_cell, &s_list, 3, av);
    if(send)
    {
        pd_list(send, &s_list, 3, av);
    }
    ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
    ebox_redraw((t_ebox *)x);
}

static void matrixctrl_mousedrag(t_matrixctrl *x, t_object *patcherview, t_pt pt, long modifiers)
{
    t_atom av[3];
    t_pd* send = ebox_getsender((t_ebox *) x);
    t_rect rect;
    ebox_get_rect_for_view((t_ebox *)x, &rect);
    const int i = (int)pd_clip(pt.x / (rect.width / (float)x->f_size[0]), 0.f, (float)x->f_size[0] - 1.f);
    const int j = (int)pd_clip(pt.y / (rect.height / (float)x->f_size[1]), 0.f, (float)x->f_size[1] - 1.f);
    const int flags = (int)ebox_parameter_getvalue((t_ebox *)x, 1);
    if(i * x->f_size[1] + j != x->f_selected)
    {
        x->f_selected = i * x->f_size[1] + j;
        atom_setfloat(av, (float)i);
        atom_setfloat(av+1, (float)j);
        if(flags & (1<<x->f_selected))
        {
            ebox_parameter_setvalue((t_ebox *)x, 1, (float)(flags & ~(1<<x->f_selected)), 1);
            atom_setfloat(av+2,  0.f);
        }
        else
        {
            ebox_parameter_setvalue((t_ebox *)x, 1, (float)(flags | (1<<x->f_selected)), 1);
            atom_setfloat(av+2,  1.f);
        }
        outlet_list(x->f_out_cell, &s_list, 3, av);
        if(send)
        {
            pd_list(send, &s_list, 3, av);
        }
        ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
        ebox_redraw((t_ebox *)x);
    }
}

static void matrixctrl_mouseup(t_matrixctrl *x, t_object *patcherview, t_pt pt, long modifiers)
{
    x->f_selected = -1;
    ebox_parameter_end_changes((t_ebox *)x, 1);
}

static t_pd_err matrixctrl_matrix_set(t_matrixctrl *x, t_object *attr, int ac, t_atom *av)
{
    if(ac > 1 && av && atom_gettype(av) == A_FLOAT && atom_gettype(av+1) == A_FLOAT)
    {
        x->f_size[0] = pd_clip(atom_getfloat(av), 1, CREAM_MAXITEMS);
        x->f_size[1] = pd_clip(atom_getfloat(av+1), 1, CREAM_MAXITEMS);
        ebox_parameter_setvalue((t_ebox *)x, 1, 0, 0);
        ebox_parameter_setminmax((t_ebox *)x, 1, 0.f, powf(2., (float)(x->f_size[0] * x->f_size[1])) - 1.f);
        ebox_parameter_setnstep((t_ebox *)x, 1, (int)powf(2., (float)(x->f_size[0] * x->f_size[1])));
        ebox_notify((t_ebox *)x, s_cream_size, cream_sym_attr_modified, NULL, NULL);
    }
    return 0;
}

static void *matrixctrl_new(t_symbol *s, int argc, t_atom *argv)
{
    t_matrixctrl *x = (t_matrixctrl *)eobj_new(matrixctrl_class);
    t_binbuf* d = binbuf_via_atoms(argc,argv);
    
    if(x && d)
    {
        ebox_new((t_ebox *)x, 0 | EBOX_GROWINDI);
        ebox_parameter_create((t_ebox *)x, 1);
        x->f_selected = -1;
        
        x->f_out_cell   = outlet_new((t_object *)x, &s_list);
        x->f_out_colrow = outlet_new((t_object *)x, &s_list);
        
        ebox_attrprocess_viabinbuf(x, d);
        ebox_ready((t_ebox *)x);
        
        return x;
    }
    
    return NULL;
}

extern "C" void setup_c0x2ematrix(void)
{
    t_eclass *c = eclass_new("c.matrix", (method)matrixctrl_new, (method)ebox_free, (short)sizeof(t_matrixctrl), 0L, A_GIMME, 0);
    
    if(c)
    {
        eclass_guiinit(c, 0);
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
        eclass_addmethod(c, (method) matrixctrl_mouseup,         "mouseup",          A_NULL, 0);
        
        CLASS_ATTR_DEFAULT              (c, "size", 0, "105 53");
        
        CLASS_ATTR_INT_ARRAY            (c, "matrix", 0, t_matrixctrl, f_size, 2);
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
}




