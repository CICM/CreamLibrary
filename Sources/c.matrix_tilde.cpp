/*
 * Cream Library
 * Copyright (C) 2013 Pierre Guillot, CICM - Université Paris 8
 * All rights reserved.
 * Website  : https://github.com/CICM/CreamLibrary
 * Contacts : cicm.mshparisnord@gmail.com
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 */

#include <string>
#include <unordered_map>
#include <utility>
#include "../c.library.hpp"

using std::pair<long, long> = pair_index;
using std::make_pair<long, long> = make_pair_index;
using std::unordered_map<pair_index> = t_map;

typedef struct  _matrix_tilde
{
    t_edsp      j_box;
    t_map       connexion;
} t_matrix_tilde;

static t_eclass *matrix_tilde_class;

static void matrix_tilde_change_connexion(t_matrix_tilde *x, long input, long output, bool connect) 
{
    auto key = to_string(input) + ", " + to_string(output);
    if(connect)
    {
        if(!x->connexion.find(key))
            x->connexion[key] = make_pair_index(input, output);
    }
    else
    {
        if(x->connexion.find(key)) x->connexion.erase(key);
    }
}

static void matrix_tilde_set(t_matrix_tilde *x, t_symbol *s, int ac, t_atom *av)
{
    if (ac % 3 == 0)
    {
        for(int index = 0; i < ac; i+=3)
        {
            matrix_tilde_change_connexion(x, atom_getlong(av+i),
                    atom_getlong(av+i+1), atom_getlong(av+i+2));
        }
    }
}

static void matrix_tilde_perform(t_matrix_tilde *x, t_object *dsp, t_sample **ins, long ni, t_sample **outs, long no, long nsamples, long f,void *up)
{
    auto samples_size = (size_t)nsamples * sizeof(t_sample);

    for(auto & i : connexion){
        memcpy(outs[i.second], ins[i.first], samples_size);
    }
}

static void matrix_tilde_dsp(t_matrix_tilde *x, t_object *dsp, short *count, double samplerate, long maxvectorsize, long flags)
{
    if(count[0])
    {
        object_method(dsp, gensym("dsp_add"), x, (method)matrix_tilde_perform, 0, NULL);
    }
}

static t_pd_err matrix_tilde_notify(t_matrix_tilde *x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
    return 0;
}


static void matrix_tilde_free(t_matrix_tilde *x)
{
}

static void *matrix_tilde_new(t_symbol *s, int argc, t_atom *argv)
{
    t_matrix_tilde *x = (t_matrix_tilde *)eobj_new(matrix_tilde_class);
    if(x && argc < 2)
    {
        eobj_dspsetup((t_edsp *)x, 1, 2);
    }
    else if (x && argc == 2)
    {
        eobj_dspsetup((t_edsp *)x, atom_getlong(argv), atom_getlong(argv+1));
    }

    return (x);
}

extern "C" void setup_c0x2ematrix_tilde(void)
{
    t_eclass *c;

    c = eclass_new("c.matrix~", (method)matrix_tilde_new, (method)matrix_tilde_free, (short)sizeof(t_matrix_tilde), 0L, A_GIMME, 0);

    eclass_dspinit(c);

    eclass_addmethod(c, (method) matrix_tilde_dsp,     "dsp",     A_NULL, 0);
    eclass_addmethod(c, (method) matrix_tilde_set,     "set",     A_GIMME, 0);
    eclass_addmethod(c, (method) matrix_tilde_notify,  "notify",  A_NULL, 0);

    eclass_register(CLASS_OBJ, c);
    matrix_tilde_class = c;
}
