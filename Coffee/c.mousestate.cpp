/*
 * PdEnhanced - Pure Data Enhanced
 *
 * An add-on for Pure Data
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

#include "../c.library.h"

/*
typedef struct _erouter
{
    t_object            e_obj;
    t_object**          e_childs;
    long                e_nchilds;
    t_clock*            e_clock;
    t_pt                e_mouse_global_position;
    long                e_mouse_modifier;
    char                e_mouse_down;
}t_erouter;

static t_erouter* erouter_setup();

static void erouter_anything(t_erouter *x, t_symbol *s, int argc, t_atom *argv)
{
    int i;
    t_eobj* z;
    rmethod nrmethod = NULL;
    if(argc >= 1 && argv && atom_gettype(argv) == A_SYMBOL)
    {
        for(i = 0; i < x->e_nchilds; i++)
        {
            if(x->e_childs[i] != NULL)
            {
                z = (t_eobj *)x->e_childs[i];
                if(z->o_id == s)
                {
                    nrmethod = (rmethod)zgetfn((t_pd *)z, atom_getsymbol(argv));
                    if(nrmethod)
                    {
                        pd_typedmess((t_pd *)z, atom_getsymbol(argv), (int)argc, argv);
                    }
                }
            }
        }
    }
}

static void erouter_mousedown(t_erouter *x, t_symbol *s, int argc, t_atom *argv)
{
    x->e_mouse_down = 1;
    x->e_mouse_modifier = (long)atom_getfloat(argv+2);
#ifdef __APPLE__

#elif _WINDOWS

    if(x->e_mouse_modifier >= 131072)
    {
        x->e_mouse_modifier -= 131072;
        x->e_mouse_modifier += EMOD_ALT;
    }
    else
        x->e_mouse_modifier -= 8;
#else
    x->e_mouse_modifier -= 16;
#endif
}

static void erouter_mouseup(t_erouter *x, t_symbol *s, int argc, t_atom *argv)
{
    x->e_mouse_down = 0;
    x->e_mouse_modifier = (long)atom_getfloat(argv+2);
#ifdef __APPLE__

#elif _WINDOWS

    if(x->e_mouse_modifier >= 131072)
    {
        x->e_mouse_modifier -= 131072;
        x->e_mouse_modifier += EMOD_ALT;
    }
    else
        x->e_mouse_modifier -= 8;
#else
    x->e_mouse_modifier -= 16;
#endif
}

static void erouter_mousemove(t_erouter *x, t_symbol *s, int argc, t_atom *argv)
{
    x->e_mouse_modifier = (long)atom_getfloat(argv+2);
#ifdef __APPLE__

#elif _WINDOWS

    if(x->e_mouse_modifier >= 131072)
    {
        x->e_mouse_modifier -= 131072;
        x->e_mouse_modifier += EMOD_ALT;
    }
    else
        x->e_mouse_modifier -= 8;
#else
    x->e_mouse_modifier -= 16;
#endif
}

static void erouter_tick(t_erouter * x)
{
    sys_gui("erouter_global_mouse\n");
    clock_delay(x->e_clock, 20.);
}

static void erouter_mouseglobal(t_erouter *x, float px, float py)
{
    x->e_mouse_global_position.x = px;
    x->e_mouse_global_position.y = py;
}

static t_pt erouter_getmouse_global_position(void)
{
    t_pt pt;
    t_erouter *x = (t_erouter *)erouter_setup();
    if(x)
    {
        return x->e_mouse_global_position;
    }
    else
    {
        pt.x = 0;
        pt.y = 0;
        return pt;
    }
}

static long erouter_getmouse_modifier(void)
{
    t_erouter *x = (t_erouter *)erouter_setup();
    if(x)
    {
        if(x->e_mouse_modifier >= 256)
            return x->e_mouse_modifier - 256;
        else
            return x->e_mouse_modifier;
    }
    else
    {
        return 0;
    }
}

static char erouter_getmouse_status(void)
{
    t_erouter *x = (t_erouter *)erouter_setup();
    if(x)
    {
        return x->e_mouse_down;
    }
    else
    {
        return 0;
    }
}

static void erouter_free(t_erouter *x)
{
    if(x->e_childs)
    {
        free(x->e_childs);
        x->e_childs = NULL;
    }
    pd_unbind((t_pd *)x, gensym("erouter1572"));
    if(x->e_clock)
    {
        clock_free(x->e_clock);
    }
}

static void eobj_detach_torouter(t_object* child)
{
    int i;
    t_erouter *x = (t_erouter *)erouter_setup();
    if(x)
    {
        for(i = 0; i < x->e_nchilds; i++)
        {
            if(x->e_childs[i] == child)
            {
                if(i != x->e_nchilds - 1)
                {
                    memcpy(x->e_childs+i, x->e_childs+i+1, x->e_nchilds-i-1);
                }
                x->e_nchilds--;
                if(!x->e_nchilds)
                {
                    free(x->e_childs);
                    x->e_childs = NULL;
                }
                else
                {
                    x->e_childs = realloc(x->e_childs, (unsigned long)(x->e_nchilds) * sizeof(t_object *));
                    if(!x->e_childs)
                    {
                        x->e_nchilds = 0;
                    }
                }
                return;
            }
        }
    }
}

static void eobj_attach_torouter(t_object* child)
{
    int i;
    t_erouter *x = (t_erouter *)erouter_setup();
    if(x)
    {
        if(x->e_clock)
        {
            clock_unset(x->e_clock);
        }
        for(i = 0; i < x->e_nchilds; i++)
        {
            if(x->e_childs[i] == child)
            {
                return;
            }
        }
        if(!x->e_nchilds || x->e_childs == NULL)
        {
            x->e_childs = (t_object **)malloc(sizeof(t_object *));
            if(x->e_childs)
            {
                x->e_childs[0]  = child;
                x->e_nchilds    = 1;
                return;
            }
            else
            {
                x->e_childs     = NULL;
                x->e_nchilds    = 0;
                return;
            }
        }
        else
        {
            for(i = 0; i < x->e_nchilds; i++)
            {
                if(x->e_childs[i] == NULL)
                {
                    x->e_childs[i]  = child;
                    return;
                }
            }

            x->e_childs = (t_object **)realloc(x->e_childs, (unsigned long)(x->e_nchilds + 1) * sizeof(t_object *));
            if(x->e_childs)
            {
                x->e_childs[x->e_nchilds]  = child;
                x->e_nchilds++;
            }
            else
            {
                x->e_childs     = NULL;
                x->e_nchilds    = 0;
            }
        }
        if(!x->e_clock)
        {
            x->e_clock = clock_new(x, (t_method)erouter_tick);
        }
        clock_set(x->e_clock, 20);
    }
}

static t_erouter* erouter_setup()
{
    t_erouter *x;
    t_class* c = NULL;
    int right = EMOD_CMD;
#ifdef __APPLE__

#elif _WINDOWS
    right += 8;
#else
    right += 16;
#endif
    t_symbol* erouter1572obj_sym = gensym("erouter1572obj");
    t_symbol* erouter1572_sym = gensym("erouter1572");
    if(!erouter1572obj_sym->s_thing)
    {
        c = class_new(gensym("erouter"), NULL, (t_method)erouter_free, sizeof(t_erouter), CLASS_PD, A_NULL);
        if (c)
        {
            class_addmethod(c, (t_method)erouter_mousedown,         gensym("mousedown"), A_GIMME, 0);
            class_addmethod(c, (t_method)erouter_mouseup,           gensym("mouseup"), A_GIMME, 0);
            class_addmethod(c, (t_method)erouter_mousemove,         gensym("mousemove"), A_GIMME, 0);
            class_addmethod(c, (t_method)erouter_mouseglobal,       gensym("globalmouse"), A_FLOAT, A_FLOAT, 0);
            class_addanything(c, erouter_anything);
            x = (t_erouter *)pd_new(c);
            if(x)
            {
                x->e_nchilds    = 0;
                x->e_childs     = NULL;
                x->e_clock      = NULL;

                sys_gui("namespace eval erouter1572 {} \n");

                sys_vgui("bind all <Button-3> {+pdsend {%s mousedown %%x %%y %i}}\n", erouter1572_sym->s_name, right);
                sys_vgui("bind all <Button-2> {+pdsend {%s mousedown %%x %%y %i}}\n", erouter1572_sym->s_name, right);
                sys_vgui("bind all <Button-1> {+pdsend {%s mousedown %%x %%y %%s}}\n", erouter1572_sym->s_name);
                sys_vgui("bind all <ButtonRelease> {+pdsend {%s mouseup %%x %%y %%s}}\n", erouter1572_sym->s_name);
                sys_vgui("bind all <Motion> {+pdsend {%s mousemove %%x %%y %%s}}\n", erouter1572_sym->s_name);

                // PATCHER MOUSE POSITION //
                sys_vgui("proc eobj_canvas_mouse {target patcher} {\n");
                sys_gui(" set rx [winfo rootx $patcher]\n");
                sys_gui(" set ry [winfo rooty $patcher]\n");
                sys_gui(" set x  [winfo pointerx .]\n");
                sys_gui(" set y  [winfo pointery .]\n");
                sys_vgui(" pdsend \"%s $target canvasmouse [expr $x - $rx] [expr $y - $ry] \"\n", erouter1572_sym->s_name);
                sys_gui("}\n");

                // GLOBAL MOUSE POSITION //
                sys_gui("proc erouter_global_mouse {} {\n");
                sys_gui(" set x [winfo pointerx .]\n");
                sys_gui(" set y [winfo pointery .]\n");
                sys_vgui(" pdsend \"%s globalmouse $x $y\"\n", erouter1572_sym->s_name);
                sys_gui("}\n");

                pd_bind((t_pd *)x, erouter1572_sym);
                erouter1572obj_sym->s_thing = (t_class **)x;
                return x;
            }
        }
        return NULL;
    }
    else
    {
        return (t_erouter *)erouter1572obj_sym->s_thing;
    }
}

t_pt eobj_get_mouse_global_position(void* x)
{
    return erouter_getmouse_global_position();
}

char eobj_get_mouse_status(void* x)
{
    return erouter_getmouse_status();
}


long eobj_get_mouse_modifier(void* x)
{
    return erouter_getmouse_modifier();
}

typedef struct  _mousestate
{
	t_eobj      l_box;
	t_outlet*   l_mouse_pressed;
    t_outlet*   l_mouse_x;
    t_outlet*   l_mouse_y;
    t_outlet*   l_mouse_deltax;
    t_outlet*   l_mouse_deltay;
    t_outlet*   l_mouse_modifier;
    t_clock*    l_clock;
    t_pt        l_mouse;
    t_pt        l_mouse_zero;
    char        l_zero;
    int         l_mode;
    t_clock*            o_clock;
} t_mousestate;

static t_eclass *mousestate_class;

static void mousestate_poll_mouse(void* x)
{
    t_eobj* obj  = (t_eobj *)x;
    clock_set(obj->o_clock, 20.);
}


static void mousestate_nopoll_mouse(void* x)
{
    t_eobj* obj  = (t_eobj *)x;
    clock_unset(obj->o_clock);
}

static void mousestate_tick(t_eobj* x)
{
    sys_vgui("eobj_canvas_mouse %s %s\n", x->o_id->s_name, x->o_canvas_id->s_name);
    clock_delay(x->o_clock, 20.);
}

static void mousestate_reset(t_mousestate *x)
{
    x->l_zero = 0;
}

static void mousestate_zero(t_mousestate *x)
{
    if(x->l_mode == 1)
        x->l_mouse_zero = eobj_get_mouse_canvas_position(x);
    else
        x->l_mouse_zero = eobj_get_mouse_global_position(x);
    x->l_zero = 1;
}

static void mousestate_mode(t_mousestate *x, float f)
{
    int mode;
    if(f == 0)
        mode = 0;
    else
        mode = 1;
    if(x->l_mode != mode)
    {
        x->l_mode = mode;
        if(x->l_mode == 1)
        {
            mousestate_poll_mouse(x);
            x->l_mouse = eobj_get_mouse_canvas_position(x);
        }
        else
        {
            mousestate_nopoll_mouse(x);
            x->l_mouse = eobj_get_mouse_global_position(x);
        }
        x->l_zero = 0;
    }
}

static void mousestate_tick(t_mousestate *x)
{
    t_pt mouse;
    if(x->l_mode == 1)
        mouse = eobj_get_mouse_canvas_position(x);
    else
        mouse = eobj_get_mouse_global_position(x);
    if(x->l_zero)
    {
        mouse.x -= x->l_mouse_zero.x;
        mouse.y -= x->l_mouse_zero.y;
    }

    outlet_float(x->l_mouse_modifier, eobj_get_mouse_modifier(x));
    outlet_float(x->l_mouse_deltay, mouse.y - x->l_mouse.y);
    outlet_float(x->l_mouse_deltax, mouse.x - x->l_mouse.x);
    outlet_float(x->l_mouse_y, mouse.y);
    outlet_float(x->l_mouse_x, mouse.x);
    outlet_float(x->l_mouse_pressed, eobj_get_mouse_status(x));

    x->l_mouse = mouse;
    clock_delay(x->l_clock, 20);
}

static void mousestate_poll(t_mousestate *x)
{
    t_pt mouse;
    if(x->l_mode == 1)
        mouse = eobj_get_mouse_canvas_position(x);
    else
        mouse = eobj_get_mouse_global_position(x);
    if(x->l_zero)
    {
        mouse.x -= x->l_mouse_zero.x;
        mouse.y -= x->l_mouse_zero.y;
    }

    outlet_float(x->l_mouse_modifier, eobj_get_mouse_modifier(x));
    outlet_float(x->l_mouse_deltay, mouse.y - x->l_mouse.y);
    outlet_float(x->l_mouse_deltax, mouse.x - x->l_mouse.x);
    outlet_float(x->l_mouse_y, mouse.y);
    outlet_float(x->l_mouse_x, mouse.x);
    outlet_float(x->l_mouse_pressed, eobj_get_mouse_status(x));

    x->l_mouse = mouse;
    clock_set(x->l_clock, 20);
}

static void mousestate_nopoll(t_mousestate *x)
{
    clock_unset(x->l_clock);
}

static void mousestate_free(t_mousestate *x)
{
    mousestate_nopoll_mouse(x);
    clock_free(x->l_clock);
    eobj_free(x);
}

static void *mousestate_new(t_symbol *s, int argc, t_atom *argv)
{
    t_mousestate *x = (t_mousestate *)eobj_new(mousestate_class);
    if(x)
    {
        x->l_mouse_pressed = outlet_new((t_object *)x, &s_float);
        x->l_mouse_x       = outlet_new((t_object *)x, &s_float);
        x->l_mouse_y       = outlet_new((t_object *)x, &s_float);
        x->l_mouse_deltax  = outlet_new((t_object *)x, &s_float);
        x->l_mouse_deltay  = outlet_new((t_object *)x, &s_float);
        x->l_mouse_modifier= outlet_new((t_object *)x, &s_float);
        x->l_mode          = 0;
        x->l_zero          = 0;
        x->l_mouse         = eobj_get_mouse_global_position((t_object *)x);
        x->l_clock         = clock_new(x, (t_method)mousestate_tick);
    }
    return (x);
}
*/
extern "C" void setup_c0x2emousestate(void)
{
    /*
	t_eclass *c;

	c = eclass_new("c.mousestate", (method)mousestate_new, (method)mousestate_free, (short)sizeof(t_mousestate), 0L, A_GIMME, 0);


    eclass_addmethod(c, (method) mousestate_poll,       "poll",            A_NULL, 0);
    eclass_addmethod(c, (method) mousestate_nopoll,     "nopoll",          A_NULL, 0);
    eclass_addmethod(c, (method) mousestate_reset,      "reset",           A_NULL, 0);
    eclass_addmethod(c, (method) mousestate_zero,       "zero",            A_NULL, 0);
    eclass_addmethod(c, (method) mousestate_mode,       "mode",            A_FLOAT, 0);
    eclass_register(CLASS_OBJ, c);
    mousestate_class = c;
     */
}


