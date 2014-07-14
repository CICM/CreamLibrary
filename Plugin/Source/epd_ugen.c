//
//  epd_ugen.c
//  Camomile
//
//  Created by Guillot Pierre on 14/07/2014.
//
//

#include "epd_ugen.h"
#include "string.h"

EXTERN t_sample *sys_soundout;
EXTERN t_sample *sys_soundin;
// Changes
/*
    m_pd.c and m_pd.h :
    t_pdinstance* pd_getinstance()
 
    d_ugen.c and m_pd.h :
    EXTERN t_signal** get_signal_freelist();
    EXTERN t_signal* get_signal_freeborrowed();
    EXTERN t_signal* get_signal_usedlist();
*/

t_epd_process *epd_process_new()
{
    sys_lock();
    t_epd_process *x = (t_epd_process *)getbytes(sizeof(*x));
    x->p_top        = pd_getinstance();
    x->p_instance   = pdinstance_new();
    x->p_canvas     = NULL;
    x->p_phase      = 0;
    
    x->p_vector_size    = 0;
    x->p_ninputs        = 0;
    x->p_noutputs       = 0;
    x->p_input_pd       = NULL;
    x->p_output_pd      = NULL;
    
    pd_setinstance(x->p_top);
    sys_unlock();
    return x;
}

void epd_process_free(t_epd_process *x)
{
    epd_process_dspstop(x);
    sys_lock();
    pd_setinstance(x->p_instance);
    if(x->p_canvas)
        libpd_closefile(x->p_canvas);
    pd_setinstance(x->p_top);
    // freebytes !!
    sys_unlock();
}

int epd_process_open(t_epd_process* x, char* name, char* dir)
{
    sys_lock();
    pd_setinstance(x->p_instance);
    if(x->p_canvas)
        libpd_closefile(x->p_canvas);
    x->p_canvas = NULL;
    x->p_canvas = libpd_openfile(name, dir);
    pd_setinstance(x->p_top);
    sys_unlock();
    if(x->p_canvas)
        return 0;
    else
        return 1;    
}

int epd_process_dspstart(t_epd_process *x, int nins, int nouts, float samplerate, float vectorsize)
{
    t_signal* list;
    t_signal** freelist;
    if(x->p_input_pd)
    {
        freebytes(x->p_input_pd, sizeof(float) * x->p_vector_size * x->p_ninputs);
        x->p_input_pd = NULL;
    }
    if(x->p_output_pd)
    {
        freebytes(x->p_output_pd, sizeof(float) * x->p_vector_size * x->p_noutputs);
        x->p_output_pd = NULL;
    }
    
    x->p_ninputs    = nins;
    x->p_noutputs   = nouts;
    if(x->p_ninputs < 0)
        x->p_ninputs = 0;
    if(x->p_noutputs < 0)
        x->p_noutputs = 0;
    
    x->p_vector_size = vectorsize;
    x->p_input_pd  = (float *)getbytes(sizeof(float) * x->p_vector_size * x->p_ninputs);
    x->p_output_pd = (float *)getbytes(sizeof(float) * x->p_vector_size * x->p_noutputs);
    
    if(x->p_input_pd && x->p_output_pd)
    {
        memset(x->p_input_pd, 0, sizeof(float) * x->p_vector_size * x->p_ninputs);
        memset(x->p_output_pd, 0, sizeof(float) * x->p_vector_size * x->p_noutputs);
        
        sys_lock();
        pd_setinstance(x->p_instance);
        libpd_init_audio(x->p_ninputs, x->p_noutputs, samplerate);
        libpd_start_message(1);
        libpd_add_float(1.f);
        libpd_finish_message("pd", "dsp");
        
        freelist = get_signal_freelist();
        for(int i = 0; i <= MAXLOGSIG; i++)
        {
            x->p_sigfreelist[i] = freelist[i];
        }
        setzero_signal_freelist();
        
        x->p_sigfreeborrowed = get_signal_freeborrowed();
        setzero_signal_freeborrowed();
        
        x->p_sigusedlist = get_signal_usedlist();
        setzero_signal_usedlist();
        
        pd_setinstance(x->p_top);
        sys_unlock();
        
        return 0;
    }
    
    return 1;
}

void epd_process_dspsuspend(t_epd_process *x)
{
    if(x->p_input_pd)
        memset(x->p_input_pd, 0, sizeof(float) * x->p_vector_size * x->p_ninputs);
    if(x->p_output_pd)
        memset(x->p_output_pd, 0, sizeof(float) * x->p_vector_size * x->p_noutputs);
}

void epd_process_dspstop(t_epd_process *x)
{
    int i;
    t_signal **svec, *sig, *sig2;
    if(x->p_input_pd)
    {
        freebytes(x->p_input_pd, sizeof(float) * x->p_vector_size * x->p_ninputs);
        x->p_input_pd = NULL;
    }
    if(x->p_output_pd)
    {
        freebytes(x->p_output_pd, sizeof(float) * x->p_vector_size * x->p_noutputs);
        x->p_output_pd = NULL;
    }
    
    while(sig = x->p_sigusedlist)
    {
        x->p_sigusedlist = sig->s_nextused;
        if (!sig->s_isborrowed)
            t_freebytes(sig->s_vec, sig->s_vecsize * sizeof (*sig->s_vec));
        t_freebytes(sig, sizeof *sig);
    }
    for (i = 0; i <= MAXLOGSIG; i++)
        x->p_sigfreelist[i] = 0;
    x->p_sigfreeborrowed = 0;
}

struct _pdinstance
{
    double pd_systime;          /* global time in Pd ticks */
    t_clock *pd_clock_setlist;  /* list of set clocks */
    t_int *pd_dspchain;         /* DSP chain */
    int pd_dspchainsize;        /* number of elements in DSP chain */
    t_canvas *pd_canvaslist;    /* list of all root canvases */
    int pd_dspstate;            /* whether DSP is on or off */
};

void eprocess_sched_tick(t_pdinstance *x)
{
    /*
    double next_sys_time = x->p_systime + sys_time_per_dsp_tick;
    int countdown = 5000;
    while (x->p_clock_setlist && x->p_clock_setlist->c_settime < next_sys_time)
    {
        t_clock *c = x->p_clock_setlist;
        pd_this->pd_systime = c->c_settime;
        clock_unset(pd_this->pd_clock_setlist);
        outlet_setstacklim();
        (*c->c_fn)(c->c_owner);
        if (!countdown--)
        {
            countdown = 5000;
            sys_pollgui();
        }
        if (sys_quit)
            return;
    }
    x->pd_systime = next_sys_time;
     */
    if(x->pd_dspchainsize)
    {
        t_int *ip;
        for(ip = x->pd_dspchain; ip; )
            ip = (*(t_perfroutine)(*ip))(ip);
        //dsp_phase++;
    }
    //sched_diddsp++;
}

void epd_process_process(t_epd_process *x, const float** inputs, float** outputs)
{
    int i, j, k;
    int blcksize = libpd_blocksize();
    int ticks = x->p_vector_size / blcksize;
    t_sample *p0, *p1;
#ifdef __APPLE__
    for(int i = 0; i < x->p_ninputs; i++)
    {
        cblas_scopy(x->p_vector_size, inputs[i], 1, x->p_input_pd+i, x->p_ninputs);
    }
#endif
    for (i = 0; i < ticks; i++)
    {
        /*
        for (j = 0, p0 = sys_soundin; j < blcksize; j++, p0++)
        {
            for (k = 0, p1 = p0; k < x->p_ninputs; k++, p1 += blcksize)
            {
                *p1 = * x->p_input_pd++;
            }
        }
        
        memset(sys_soundout, 0, sys_outchannels*blcksize * sizeof(t_sample));
        */
        eprocess_sched_tick(x->p_instance);
        /*
        for (j = 0, p0 = sys_soundout; j < blcksize; j++, p0++)
        {
            for (k = 0, p1 = p0; k < x->p_noutputs; k++, p1 += blcksize)
            {
                *x->p_output_pd++ = *p1; \
            }
        }
         */
    }
#ifdef __APPLE__
    for(int i = 0; i < x->p_noutputs; i++)
    {
        cblas_scopy(x->p_vector_size, x->p_output_pd+i, x->p_noutputs, outputs[i], 1);
    }
#endif
}


