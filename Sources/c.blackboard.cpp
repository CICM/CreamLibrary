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

typedef struct  _blackboard
{
	t_ebox      j_box;
    t_rect      f_box;
    
    long        f_pen_mode;
    t_pt        f_pen_new;
    t_pt        f_pen_old;
    char        f_pen_down;
    
    long        f_width;
    t_symbol*   f_color;
    char        f_fill;
    
    t_outlet*   f_out_move;
    t_outlet*   f_out_drag;
    t_outlet*   f_out_down;
    
    t_rgba		f_color_background;
	t_rgba		f_color_border;
    char**      f_instructions;
    int         f_ninstructions;
    static const int maxcmd = 1000;
} t_blackboard;

t_eclass *blackboard_class;

static void blackboard_getdrawparams(t_blackboard *x, t_object *view, t_edrawparams *params)
{
	params->d_borderthickness   = 2;
	params->d_cornersize        = 2;
    params->d_bordercolor       = x->f_color_border;
    params->d_boxfillcolor      = x->f_color_background;
}

static void blackboard_oksize(t_blackboard *x, t_rect *newrect)
{
    newrect->width = pd_clip_min(newrect->width, 5.);
    newrect->height = pd_clip_min(newrect->height, 5.);
}

static void blackboard_output(t_blackboard *x)
{
    t_atom argv[2];
    ;
}

static t_pd_err blackboard_notify(t_blackboard *x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
	if(msg == cream_sym_attr_modified)
	{
        ebox_redraw((t_ebox *)x);
	}
	return 0;
}

static void blackboard_clear(t_blackboard *x)
{
    ;
    ebox_redraw((t_ebox *)x);
}

static void blackboard_reset(t_blackboard *x)
{
    x->f_pen_new.x    = 0.;
    x->f_pen_new.y    = 0.;
    x->f_pen_old.x    = 0.;
    x->f_pen_old.y    = 0.;
    x->f_width        = 1;
    x->f_fill         = 0;
    x->f_color        = gensym("#000000");
    
    blackboard_clear(x);
}

static void blackboard_width(t_blackboard *x, float f)
{
    x->f_width = pd_clip_min(f, 1);
}

static void blackboard_color(t_blackboard *x, t_symbol *s, int argc, t_atom *argv)
{
    t_rgb color_rgb = {0., 0., 0.};
    t_hsl color_hsl = {0., 0., 0.};
    if(argc > 1 && atom_gettype(argv) == A_SYMBOL)
    {
        if(atom_getsymbol(argv) == gensym("rgba") || atom_getsymbol(argv) == gensym("rgb"))
        {
            if(atom_gettype(argv+1) == A_FLOAT)
                color_rgb.red = atom_getfloat(argv+1);
            if(argc > 2 && atom_gettype(argv+2) == A_FLOAT)
                color_rgb.green = atom_getfloat(argv+2);
            if(argc > 3 && atom_gettype(argv+3) == A_FLOAT)
                color_rgb.blue = atom_getfloat(argv+3);
            x->f_color = gensym(rgb_to_hex(&color_rgb));
        }
        else if(atom_getsymbol(argv) == gensym("hsla") || atom_getsymbol(argv) == gensym("hsl"))
        {
            if(atom_gettype(argv+1) == A_FLOAT)
                color_hsl.hue = atom_getfloat(argv+1);
            if(argc > 2 && atom_gettype(argv+2) == A_FLOAT)
                color_hsl.saturation = atom_getfloat(argv+2);
            if(argc > 3 && atom_gettype(argv+3) == A_FLOAT)
                color_hsl.lightness = atom_getfloat(argv+3);
            x->f_color = gensym(hsl_to_hex(&color_hsl));
        }
        else if(atom_getsymbol(argv) == gensym("hex") || atom_getsymbol(argv) == gensym("hexa"))
        {
            if(atom_gettype(argv+1) == A_SYMBOL)
                x->f_color = atom_getsymbol(argv+1);
        }
    }
}

static void blackboard_fill(t_blackboard *x, float f)
{
    x->f_fill = pd_clip(f, 0, 1);
}

static void blackboard_line(t_blackboard *x, t_symbol *s, int argc, t_atom *argv)
{
    ;
}

static void blackboard_path(t_blackboard *x, t_symbol *s, int argc, t_atom *argv)
{
    ;
}

static void blackboard_rect(t_blackboard *x, t_symbol *s, int argc, t_atom *argv)
{
    ;
}

static void blackboard_oval(t_blackboard *x, t_symbol *s, int argc, t_atom *argv)
{
    ;
}

static void blackboard_arc(t_blackboard *x, t_symbol *s, int argc, t_atom *argv)
{
    ;
}

static void blackboard_image(t_blackboard *x, t_symbol *s, int argc, t_atom *argv)
{
    ;
    
}

static void blackboard_text(t_blackboard *x, t_symbol *s, int argc, t_atom *argv)
{
    int i;
    char buffer[MAXPDSTRING];
    size_t length;
    if(x->f_ninstructions >= _blackboard::maxcmd)
    {
        pd_error(x, "%s too many drawing commands.", eobj_getclassname(x)->s_name);
        return;
    }
    
    if(argc > 2 && argv)
    {
        if (atom_gettype(argv) == A_FLOAT && atom_gettype(argv+1) == A_FLOAT)
        {
            sprintf(x->f_instructions[x->f_ninstructions], "create text %d %d -anchor nw -text {", (int)atom_getfloat(argv), (int)atom_getfloat(argv+1));
            
            for(i = 2; i < argc; i++)
            {
                atom_string(argv+i, buffer, MAXPDSTRING);
                length = strlen(buffer);
                strncat(x->f_instructions[x->f_ninstructions], " ", 1);
                strncat(x->f_instructions[x->f_ninstructions], buffer, length);
            }
            
            int todo;
            //sprintf(buffer, "} -font {%s %d %s} -fill %s", x->j_box.b_font.c_family->s_name, (int)x->j_box.b_font.c_size, x->j_box.b_font.c_weight->s_name, x->f_color->s_name);
            length = strlen(buffer);
            strncat(x->f_instructions[x->f_ninstructions], buffer, length);
            
            x->f_ninstructions++;
            ebox_redraw((t_ebox *)x);
        }
        
    }
}

static void blackboard_pen(t_blackboard *x)
{
    ;
}

static void blackboard_paint(t_blackboard *x, t_object *view)
{
	t_rect rect;
	ebox_getdrawbounds((t_ebox *)x, view,  &rect);
    
    ;
    x->f_box = rect;
}

static void blackboard_mousemove(t_blackboard *x, t_object *view, t_pt pt, long modifiers)
{
    x->f_pen_new = pt;
    x->f_pen_down = 0;
    blackboard_output(x);
}

static void blackboard_mousedrag(t_blackboard *x, t_object *view, t_pt pt, long modifiers)
{
    x->f_pen_old = x->f_pen_new;
    x->f_pen_new = pt;
    x->f_pen_down = 1;
    if(x->f_pen_mode)
    {
        blackboard_pen(x);
    }
    blackboard_output(x);
    outlet_float(x->f_out_down, 1);
}

static void blackboard_mousedown(t_blackboard *x, t_object *view, t_pt pt, long modifiers)
{
    x->f_pen_new = pt;
    x->f_pen_old = pt;
    x->f_pen_down = 1;
    if(x->f_pen_mode)
    {
        blackboard_pen(x);
    }
    blackboard_output(x);
}

static void blackboard_mouseup(t_blackboard *x, t_object *view, t_pt pt, long modifiers)
{
    x->f_pen_new = pt;
    x->f_pen_down = 0;
    blackboard_output(x);
    outlet_float(x->f_out_down, 0);
}

static void *blackboard_new(t_symbol *s, int argc, t_atom *argv)
{
    t_blackboard *x = (t_blackboard *)eobj_new(blackboard_class);
    t_binbuf* d     = binbuf_via_atoms(argc,argv);
    
    if(x && d)
    {
        ebox_new((t_ebox *)x, 0 | EBOX_GROWINDI);
        
        x->f_out_drag   = outlet_new((t_object *)x, &s_list);
        x->f_out_move   = outlet_new((t_object *)x, &s_list);
        x->f_out_down   = outlet_new((t_object *)x, &s_float);
        x->f_pen_new.x  = 0.;
        x->f_pen_new.y  = 0.;
        x->f_pen_old.x  = 0.;
        x->f_pen_old.y  = 0.;
        x->f_pen_down   = 0;
        
        x->f_width      = 1;
        x->f_color      = gensym("#000000");
        x->f_fill       = 0;
        x->f_ninstructions = 0;
        x->f_instructions = (char **)malloc(_blackboard::maxcmd * sizeof(char*));
        for(int i = 0; i < _blackboard::maxcmd; i++)
        {
            x->f_instructions[i] = (char *)malloc(MAXPDSTRING * sizeof(char));
        }
        
        eobj_attr_read(x, d);
        ebox_ready((t_ebox *)x);
    }

    return (x);
}

static void blackboard_free(t_blackboard *x)
{
    ebox_free((t_ebox *)x);
    for(int i = 0; i < _blackboard::maxcmd; i++)
    {
        free(x->f_instructions[i]);
    }
    free(x->f_instructions);
}

extern "C" void setup_c0x2eblackboard(void)
{
    t_eclass *c;
    
    c = eclass_new("c.blackboard", (t_method)blackboard_new, (t_method)blackboard_free, (short)sizeof(t_blackboard), 0L, A_GIMME, 0);
    eclass_guiinit(c, 0);

    
    eclass_addmethod(c, (t_method) blackboard_paint,           "paint",            A_NULL, 0);
    eclass_addmethod(c, (t_method) blackboard_notify,          "notify",           A_NULL, 0);
    eclass_addmethod(c, (t_method) blackboard_getdrawparams,   "getdrawparams",    A_NULL, 0);
    eclass_addmethod(c, (t_method) blackboard_oksize,          "oksize",           A_NULL, 0);
    
    eclass_addmethod(c, (t_method) blackboard_width,           "width",            A_FLOAT,0);
    eclass_addmethod(c, (t_method) blackboard_color,           "color",            A_GIMME,0);
    eclass_addmethod(c, (t_method) blackboard_fill,            "fill",             A_FLOAT,0);
    
    eclass_addmethod(c, (t_method) blackboard_line,            "line",             A_GIMME,0);
    eclass_addmethod(c, (t_method) blackboard_path,            "path",             A_GIMME,0);
    eclass_addmethod(c, (t_method) blackboard_rect,            "rect",             A_GIMME,0);
    eclass_addmethod(c, (t_method) blackboard_oval,            "oval",             A_GIMME,0);
    eclass_addmethod(c, (t_method) blackboard_arc,             "arc",              A_GIMME,0);
    eclass_addmethod(c, (t_method) blackboard_image,           "image",            A_GIMME,0);
    eclass_addmethod(c, (t_method) blackboard_text,            "text",             A_GIMME,0);
    
    eclass_addmethod(c, (t_method) blackboard_clear,           "clear",            A_NULL, 0);
    eclass_addmethod(c, (t_method) blackboard_reset,           "reset",            A_NULL, 0);
    
    eclass_addmethod(c, (t_method) blackboard_mousemove,       "mousemove",        A_NULL, 0);
    eclass_addmethod(c, (t_method) blackboard_mousedrag,       "mousedrag",        A_NULL, 0);
    eclass_addmethod(c, (t_method) blackboard_mousedown,       "mousedown",        A_NULL, 0);
    eclass_addmethod(c, (t_method) blackboard_mouseup,         "mouseup",          A_NULL, 0);
    
    CLASS_ATTR_DEFAULT              (c, "size", 0, "200 200");
    CLASS_ATTR_INVISIBLE            (c, "send", 1);
    
    CLASS_ATTR_LONG                 (c, "chalkmode", 0, t_blackboard, f_pen_mode);
    CLASS_ATTR_LABEL                (c, "chalkmode", 0, "Chalk Mode");
    CLASS_ATTR_FILTER_CLIP          (c, "chalkmode", 0, 1);
    CLASS_ATTR_ORDER                (c, "chalkmode", 0, "1");
    CLASS_ATTR_DEFAULT              (c, "chalkmode", 0, "1");
    CLASS_ATTR_SAVE                 (c, "chalkmode", 0);
    CLASS_ATTR_STYLE                (c, "chalkmode", 0, "onoff");
    
    CLASS_ATTR_RGBA                 (c, "bgcolor", 0, t_blackboard, f_color_background);
    CLASS_ATTR_LABEL                (c, "bgcolor", 0, "Background Color");
    CLASS_ATTR_ORDER                (c, "bgcolor", 0, "1");
    CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bgcolor", 0, "0.75 0.75 0.75 1.");
    CLASS_ATTR_STYLE                (c, "bgcolor", 0, "color");
    
    CLASS_ATTR_RGBA                 (c, "bdcolor", 0, t_blackboard, f_color_border);
    CLASS_ATTR_LABEL                (c, "bdcolor", 0, "Border Color");
    CLASS_ATTR_ORDER                (c, "bdcolor", 0, "2");
    CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bdcolor", 0, "0.5 0.5 0.5 1.");
    CLASS_ATTR_STYLE                (c, "bdcolor", 0, "color");
    
    eclass_register(CLASS_BOX, c);
    blackboard_class = c;
}






