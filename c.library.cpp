/*
// Copyright (c) 2012-2014 Eliott Paris, Julien Colafrancesco & Pierre Guillot, CICM, Universite Paris 8.
// For information on usage and redistribution, and for a DISCLAIMER OF ALL
// WARRANTIES, see the file, "LICENSE.txt," in this distribution.
*/

#include "c.library.h"

void cream_initclass(t_eclass* c)
{
    char help[MAXPDSTRING];
    sprintf(help, "helps/%s", c->c_class.c_name->s_name);
    class_sethelpsymbol((t_class *)c, gensym(help));
}

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

extern "C" void cream_setup(void)
{    
    post("Cream Library by Pierre Guillot");
    post("© 2013 - 2015  CICM | Paris 8 University");
    post("Version %s (%s) for %s",creamversion, __DATE__, pdversion);
    post("");
    
    // Cinnamon
    setup_c0x2eatan_tilde();
    setup_c0x2esin_tilde();
    
    // Caramel
    setup_c0x2econvolve_tilde();
    setup_c0x2efir_tilde();
    setup_c0x2efreeverb_tilde();
    setup_c0x2ematrix_tilde();
    
    // Chocolate
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
    
    // Coffee
    setup_c0x2eloadmess();
    setup_c0x2epak();
    setup_c0x2epatcherargs();
    setup_c0x2epatcherinfos();
    setup_c0x2eprepend();
    setup_c0x2emousestate();
    setup_c0x2epatchermess();
    
    pd_library_add_folder("Cream", "misc");
    pd_library_add_folder("Cream", "helps");
}

extern "C" void Cream_setup(void)
{
	cream_setup();
}
