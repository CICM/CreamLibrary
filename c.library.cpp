/*
// Copyright (c) 2012-2014 Eliott Paris, Julien Colafrancesco & Pierre Guillot, CICM, Universite Paris 8.
// For information on usage and redistribution, and for a DISCLAIMER OF ALL
// WARRANTIES, see the file, "LICENSE.txt," in this distribution.
*/

#include "c.library.hpp"

char creamversion[] = "Beta 0.4";

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
#ifndef LIB_PD
        logpost(x, 3, "Cream Library by Pierre Guillot\n© 2013 - 2015  CICM | Paris 8 University\nVersion %s (%s) for Pure Data %i.%i\n", creamversion, __DATE__, PD_MAJOR_VERSION, PD_MINOR_VERSION);
#endif
    }
    return (x);
}

extern "C" void cream_setup(void)
{
    cream_class = eclass_new("cream", (t_method)cream_new, (t_method)eobj_free, (short)sizeof(t_eobj), CLASS_PD, A_NULL, 0);
    cream_class = eclass_new("Cream", (t_method)cream_new, (t_method)eobj_free, (short)sizeof(t_eobj), CLASS_PD, A_NULL, 0);
    t_eobj* obj = (t_eobj *)cream_new(NULL);
    if(!obj)
    {
#ifndef LIB_PD
        verbose(3, "Cream Library by Pierre Guillot\n© 2013 - 2015  CICM | Paris 8 University\nVersion %s (%s) for Pure Data %i.%i\n", creamversion, __DATE__, PD_MAJOR_VERSION, PD_MINOR_VERSION);
#endif
    }
    else
    {
        eobj_free(obj);
    }
    //epd_add_lib("cream");
    
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
    setup_c0x2ecomment();
    
#ifdef __APPLE__
    setup_c0x2ekeyboard();
    setup_c0x2ecamomile();
#endif
    
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

float pd_wrap(float f, const float min, const float max)
{
    if(min < max)
    {
        const float ratio = max - min;
        while(f < min){f += ratio;}
        while(f > max){f -= ratio;}
        return f;
    }
    else
    {
        const float ratio = min - max;
        while(f < max){f += ratio;}
        while(f > min){f -= ratio;}
        return f;
    }
}

float pd_clip(const float f, const float min, const float max)
{
    return (f < min) ? min : ((f > max) ? max : f);
}

float pd_clip_min(const float f, const float min)
{
    return (f < min) ? min : f;
}

float pd_clip_max(const float f, const float max)
{
    return (f > max) ? max : f;
}

float pd_ordinate(const float radius, const float angle)
{
    return radius * sinf(angle);
}

float pd_abscissa(const float radius, const float angle)
{
    return radius * cosf(angle);
}

float pd_radius(const float x, const float y)
{
    return sqrtf(x*x + y*y);
}

float pd_angle(const float x, const float y)
{
    return atan2f(y, x);
}





