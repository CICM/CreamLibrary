/*
// Copyright (c) 2012-2014 Eliott Paris, Julien Colafrancesco & Pierre Guillot, CICM, Universite Paris 8.
// For information on usage and redistribution, and for a DISCLAIMER OF ALL
// WARRANTIES, see the file, "LICENSE.txt," in this distribution.
*/

#include "c.library.h"

char creamversion[] = "Beta 0.4";
#ifdef PD_EXTENDED
char pdversion[] = "Pd-Extended";
#else
char pdversion[] = "Pd-Vanilla";
#endif

extern "C" void libpd_loadcream(void)
{
    cream_setup();
}

static t_eclass *cream_class;

static void *cream_new(t_symbol *s)
{
    t_eobj *x = (t_eobj *)eobj_new(cream_class);
    if(x)
    {
        logpost(x, 3, "Cream Library by Pierre Guillot\n© 2013 - 2015  CICM | Paris 8 University\nVersion %s (%s) for %s %i.%i\n",creamversion, __DATE__, pdversion, PD_MAJOR_VERSION, PD_MINOR_VERSION);
    }
    return (x);
}

extern "C" void cream_setup(void)
{
    cream_class = eclass_new("cream", (method)cream_new, (method)eobj_free, (short)sizeof(t_eobj), CLASS_PD, A_NULL, 0);
    cream_class = eclass_new("Cream", (method)cream_new, (method)eobj_free, (short)sizeof(t_eobj), CLASS_PD, A_NULL, 0);
    t_eobj* obj = (t_eobj *)cream_new(NULL);
    if(!obj)
    {
        verbose(3, "Cream Library by Pierre Guillot\n© 2013 - 2015  CICM | Paris 8 University\nVersion %s (%s) for %s %i.%i\n",creamversion, __DATE__, pdversion, PD_MAJOR_VERSION, PD_MINOR_VERSION);
        eobj_free(obj);
    }
    
    setup_c0x2ebang();
    setup_c0x2eblackboard();
    setup_c0x2ebreakpoints();
    setup_c0x2ecolorpanel();
    setup_c0x2edsp_tilde();
    setup_c0x2egain_tilde();
    setup_c0x2eincdec();
    setup_c0x2eknob();
    setup_c0x2ematrix();
    setup_c0x2emenu();
    setup_c0x2emeter_tilde();
    setup_c0x2enumber();
    setup_c0x2enumber_tilde();
    setup_c0x2eplane();
    setup_c0x2epreset();
    setup_c0x2eradio();
    setup_c0x2erslider();
    setup_c0x2escope_tilde();
    setup_c0x2eslider();
    setup_c0x2etab();
    setup_c0x2etoggle();
    
    // Deprecated
    setup_c0x2econvolve_tilde();
    setup_c0x2efreeverb_tilde();
    setup_c0x2epak();
    setup_c0x2epatcherargs();
    setup_c0x2epatcherinfos();
    setup_c0x2epatchermess();
    setup_c0x2eloadmess();
    setup_c0x2eprepend();
    
    epd_add_folder("Cream", "misc");
    epd_add_folder("Cream", "helps");
}

typedef t_object *(*t_returnnewmethod)(t_symbol *s);

extern "C" void Cream_setup(void)
{
	cream_setup();
}

