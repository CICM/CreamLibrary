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

static t_class *cin_class;

typedef struct _cin
{
    t_object x_obj;
    t_int x_n;
    t_int *x_vec;
    t_sample* x_inputs;
} t_cin;

static t_class *cout_class;

typedef struct _cout
{
    t_object x_obj;
    t_int x_n;
    t_int *x_vec;
    t_float x_f;
    t_sample* x_outputs;
} t_cout;

/* ----------------------------- cout~ --------------------------- */
static void *cout_new(t_symbol *s, int argc, t_atom *argv)
{
    t_cout *x = (t_cout *)pd_new(cout_class);
    t_atom defarg[2];
    int i;
    if (!argc)
    {
        argv = defarg;
        argc = 2;
        SETFLOAT(&defarg[0], 1);
        SETFLOAT(&defarg[1], 2);
    }
    x->x_n = argc;
    x->x_vec = (t_int *)getbytes(argc * sizeof(*x->x_vec));
    for (i = 0; i < argc; i++)
        x->x_vec[i] = atom_getintarg(i, argc, argv);
    for (i = 1; i < argc; i++)
        inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
    x->x_f = 0;
    x->x_outputs = NULL;
    return (x);
}

static void cout_dsp(t_cout *x, t_signal **sp)
{
    t_int i, *ip;
    t_signal **sp2;
#ifdef LIBPD
    for (i = x->x_n, ip = x->x_vec, sp2 = sp; i--; ip++, sp2++)
    {
        int ch = *ip - 1;
        if ((*sp2)->s_n != DEFDACBLKSIZE)
            error("cout~: bad vector size");
        else if(ch >= 0 && ch < sys_get_outchannels() && x->x_outputs)
            dsp_add(plus_perform, 4, x->x_outputs + DEFDACBLKSIZE*ch, (*sp2)->s_vec, x->x_outputs + DEFDACBLKSIZE*ch, DEFDACBLKSIZE);
    }
#else
    for (i = x->x_n, ip = x->x_vec, sp2 = sp; i--; ip++, sp2++)
    {
        int ch = *ip - 1;
        if ((*sp2)->s_n != DEFDACBLKSIZE)
            error("dac~: bad vector size");
        else if (ch >= 0 && ch < sys_get_outchannels())
            dsp_add(plus_perform, 4, sys_soundout + DEFDACBLKSIZE*ch,
                    (*sp2)->s_vec, sys_soundout + DEFDACBLKSIZE*ch, DEFDACBLKSIZE);
    }
#endif
}

static void cout_free(t_cout *x)
{
    freebytes(x->x_vec, x->x_n * sizeof(*x->x_vec));
}

static void cout_setup(void)
{
    cout_class = class_new(gensym("c.out~"), (t_newmethod)cout_new, (t_method)cout_free, sizeof(t_cout), 0, A_GIMME, 0);
    CLASS_MAINSIGNALIN(cout_class, t_cout, x_f);
    class_addmethod(cout_class, (t_method)cout_dsp, gensym("dsp"), A_CANT, 0);
}

/* ----------------------------- cin~ --------------------------- */

static void *cin_new(t_symbol *s, int argc, t_atom *argv)
{
    t_cin *x = (t_cin *)pd_new(cin_class);
    t_atom defarg[2];
    int i;
    if (!argc)
    {
        argv = defarg;
        argc = 2;
        SETFLOAT(&defarg[0], 1);
        SETFLOAT(&defarg[1], 2);
    }
    x->x_n = argc;
    x->x_vec = (t_int *)getbytes(argc * sizeof(*x->x_vec));
    for (i = 0; i < argc; i++)
        x->x_vec[i] = atom_getintarg(i, argc, argv);
    for (i = 0; i < argc; i++)
        outlet_new(&x->x_obj, &s_signal);
    
    x->x_inputs = NULL;
    return (x);
}

t_int *copy_perf8(t_int *w)
{
    t_sample *in1 = (t_sample *)(w[1]);
    t_sample *out = (t_sample *)(w[2]);
    int n = (int)(w[3]);
    
    for (; n; n -= 8, in1 += 8, out += 8)
    {
        t_sample f0 = in1[0];
        t_sample f1 = in1[1];
        t_sample f2 = in1[2];
        t_sample f3 = in1[3];
        t_sample f4 = in1[4];
        t_sample f5 = in1[5];
        t_sample f6 = in1[6];
        t_sample f7 = in1[7];
        
        out[0] = f0;
        out[1] = f1;
        out[2] = f2;
        out[3] = f3;
        out[4] = f4;
        out[5] = f5;
        out[6] = f6;
        out[7] = f7;
    }
    return (w+4);
}

static void cin_dsp(t_cin *x, t_signal **sp)
{
    t_int i, *ip;
    t_signal **sp2;
#ifdef LIBPD
    for (i = x->x_n, ip = x->x_vec, sp2 = sp; i--; ip++, sp2++)
    {
        int ch = *ip - 1;
        if ((*sp2)->s_n != DEFDACBLKSIZE)
            error("cin~: bad vector size");
        else if (ch >= 0 && ch < sys_get_inchannels() && x->x_inputs)
            dsp_add_copy(x->x_inputs + DEFDACBLKSIZE*ch,(*sp2)->s_vec, DEFDACBLKSIZE);
        else dsp_add_zero((*sp2)->s_vec, DEFDACBLKSIZE);
    }
#else
    for (i = x->x_n, ip = x->x_vec, sp2 = sp; i--; ip++, sp2++)
    {
        int ch = *ip - 1;
        if ((*sp2)->s_n != DEFDACBLKSIZE)
            error("adc~: bad vector size");
        else if (ch >= 0 && ch < sys_get_inchannels())
            dsp_add_copy(sys_soundin + DEFDACBLKSIZE*ch,
                         (*sp2)->s_vec, DEFDACBLKSIZE);
        else dsp_add_zero((*sp2)->s_vec, DEFDACBLKSIZE);
    }
#endif
}

static void cin_free(t_cin *x)
{
    freebytes(x->x_vec, x->x_n * sizeof(*x->x_vec));
}

static void cin_setup(void)
{
    cin_class = class_new(gensym("c.in~"), (t_newmethod)cin_new, (t_method)cin_free, sizeof(t_cin), 0, A_GIMME, 0);
    class_addmethod(cin_class, (t_method)cin_dsp, gensym("dsp"), A_CANT, 0);
}

void cio_setup(void)
{
    cout_setup();
    cin_setup();
}



