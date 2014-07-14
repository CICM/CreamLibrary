//
//  epd_ugen.h
//  Camomile
//
//  Created by Guillot Pierre on 14/07/2014.
//
//

#ifndef Camomile_epd_ugen_h
#define Camomile_epd_ugen_h

#include "z_libpd.h"

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

#ifdef __APPLE__
#include <Accelerate/Accelerate.h>
#endif

#define EMAXLOGSIG 16384

typedef struct _epd_process
{
    t_pdinstance*   p_instance;
    t_canvas*       p_canvas;
    t_pdinstance*   p_top;
    int             p_phase;
    
    int             p_vector_size;
    int             p_ninputs;
    int             p_noutputs;
    float*          p_input_pd;
    float*          p_output_pd;
    
    t_signal*       p_sigfreelist[MAXLOGSIG+1];
    t_signal*       p_sigfreeborrowed;
    t_signal*       p_sigusedlist;
}
t_epd_process;

t_epd_process *epd_process_new();

int epd_process_open(t_epd_process* x, char* name, char* dir);
int epd_process_dspstart(t_epd_process *x, int nins, int nouts, float samplerate, float vectorsize);
void epd_process_dspsuspend(t_epd_process *x);
void epd_process_dspstop(t_epd_process *x);
void epd_process_process(t_epd_process *x, const float** inputs, float** outputs);
void epd_process_free(t_epd_process *x);

#endif
