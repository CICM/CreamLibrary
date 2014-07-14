//
//  epd_ugen.h
//  Camomile
//
//  Created by Guillot Pierre on 14/07/2014.
//
//

#ifndef Camomile_epd_ugen_h
#define Camomile_epd_ugen_h

#ifndef __m_pd_h_
#ifdef PD_EXTENTED
#include "pd-extented/m_pd.h"
#include "pd-extented/m_imp.h"
#include "pd-extented/g_canvas.h"
#else
#include "m_pd.h"
#include "m_imp.h"
#include "g_canvas.h"
#endif
#endif

#define EMAXLOGSIG 16384

union einletunion
{
    t_symbol *iu_symto;
    t_gpointer *iu_pointerslot;
    t_float *iu_floatslot;
    t_symbol **iu_symslot;
    t_float iu_floatsignalvalue;
};

typedef struct _einlet
{
    t_pd i_pd;
    struct _inlet *i_next;
    t_object *i_owner;
    t_pd *i_dest;
    t_symbol *i_symfrom;
    union einletunion i_un;
}t_einlet;

struct _esigoutlet;

typedef struct _esiginlet
{
    int i_nconnect;
    int i_ngot;
    t_signal *i_signal;
} t_esiginlet;

typedef struct _eugenbox
{
    struct _esiginlet *u_in;
    int u_nin;
    struct _esigoutlet *u_out;
    int u_nout;
    int u_phase;
    struct _eugenbox *u_next;
    t_object *u_obj;
    int u_done;
} t_eugenbox;

typedef struct _esigoutconnect
{
    t_eugenbox *oc_who;
    int oc_inno;
    struct _esigoutconnect *oc_next;
} t_esigoutconnect;

typedef struct _esigoutlet
{
    int o_nconnect;
    int o_nsent;
    t_signal *o_signal;
    t_esigoutconnect *o_connections;
} t_esigoutlet;

typedef struct _edspcontext
{
    struct _eugenbox *dc_ugenlist;
    struct _edspcontext *dc_parentcontext;
    int dc_ninlets;
    int dc_noutlets;
    t_signal **dc_iosigs;
    t_float dc_srate;
    int dc_vecsize;         /* vector size, power of two */
    int dc_calcsize;        /* number of elements to calculate */
    char dc_toplevel;       /* true if "iosigs" is invalid. */
    char dc_reblock;        /* true if we have to reblock inlets/outlets */
    char dc_switched;       /* true if we're switched */
    
    t_signal *dc_sigfreelist[EMAXLOGSIG+1];
    t_signal *dc_sigfreeborrowed;
    t_signal *dc_sigusedlist;
}t_edspcontext;

typedef struct _epd_process
{
    t_object        pd_me;
    t_edspcontext*  pd_dsp_context;
    t_canvas*       pd_canvas;
    
    double          pd_systime;          /* global time in Pd ticks */
    t_clock*        pd_clock_setlist;  /* list of set clocks */
    t_int*          pd_dspchain;         /* DSP chain */
    int             pd_dspchainsize;        /* number of elements in DSP chain */
    t_canvas*       pd_canvaslist;    /* list of all root canvases */
    int             pd_dspstate;            /* whether DSP is on or off */
    
    int             pd_phase;
}
t_epd_process;

t_epd_process *epd_process_new();
int epd_process_open(t_epd_process* x, char* name, char* dir);
int epd_process_compile(t_epd_process *x, float samplerate, float vectorsize);
void epd_process_stop(t_epd_process *x);
void epd_process_free(t_epd_process *x);

#endif
