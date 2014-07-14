//
//  epd_ugen.c
//  Camomile
//
//  Created by Guillot Pierre on 14/07/2014.
//
//

#include "epd_ugen.h"

t_edspcontext* edsp_context_new();
void edsp_context_addcanvas(t_epd_process *x, t_edspcontext* dc, t_canvas *cnv);
void edsp_context_compile(t_edspcontext *dc, float samplerate, float vectorsize);
void edsp_context_clearsignal(t_edspcontext *dc);
void edsp_context_free(t_edspcontext *dc);
void edsp_context_removecanvas(t_edspcontext *dc);

t_epd_process *epd_process_new()
{
    t_epd_process* x    = (t_epd_process *)getbytes(sizeof(*x));
    x->pd_canvas        = 0;
    x->pd_dsp_context   = 0;
    x->pd_phase         = 0;
    
    x->pd_systime       = 0;
    x->pd_clock_setlist = 0;
    x->pd_dspchain      = 0;
    x->pd_dspchainsize  = 0;
    x->pd_canvaslist    = 0;
    x->pd_dspstate      = 0;
    
    return x;
}

void epd_process_free(t_epd_process *x)
{
    epd_process_stop(x);
    if(x->pd_canvas)
    {
        x->pd_canvas->gl_owner = (t_glist *)(&x->pd_me);
        canvas_free(x->pd_canvas);
    }
    if(x->pd_dsp_context)
        edsp_context_free(x->pd_dsp_context);
}

int epd_process_open(t_epd_process* x, char* name, char* dir)
{
    t_canvas *z;
    t_pd *boundx = s__X.s_thing;
    s__X.s_thing = 0;
    
    if(x->pd_canvas)
    {
        if(x->pd_dsp_context)
            edsp_context_free(x->pd_dsp_context);
        x->pd_dsp_context = 0;
        x->pd_canvas->gl_owner = (t_glist *)(&x->pd_me);
        canvas_free(x->pd_canvas);
        x->pd_canvas = 0;
    }
    
    binbuf_evalfile(gensym(name), gensym(dir));
    while (((t_pd *)x->pd_canvas != s__X.s_thing) && s__X.s_thing)
    {
        x->pd_canvas = (t_canvas *)s__X.s_thing;
        vmess((t_pd *)x->pd_canvas, gensym("pop"), "i", 1);
        s__X.s_thing = boundx;
        
        // Remove canvas from top list
        if (x->pd_canvas == canvas_list)
        {
            canvas_list = x->pd_canvas->gl_next;
        }
        else
        {
            for(z = canvas_list; z->gl_next != x->pd_canvas; z = z->gl_next)
            {
                ;
            }
            z->gl_next = x->pd_canvas->gl_next;
        }
        
        canvas_loadbang(x->pd_canvas);
        if(x->pd_dsp_context)
            edsp_context_free(x->pd_dsp_context);
        x->pd_dsp_context = 0;
        
        x->pd_dsp_context = edsp_context_new();
        return 0;
    }
    s__X.s_thing = boundx;
    
    return 1;
}

void epd_process_stop(t_epd_process *x)
{
    if(x->pd_dspchain)
    {
        freebytes(x->pd_dspchain,
                  x->pd_dspchainsize * sizeof (t_int));
        x->pd_dspchain = 0;
    }
    if(x->pd_dsp_context)
        edsp_context_clearsignal(x->pd_dsp_context);
}

int epd_process_compile(t_epd_process *x, float samplerate, float vectorsize)
{
    epd_process_stop(x);
    if(x->pd_canvas && x->pd_dsp_context)
    {
        edsp_context_removecanvas(x->pd_dsp_context);
        edsp_context_addcanvas(x, x->pd_dsp_context, x->pd_canvas);
        edsp_context_compile(x->pd_dsp_context,samplerate, vectorsize);
        
        // We steal the pd_this's dspchain
        x->pd_dspchain = pd_this->pd_dspchain;
        x->pd_dspchainsize = pd_this->pd_dspchainsize;
        pd_this->pd_dspchain = 0;
        pd_this->pd_dspchainsize = 0;
        return 0;
    }
    else
        return 1;
}

void epd_process_dsp_tick(t_epd_process *x)
{
    if (x->pd_dspchain)
    {
        t_int *ip;
        for (ip = x->pd_dspchain; ip; ) ip = (*(t_perfroutine)(*ip))(ip);
        x->pd_phase++;
    }
}
/*
typedef void (*t_clockmethod)(void *client);
struct _clock
{
    double c_settime;
    void *c_owner;
    t_clockmethod c_fn;
    struct _clock *c_next;
    t_float c_unit;
}t_clock;

void sched_tick(t_epd_process *x)
{
    double next_sys_time = pd_this->pd_systime + sys_time_per_dsp_tick;
    int countdown = 5000;
    while (pd_this->pd_clock_setlist && pd_this->pd_clock_setlist->c_settime < next_sys_time)
    {
        t_clock *c = pd_this->pd_clock_setlist;
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
    pd_this->pd_systime = next_sys_time;
    dsp_tick();
    sched_diddsp++;
}
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

t_edspcontext* edsp_context_new()
{
    int i;
    t_edspcontext *dc = (t_edspcontext *)getbytes(sizeof(*dc));
    
    dc->dc_ugenlist = NULL;
    dc->dc_toplevel = 1;
    dc->dc_iosigs   = NULL;
    dc->dc_ninlets  = 0;
    dc->dc_noutlets = 0;
    dc->dc_parentcontext = NULL;
    
    dc->dc_srate    = 0;
    dc->dc_vecsize  = 0;
    dc->dc_calcsize = 0;
    dc->dc_reblock  = 0;
    dc->dc_switched = 0;
    
    dc->dc_sigfreeborrowed = NULL;
    dc->dc_sigusedlist = NULL;
    for(i = 0; i <= EMAXLOGSIG; i++)
        dc->dc_sigfreelist[i] = NULL;
    return dc;
}

// Free a DSP Context //
void edsp_context_free(t_edspcontext *dc)
{
    if(!dc)
        return;
    
    edsp_context_removecanvas(dc);
    freebytes(dc, sizeof(*dc));
}

void edsp_context_clearsignal(t_edspcontext *dc)
{
    if(dc)
    {
        int i;
        t_signal *temp;
        t_signal *sig = dc->dc_sigusedlist;
        while(sig)
        {
            temp = sig->s_nextused;
            
            if (!sig->s_isborrowed && sig->s_vec && sig->s_vecsize)
                t_freebytes(sig->s_vec, sig->s_vecsize * sizeof (*sig->s_vec));
            
            t_freebytes(sig, sizeof(*sig));
            sig = temp;
        }
        dc->dc_sigusedlist = NULL;
        for (i = 0; i <= EMAXLOGSIG; i++)
            dc->dc_sigfreelist[i] = NULL;
        dc->dc_sigfreeborrowed = NULL;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

t_signal *esignal_new(t_edspcontext *dc, int n, t_float sr)
{
    int logn, n2, vecsize = 0;
    t_signal *ret, **whichlist;
    t_sample *fp;
    logn = ilog2(n);
    if (n)
    {
        if ((vecsize = (1<<logn)) != n)
            vecsize *= 2;
        if (logn > EMAXLOGSIG)
            bug("signal buffer too large");
        whichlist = dc->dc_sigfreelist + logn;
    }
    else
        whichlist = &dc->dc_sigfreeborrowed;
    
    /* first try to reclaim one from the free list */
    if (ret = *whichlist)
    {
        *whichlist = ret->s_nextfree;
    }
    else
    {
        /* LATER figure out what to do for out-of-space here! */
        ret = (t_signal *)getbytes(sizeof *ret);
        if (n)
        {
            ret->s_vec = (t_sample *)getbytes(vecsize * sizeof (*ret->s_vec));
            ret->s_isborrowed = 0;
        }
        else
        {
            ret->s_vec = 0;
            ret->s_isborrowed = 1;
        }
        ret->s_nextused = dc->dc_sigusedlist;
        dc->dc_sigusedlist = ret;
    }
    ret->s_n = n;
    ret->s_vecsize = vecsize;
    ret->s_sr = sr;
    ret->s_refcount = 0;
    ret->s_borrowedfrom = 0;
    
    return (ret);
}

void esignal_makereusable(t_edspcontext *dc, t_signal *sig)
{
    int logn = ilog2(sig->s_vecsize);
#if 1
    t_signal *s5;
    for (s5 = dc->dc_sigfreeborrowed; s5; s5 = s5->s_nextfree)
    {
        if (s5 == sig)
        {
            bug("signal_free 3");
            return;
        }
    }
    for (s5 = dc->dc_sigfreelist[logn]; s5; s5 = s5->s_nextfree)
    {
        if (s5 == sig)
        {
            bug("signal_free 4");
            return;
        }
    }
#endif
    
    if (sig->s_isborrowed)
    {
        /* if the signal is borrowed, decrement the borrowed-from signal's
         reference count, possibly marking it reusable too */
        t_signal *s2 = sig->s_borrowedfrom;
        if(s2)// Change need to check here
        {
            s2->s_refcount--;
            if (!s2->s_refcount)
                esignal_makereusable(dc, s2);
        }
        if ((s2 == sig) || !s2)
            bug("signal_free");
        
        sig->s_nextfree = dc->dc_sigfreeborrowed;
        dc->dc_sigfreeborrowed = sig;
    }
    else
    {
        /* if it's a real signal (not borrowed), put it on the free list
         so we can reuse it. */
        if (dc->dc_sigfreelist[logn] == sig) bug("signal_free 2");
        sig->s_nextfree = dc->dc_sigfreelist[logn];
        dc->dc_sigfreelist[logn] = sig;
    }
}

t_signal *esignal_newlike(t_edspcontext *dc, const t_signal *sig)
{
    return (esignal_new(dc, sig->s_n, sig->s_sr));
}

void esignal_setborrowed(t_signal *sig, t_signal *sig2)
{
    if (!sig->s_isborrowed || sig->s_borrowedfrom)
        bug("signal_setborrowed");
    if (sig == sig2)
        bug("signal_setborrowed 2");
    sig->s_borrowedfrom = sig2;
    sig->s_vec = sig2->s_vec;
    sig->s_n = sig2->s_n;
    sig->s_vecsize = sig2->s_vecsize;
}

int esignal_compatible(t_signal *s1, t_signal *s2)
{
    return (s1->s_n == s2->s_n && s1->s_sr == s2->s_sr);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

t_float *eobj_findsignalscalar(t_object *x, int m)
{
    int n = 0;
    t_einlet *i;
    if (x->ob_pd->c_firstin && x->ob_pd->c_floatsignalin)
    {
        if (!m--)
            return (x->ob_pd->c_floatsignalin > 0 ?
                    (t_float *)(((char *)x) + x->ob_pd->c_floatsignalin) : 0);
        n++;
    }
    for (i = (t_einlet *)x->ob_inlet; i; i = (t_einlet *)i->i_next, m--)
        if (i->i_symfrom == &s_signal)
        {
            if (m == 0)
                return (&i->i_un.iu_floatsignalvalue);
            n++;
        }
    return (0);
}

// Put a ugenbox on the chain, recursively putting any others on that this one might uncover. //
void edsp_context_compilebox(t_edspcontext *dc, t_eugenbox *u)
{
    t_esigoutlet *uout;
    t_esiginlet *uin;
    t_esigoutconnect *oc;
    t_float *scalar;
    t_signal **insig, **outsig, **sig, *s1, *s2, *s3;
    t_eugenbox *u2;
    
    t_class *class = pd_class(&u->u_obj->ob_pd);
    int i, n;
    int nonewsigs = (class == canvas_class || class->c_name == gensym("canvas"));
    int nofreesigs = (class == canvas_class || class->c_name == gensym("canvas"));
    
    // Scalar to signal //
    for(i = 0, uin = u->u_in; i < u->u_nin; i++, uin++)
    {
        if(!uin->i_nconnect)
        {
            s3 = esignal_new(dc, dc->dc_vecsize, dc->dc_srate);
            if((scalar = eobj_findsignalscalar(u->u_obj, i)))
                dsp_add_scalarcopy(scalar, s3->s_vec, s3->s_n);
            else
                dsp_add_zero(s3->s_vec, s3->s_n);
            uin->i_signal = s3;
            s3->s_refcount = 1;
        }
    }
    
    insig = (t_signal **)getbytes((u->u_nin + u->u_nout) * sizeof(t_signal *));
    outsig = insig + u->u_nin;
    for (sig = insig, uin = u->u_in, i = u->u_nin; i--; sig++, uin++)
    {
        int newrefcount;
        *sig = uin->i_signal;
        newrefcount = --(*sig)->s_refcount;
        if (nofreesigs)
            (*sig)->s_refcount++;
        else if (!newrefcount)
            esignal_makereusable(dc, *sig);
    }
    
    for (sig = outsig, uout = u->u_out, i = u->u_nout; i--; sig++, uout++)
    {
        if (nonewsigs)
        {
            *sig = uout->o_signal = esignal_new(dc, 0, dc->dc_srate);
        }
        else
            *sig = uout->o_signal = esignal_new(dc, dc->dc_vecsize, dc->dc_srate);
        
        (*sig)->s_refcount = uout->o_nconnect;
    }
    
    mess1(&u->u_obj->ob_pd, gensym("dsp"), insig);
    
    for (sig = outsig, uout = u->u_out, i = u->u_nout; i--; sig++, uout++)
    {
        if (!(*sig)->s_refcount)
            esignal_makereusable(dc, *sig);
    }
    
    // Pass it on and trip anyone whose last inlet was filled //
    for (uout = u->u_out, i = u->u_nout; i--; uout++)
    {
        s1 = uout->o_signal;
        for (oc = uout->o_connections; oc; oc = oc->oc_next)
        {
            u2 = oc->oc_who;
            uin = &u2->u_in[oc->oc_inno];
            
            // If there's already someone here, sum the two //
            if((s2 = uin->i_signal))
            {
                s1->s_refcount--;
                s2->s_refcount--;
                if (!esignal_compatible(s1, s2))
                {
                    pd_error(u->u_obj, "%s: incompatible signal inputs",
                             class_getname(u->u_obj->ob_pd));
                    return;
                }
                s3 = esignal_newlike(dc, s1);
                dsp_add_plus(s1->s_vec, s2->s_vec, s3->s_vec, s1->s_n);
                uin->i_signal = s3;
                s3->s_refcount = 1;
                if (!s1->s_refcount) esignal_makereusable(dc, s1);
                if (!s2->s_refcount) esignal_makereusable(dc, s2);
            }
            else uin->i_signal = s1;
            uin->i_ngot++;
            
            // If we didn't fill this inlet don't bother yet //
            if (uin->i_ngot < uin->i_nconnect)
                goto notyet;
            // if there's more than one, check them all //
            if (u2->u_nin > 1)
            {
                for (uin = u2->u_in, n = u2->u_nin; n--; uin++)
                    if (uin->i_ngot < uin->i_nconnect) goto notyet;
            }
            // so now we can schedule the ugen. //
            edsp_context_compilebox(dc, u2);
        notyet: ;
        }
    }
    t_freebytes(insig,(u->u_nin + u->u_nout) * sizeof(t_signal *));
    u->u_done = 1;
}

void edsp_context_compile(t_edspcontext *dc, float samplerate, float vectorsize)
{
    int i;
    t_eugenbox *u;
    t_esigoutlet *uout;
    t_esiginlet *uin;
    t_signal **sigp;
    
    if(!dc)
        return;
    
    dc->dc_reblock = 1;
    dc->dc_switched = 0;
    dc->dc_srate = samplerate;
    dc->dc_vecsize = vectorsize;
    dc->dc_calcsize = vectorsize;
    
    if(dc->dc_iosigs)
    {
        for (i = 0, sigp = dc->dc_iosigs + dc->dc_ninlets; i < dc->dc_noutlets; i++, sigp++)
        {
            if ((*sigp)->s_isborrowed && !(*sigp)->s_borrowedfrom)
            {
                esignal_setborrowed(*sigp, esignal_new(dc, dc->dc_vecsize, dc->dc_srate));
                (*sigp)->s_refcount++;
            }
        }
    }
    
    // Initialize for sorting //
    for(u = dc->dc_ugenlist; u; u = u->u_next)
    {
        u->u_done = 0;
        for(uout = u->u_out, i = u->u_nout; i--; uout++)
        {
            uout->o_nsent = 0;
        }
        for(uin = u->u_in, i = u->u_nin; i--; uin++)
        {
            uin->i_ngot = 0;
            uin->i_signal = 0;
        }
    }
    
    // Do the sort //
    for(u = dc->dc_ugenlist; u; u = u->u_next)
    {
        //post(u->u_obj->te_g.g_pd->c_name->s_name);
        // Check that we have no connected signal inlets //
        if(u->u_done)
            continue;
        for(uin = u->u_in, i = u->u_nin; i--; uin++)
        {
            if (uin->i_nconnect)
            {
                goto next;
            }
        }
        edsp_context_compilebox(dc, u);
    next: ;
    }
    
    // check for a DSP loop, which is evidenced here by the presence of ugens not yet scheduled. //
    for (u = dc->dc_ugenlist; u; u = u->u_next)
    {
        if (!u->u_done)
        {
            pd_error(u->u_obj,  "DSP loop detected (some tilde objects not scheduled)");
            // this might imply that we have unfilled "borrowed" outputs which we'd better fill in now. //
            for (i = 0, sigp = dc->dc_iosigs + dc->dc_ninlets; i < dc->dc_noutlets; i++, sigp++)
            {
                if((*sigp)->s_isborrowed && !(*sigp)->s_borrowedfrom)
                {
                    t_signal *s3 = esignal_new(dc, dc->dc_vecsize, dc->dc_srate);
                    esignal_setborrowed(*sigp, s3);
                    (*sigp)->s_refcount++;
                    dsp_add_zero(s3->s_vec, s3->s_n);
                }
            }
            break;
        }
    }
}

EXTERN_STRUCT _vinlet;
EXTERN_STRUCT _voutlet;

void vinlet_dspprolog(struct _vinlet *x, t_signal **parentsigs,
                      int myvecsize, int calcsize, int phase, int period, int frequency,
                      int downsample, int upsample,  int reblock, int switched);
void voutlet_dspprolog(struct _voutlet *x, t_signal **parentsigs,
                       int myvecsize, int calcsize, int phase, int period, int frequency,
                       int downsample, int upsample, int reblock, int switched);
void voutlet_dspepilog(struct _voutlet *x, t_signal **parentsigs,
                       int myvecsize, int calcsize, int phase, int period, int frequency,
                       int downsample, int upsample, int reblock, int switched);

typedef struct _eblock
{
    t_object x_obj;
    int x_vecsize;      /* size of audio signals in this block */
    int x_calcsize;     /* number of samples actually to compute */
    int x_overlap;
    int x_phase;        /* from 0 to period-1; when zero we run the block */
    int x_period;       /* submultiple of containing canvas */
    int x_frequency;    /* supermultiple of comtaining canvas */
    int x_count;        /* number of times parent block has called us */
    int x_chainonset;   /* beginning of code in DSP chain */
    int x_blocklength;  /* length of dspchain for this block */
    int x_epiloglength; /* length of epilog */
    char x_switched;    /* true if we're acting as a a switch */
    char x_switchon;    /* true if we're switched on */
    char x_reblock;     /* true if inlets and outlets are reblocking */
    int x_upsample;     /* upsampling-factor */
    int x_downsample;   /* downsampling-factor */
    int x_return;       /* stop right after this block (for one-shots) */
} t_eblock;

#define EPROLOGCALL 2
#define EEPILOGCALL 2

static t_int *eblock_prolog(t_int *w)
{
    t_eblock *x = (t_eblock *)w[1];
    int phase = x->x_phase;
    /* if we're switched off, jump past the epilog code */
    if (!x->x_switchon)
        return (w + x->x_blocklength);
    if (phase)
    {
        phase++;
        if (phase == x->x_period) phase = 0;
        x->x_phase = phase;
        return (w + x->x_blocklength);  /* skip block; jump past epilog */
    }
    else
    {
        x->x_count = x->x_frequency;
        x->x_phase = (x->x_period > 1 ? 1 : 0);
        return (w + EPROLOGCALL);        /* beginning of block is next ugen */
    }
}

static t_int *eblock_epilog(t_int *w)
{
    t_eblock *x = (t_eblock *)w[1];
    int count = x->x_count - 1;
    if (x->x_return)
        return (0);
    if (!x->x_reblock)
        return (w + x->x_epiloglength + EEPILOGCALL);
    if (count)
    {
        x->x_count = count;
        return (w - (x->x_blocklength -
                     (EPROLOGCALL + EEPILOGCALL)));   /* go to ugen after prolog */
    }
    else return (w + EEPILOGCALL);
}

void eugen_doit(t_edspcontext *dc, t_eugenbox *u)
{
    t_esigoutlet *uout;
    t_esiginlet *uin;
    t_esigoutconnect *oc, *oc2;
    t_class *class = pd_class(&u->u_obj->ob_pd);
    int i, n;
    /* suppress creating new signals for the outputs of signal
     inlets and subpatchs; except in the case we're an inlet and "blocking"
     is set.  We don't yet know if a subcanvas will be "blocking" so there
     we delay new signal creation, which will be handled by calling
     signal_setborrowed in the ugen_done_graph routine below. */
    int nonewsigs = (class == canvas_class ||
                     (class == vinlet_class) && !(dc->dc_reblock));
    /* when we encounter a subcanvas or a signal outlet, suppress freeing
     the input signals as they may be "borrowed" for the super or sub
     patch; same exception as above, but also if we're "switched" we
     have to do a copy rather than a borrow.  */
    int nofreesigs = (class == canvas_class ||
                      (class == voutlet_class) &&  !(dc->dc_reblock || dc->dc_switched));
    t_signal **insig, **outsig, **sig, *s1, *s2, *s3;
    t_eugenbox *u2;
    
    for (i = 0, uin = u->u_in; i < u->u_nin; i++, uin++)
    {
        if (!uin->i_nconnect)
        {
            t_float *scalar;
            s3 = esignal_new(dc, dc->dc_vecsize, dc->dc_srate);
            /* post("%s: unconnected signal inlet set to zero",
             class_getname(u->u_obj->ob_pd)); */
            if (scalar = eobj_findsignalscalar(u->u_obj, i))
                dsp_add_scalarcopy(scalar, s3->s_vec, s3->s_n);
            else
                dsp_add_zero(s3->s_vec, s3->s_n);
            uin->i_signal = s3;
            s3->s_refcount = 1;
        }
    }
    insig = (t_signal **)getbytes((u->u_nin + u->u_nout) * sizeof(t_signal *));
    outsig = insig + u->u_nin;
    for (sig = insig, uin = u->u_in, i = u->u_nin; i--; sig++, uin++)
    {
        int newrefcount;
        *sig = uin->i_signal;
        newrefcount = --(*sig)->s_refcount;
        /* if the reference count went to zero, we free the signal now,
         unless it's a subcanvas or outlet; these might keep the
         signal around to send to objects connected to them.  In this
         case we increment the reference count; the corresponding decrement
         is in sig_makereusable(). */
        if (nofreesigs)
            (*sig)->s_refcount++;
        else if (!newrefcount)
            esignal_makereusable(dc, *sig);
    }
    for (sig = outsig, uout = u->u_out, i = u->u_nout; i--; sig++, uout++)
    {
        /* similarly, for outlets of subcanvases we delay creating
         them; instead we create "borrowed" ones so that the refcount
         is known.  The subcanvas replaces the fake signal with one showing
         where the output data actually is, to avoid having to copy it.
         For any other object, we just allocate a new output vector;
         since we've already freed the inputs the objects might get called
         "in place." */
        if (nonewsigs)
        {
            *sig = uout->o_signal =
            esignal_new(dc, 0, dc->dc_srate);
        }
        else
            *sig = uout->o_signal = esignal_new(dc, dc->dc_vecsize, dc->dc_srate);
        (*sig)->s_refcount = uout->o_nconnect;
    }
    /* now call the DSP scheduling routine for the ugen.  This
     routine must fill in "borrowed" signal outputs in case it's either
     a subcanvas or a signal inlet. */
    mess1(&u->u_obj->ob_pd, gensym("dsp"), insig);
    
    /* if any output signals aren't connected to anyone, free them
     now; otherwise they'll either get freed when the reference count
     goes back to zero, or even later as explained above. */
    
    for (sig = outsig, uout = u->u_out, i = u->u_nout; i--; sig++, uout++)
    {
        if (!(*sig)->s_refcount)
            esignal_makereusable(dc, *sig);
    }
    
    /* pass it on and trip anyone whose last inlet was filled */
    for (uout = u->u_out, i = u->u_nout; i--; uout++)
    {
        s1 = uout->o_signal;
        for (oc = uout->o_connections; oc; oc = oc->oc_next)
        {
            u2 = oc->oc_who;
            uin = &u2->u_in[oc->oc_inno];
            /* if there's already someone here, sum the two */
            if (s2 = uin->i_signal)
            {
                s1->s_refcount--;
                s2->s_refcount--;
                if (!esignal_compatible(s1, s2))
                {
                    pd_error(u->u_obj, "%s: incompatible signal inputs",
                             class_getname(u->u_obj->ob_pd));
                    return;
                }
                s3 = esignal_newlike(dc, s1);
                dsp_add_plus(s1->s_vec, s2->s_vec, s3->s_vec, s1->s_n);
                uin->i_signal = s3;
                s3->s_refcount = 1;
                if (!s1->s_refcount) esignal_makereusable(dc, s1);
                if (!s2->s_refcount) esignal_makereusable(dc, s2);
            }
            else uin->i_signal = s1;
            uin->i_ngot++;
            /* if we didn't fill this inlet don't bother yet */
            if (uin->i_ngot < uin->i_nconnect)
                goto notyet;
            /* if there's more than one, check them all */
            if (u2->u_nin > 1)
            {
                for (uin = u2->u_in, n = u2->u_nin; n--; uin++)
                    if (uin->i_ngot < uin->i_nconnect) goto notyet;
            }
            /* so now we can schedule the ugen.  */
            eugen_doit(dc, u2);
        notyet: ;
        }
    }
    t_freebytes(insig,(u->u_nin + u->u_nout) * sizeof(t_signal *));
    u->u_done = 1;
}

void eugen_done_graph(t_epd_process *x, t_edspcontext *dc)
{
    t_eugenbox *u1, *u2;
    t_esigoutlet *uout;
    t_esiginlet *uin;
    t_esigoutconnect *oc, *oc2;
    int i, n;
    t_edspcontext *parent_context = dc->dc_parentcontext;
    t_eblock *block = NULL;
    t_float parent_srate;
    int parent_vecsize;
    int period, frequency, phase, vecsize, calcsize;
    t_float srate;
    int chainblockbegin;    /* DSP chain onset before block prolog code */
    int chainblockend;      /* and after block epilog code */
    int chainafterall;      /* and after signal outlet epilog */
    int reblock = 0, switched;
    int downsample = 1, upsample = 1;
    
    // If the current context have a parent context then retrieve the sampling rate and vector size
    if(parent_context)
    {
        parent_srate = parent_context->dc_srate;
        parent_vecsize = parent_context->dc_vecsize;
    }
    else
    {
        parent_srate = dc->dc_srate;
        parent_vecsize = dc->dc_vecsize;
    }
    
    // Look for a block object
    for(u1 = dc->dc_ugenlist; u1; u1 = u1->u_next)
    {
        // Take the first one
        if(pd_class(&u1->u_obj->ob_pd)->c_name == gensym("block~"))
        {
            block = (t_eblock *)&u1->u_obj->ob_pd;
            break;
        }
    }
    
    // If there is a block object
    if(block)
    {
        int realoverlap;
        vecsize = block->x_vecsize;
        if (vecsize == 0)
            vecsize = parent_vecsize;
        calcsize = block->x_calcsize;
        if (calcsize == 0)
            calcsize = vecsize;
        realoverlap = block->x_overlap;
        if (realoverlap > vecsize) realoverlap = vecsize;
        downsample = block->x_downsample;
        upsample   = block->x_upsample;
        if (downsample > parent_vecsize)
            downsample = parent_vecsize;
        period = (vecsize * downsample)/
        (parent_vecsize * realoverlap * upsample);
        frequency = (parent_vecsize * realoverlap * upsample)/
        (vecsize * downsample);
        phase = block->x_phase;
        srate = parent_srate * realoverlap * upsample / downsample;
        if (period < 1) period = 1;
        if (frequency < 1) frequency = 1;
        block->x_frequency = frequency;
        block->x_period = period;
        block->x_phase = x->pd_phase & (period - 1);
        if (! parent_context || (realoverlap != 1) ||
            (vecsize != parent_vecsize) ||
            (downsample != 1) || (upsample != 1))
            reblock = 1;
        switched = block->x_switched;
    }
    else
    {
        srate = parent_srate;
        vecsize = parent_vecsize;
        calcsize = (parent_context ? parent_context->dc_calcsize : vecsize);
        downsample = upsample = 1;
        period = frequency = 1;
        phase = 0;
        if (!parent_context) reblock = 1;
        switched = 0;
    }
    dc->dc_reblock = reblock;
    dc->dc_switched = switched;
    dc->dc_srate = srate;
    dc->dc_vecsize = vecsize;
    dc->dc_calcsize = calcsize;
    
    /* if we're reblocking or switched, we now have to create output
     signals to fill in for the "borrowed" ones we have now.  This
     is also possibly true even if we're not blocked/switched, in
     the case that there was a signal loop.  But we don't know this
     yet.  */
    
    if (dc->dc_iosigs && (switched || reblock))
    {
        t_signal **sigp;
        for(i = 0, sigp = dc->dc_iosigs + dc->dc_ninlets; i < dc->dc_noutlets; i++, sigp++)
        {
            if((*sigp)->s_isborrowed && !(*sigp)->s_borrowedfrom)
            {
                esignal_setborrowed(*sigp, esignal_new(dc, parent_vecsize, parent_srate));
                (*sigp)->s_refcount++;
            }
        }
    }
    
    /* schedule prologs for inlets and outlets.  If the "reblock" flag
     is set, an inlet will put code on the DSP chain to copy its input
     into an internal buffer here, before any unit generators' DSP code
     gets scheduled.  If we don't "reblock", inlets will need to get
     pointers to their corresponding inlets/outlets on the box we're inside,
     if any.  Outlets will also need pointers, unless we're switched, in
     which case outlet epilog code will kick in. */
    
    for (u1 = dc->dc_ugenlist; u1; u1 = u1->u_next)
    {
        t_pd *zz = &u1->u_obj->ob_pd;
        t_signal **insigs = dc->dc_iosigs, **outsigs = dc->dc_iosigs;
        if (outsigs) outsigs += dc->dc_ninlets;
        
        // TEst
        if (pd_class(zz)->c_name == gensym("inlet"))
            vinlet_dspprolog((struct _vinlet *)zz,
                             dc->dc_iosigs, vecsize, calcsize, x->pd_phase, period, frequency,
                             downsample, upsample, reblock, switched);
         // TEst
        else if (pd_class(zz)->c_name == gensym("outlet"))
            voutlet_dspprolog((struct _voutlet *)zz,
                              outsigs, vecsize, calcsize, x->pd_phase, period, frequency,
                              downsample, upsample, reblock, switched);
    }
    chainblockbegin = x->pd_dspchainsize;
    
    if(block && (reblock || switched))   /* add the block DSP prolog */
    {
        dsp_add(eblock_prolog, 1, block);
        block->x_chainonset = x->pd_dspchainsize - 1;
    }
    /* Initialize for sorting */
    for(u1 = dc->dc_ugenlist; u1; u1 = u1->u_next)
    {
        u1->u_done = 0;
        for (uout = u1->u_out, i = u1->u_nout; i--; uout++)
            uout->o_nsent = 0;
        for (uin = u1->u_in, i = u1->u_nin; i--; uin++)
            uin->i_ngot = 0, uin->i_signal = 0;
    }
    
    /* Do the sort */
    for (u1 = dc->dc_ugenlist; u1; u1 = u1->u_next)
    {
        /* check that we have no connected signal inlets */
        if (u1->u_done)
            continue;
        for (uin = u1->u_in, i = u1->u_nin; i--; uin++)
            if (uin->i_nconnect) goto next;
        
        eugen_doit(dc, u1);
    next: ;
    }
    
    /* check for a DSP loop, which is evidenced here by the presence
     of ugens not yet scheduled. */

    for(u1 = dc->dc_ugenlist; u1; u1 = u1->u_next)
    {
        if(!u1->u_done)
        {
            t_signal **sigp;
            pd_error(u1->u_obj,
                     "DSP loop detected (some tilde objects not scheduled)");
            /* this might imply that we have unfilled "borrowed" outputs
             which we'd better fill in now. */
            for (i = 0, sigp = dc->dc_iosigs + dc->dc_ninlets; i < dc->dc_noutlets;
                 i++, sigp++)
            {
                if ((*sigp)->s_isborrowed && !(*sigp)->s_borrowedfrom)
                {
                    t_signal *s3 = esignal_new(dc, parent_vecsize, parent_srate);
                    esignal_setborrowed(*sigp, s3);
                    (*sigp)->s_refcount++;
                    dsp_add_zero(s3->s_vec, s3->s_n);
                }
            }
            break;   /* don't need to keep looking. */
        }
    }
    if(block && (reblock || switched))
    {
        dsp_add(eblock_epilog, 1, block);
    }
    chainblockend = x->pd_dspchainsize;
    
    // Add epilogs for outlets
    for(u1 = dc->dc_ugenlist; u1; u1 = u1->u_next)
    {
        t_pd *zz = &u1->u_obj->ob_pd;
        if (pd_class(zz) == voutlet_class)
        {
            t_signal **iosigs = dc->dc_iosigs;
            if (iosigs) iosigs += dc->dc_ninlets;
            voutlet_dspepilog((struct _voutlet *)zz,
                              iosigs, vecsize, calcsize, x->pd_phase, period, frequency,
                              downsample, upsample, reblock, switched);
        }
    }
    
    chainafterall = x->pd_dspchainsize;
    if(block)
    {
        block->x_blocklength = chainblockend - chainblockbegin;
        block->x_epiloglength = chainafterall - chainblockend;
        block->x_reblock = reblock;
    }
    
    edsp_context_removecanvas(dc);
    x->pd_dsp_context = NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

// Add a boxe to a DSP Context //
void edsp_context_addobject(t_edspcontext *dc, t_object *obj)
{
    int i;
    t_esigoutlet *uout  = NULL;
    t_esiginlet *uin    = NULL;
    t_eugenbox *x       = NULL;
    // Allocate a ugenbox
    x  = (t_eugenbox *)getbytes(sizeof *x);
    if(x)
    {
        // Put the new ugenbox to the head of the ugenlist
        x->u_next = dc->dc_ugenlist;
        dc->dc_ugenlist = x;
        
        // Setup the ugenbox
        x->u_obj = obj;
        
        // Allocate the space for inlets connections
        x->u_nin = obj_nsiginlets(obj);
        x->u_in = getbytes(x->u_nin * sizeof (*x->u_in));
        
        // Initialize the inlets connections
        for(uin = x->u_in, i = x->u_nin; i--; uin++)
        {
            uin->i_nconnect = 0;
            uin->i_ngot = 0;
            uin->i_signal = NULL;
        }
        
        // Allocate the space for outlets connections
        x->u_nout = obj_nsigoutlets(obj);
        x->u_out = getbytes(x->u_nout * sizeof (*x->u_out));
        
        // Initialize the outlets connections
        for (uout = x->u_out, i = x->u_nout; i--; uout++)
        {
            uout->o_connections = NULL;
            uout->o_nconnect    = 0;
            uout->o_nsent       = 0;
            uout->o_signal      = NULL;
        }
    }
}

// Add a connection to a DSP Context //
void edsp_context_addconnection(t_edspcontext *dc, t_object *x1, int outno, t_object *x2, int inno)
{
    t_eugenbox       *u1;
    t_eugenbox       *u2;
    t_esigoutlet     *uout;
    t_esiginlet      *uin;
    t_esigoutconnect *oc = NULL;
    int sigoutno    = obj_sigoutletindex(x1, outno);
    int siginno     = obj_siginletindex(x2, inno);
    
    // Retrieve the ugen box of the outlet from the ugen list
    for (u1 = dc->dc_ugenlist; u1 && u1->u_obj != x1; u1 = u1->u_next);
    // Retrieve the ugen box of the inlet from the ugen list
    for (u2 = dc->dc_ugenlist; u2 && u2->u_obj != x2; u2 = u2->u_next);
    
    // If the boxes we try to connect are not in the ugen list or the inlet or the outlet is not signal
    // we don't add the connection
    if(!u1 || !u2 || siginno < 0 || sigoutno < 0 || sigoutno >= u1->u_nout || siginno >= u2->u_nin)
    {
        return;
    }
    
    // Retrieve the outlet and the inlet connection from ugen boxes
    uout = u1->u_out + sigoutno;
    uin = u2->u_in + siginno;
    
    // Allocate a connection
    oc = (t_esigoutconnect *)getbytes(sizeof *oc);
    if(oc)
    {
        // Put the connection to the head of the connection list of the outlet ugenbox
        oc->oc_next = uout->o_connections;
        uout->o_connections = oc;
        
        // Setup the connection
        oc->oc_who = u2;
        oc->oc_inno = siginno;
        
        // Increment the ugenbox inlet and outlet connections
        uout->o_nconnect++;
        uin->i_nconnect++;
    }
}


// Add a canvas to a DSP Context //
void edsp_context_addcanvas(t_epd_process *x, t_edspcontext* dc, t_canvas *cnv)
{
    t_linetraverser t;
    t_gobj          *y;
    t_object        *ob;
    t_outconnect    *oc;
    t_symbol        *dspsym = gensym("dsp");
    char            block_alert = 0;
    char            inlet_alert = 0;
    char            outlet_alert = 0;
    
    if(dc && cnv)
    {
        // Look for all the objects of a canvas
        for(y = cnv->gl_list; y; y = y->g_next)
        {
            // Check if the object is patchable and if it's a dsp object
            if((ob = pd_checkobject(&y->g_pd)) && zgetfn(&y->g_pd, dspsym))
            {
                // Add the object to the dsp context
                edsp_context_addobject(dc, ob);
            }
        }
        
        // Look for all the connections of a canvas
        linetraverser_start(&t, cnv);
        while((oc = linetraverser_next(&t)))
        {
            // Check if the outlet is signal
            if(obj_issignaloutlet(t.tr_ob, t.tr_outno))
            {
                // Add the connection between the object "t.tr_ob" outlet number "t.tr_outno"
                // and the object "t.tr_ob2" outlet number "t.tr_inno" to the dsp context
                edsp_context_addconnection(dc, t.tr_ob, t.tr_outno, t.tr_ob2, t.tr_inno);
            }
        }
        
        // Then generate the ugen graph
        eugen_done_graph(x, dc);
    }
}

// Remove the canvas from a DSP Context //
void edsp_context_removecanvas(t_edspcontext *dc)
{
    int n;
    t_eugenbox *u;
    t_esigoutlet *uout;
    t_esigoutconnect *oc, *oc2;
    if(dc)
    {
        while(dc->dc_ugenlist)
        {
            // First free the connection //
            for(uout = dc->dc_ugenlist->u_out, n = dc->dc_ugenlist->u_nout; n--; uout++)
            {
                oc = uout->o_connections;
                while (oc)
                {
                    oc2 = oc->oc_next;
                    freebytes(oc, sizeof *oc);
                    oc = oc2;
                }
            }
            
            // Then free the inlets and outlets //
            freebytes(dc->dc_ugenlist->u_out, dc->dc_ugenlist->u_nout * sizeof(*dc->dc_ugenlist->u_out));
            freebytes(dc->dc_ugenlist->u_in, dc->dc_ugenlist->u_nin * sizeof(*dc->dc_ugenlist->u_in));
            
            // Then free the boxes //
            u = dc->dc_ugenlist;
            dc->dc_ugenlist = u->u_next;
            freebytes(u, sizeof *u);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
