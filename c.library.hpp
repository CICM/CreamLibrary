/*
// Copyright (c) 2012-2014 Eliott Paris, Julien Colafrancesco & Pierre Guillot, CICM, Universite Paris 8.
// For information on usage and redistribution, and for a DISCLAIMER OF ALL
// WARRANTIES, see the file, "LICENSE.txt," in this distribution.
*/

#ifndef DEF_CREAM_LIBRARY
#define DEF_CREAM_LIBRARY

extern "C"
{
#include "ThirdParty/CicmWrapper/Sources/cicm_wrapper.h"
}

#define CREAM_MAXITEMS 256

extern "C" void cream_setup(void);
extern "C" void Cream_setup(void);
extern "C" void libpd_loadcream(void);

extern "C" void setup_c0x2ebang(void);
extern "C" void setup_c0x2eblackboard(void);
extern "C" void setup_c0x2ebreakpoints(void);
extern "C" void setup_c0x2ecolorpanel(void);
extern "C" void setup_c0x2edsp_tilde(void);
extern "C" void setup_c0x2egain_tilde(void);
extern "C" void setup_c0x2eincdec(void);
extern "C" void setup_c0x2eknob(void);
extern "C" void setup_c0x2ematrix(void);
extern "C" void setup_c0x2emenu(void);
extern "C" void setup_c0x2emeter_tilde(void);
extern "C" void setup_c0x2enumber(void);
extern "C" void setup_c0x2enumber_tilde(void);
extern "C" void setup_c0x2eplane(void);
extern "C" void setup_c0x2epreset(void);
extern "C" void setup_c0x2eradio(void);
extern "C" void setup_c0x2erslider(void);
extern "C" void setup_c0x2escope_tilde(void);
extern "C" void setup_c0x2eslider(void);
extern "C" void setup_c0x2etab(void);
extern "C" void setup_c0x2etoggle(void);

#ifdef __APPLE__
extern "C" void setup_c0x2ekeyboard(void);
extern "C" void setup_c0x2ecamomile(void);

extern "C" void setup_c0x2ewavesel(void);
#endif

// Deprecated
extern "C" void setup_c0x2econvolve_tilde(void);
extern "C" void setup_c0x2efreeverb_tilde(void);
extern "C" void setup_c0x2epak(void);
extern "C" void setup_c0x2eloadmess(void);
extern "C" void setup_c0x2eprepend(void);
extern "C" void setup_c0x2epatcherargs(void);
extern "C" void setup_c0x2epatcherinfos(void);
extern "C" void setup_c0x2epatchermess(void);

static t_symbol* cream_sym_goppos               = gensym("goppos");
static t_symbol* cream_sym_gopsize              = gensym("gopsize");
static t_symbol* cream_sym_select               = gensym("select");
static t_symbol* cream_sym_delete               = gensym("delete");
static t_symbol* cream_sym_past                 = gensym("paste");

static t_symbol* cream_sym_attr_modified        = gensym("attr_modified");
static t_symbol* cream_sym_param_changed        = gensym("param_changed");

static t_symbol* cream_sym_background_layer     = gensym("background_layer");
static t_symbol* cream_sym_text_layer           = gensym("text_layer");
static t_symbol* cream_sym_selection_layer      = gensym("selection_layer");
static t_symbol* cream_sym_points_layer         = gensym("points_layer");
static t_symbol* cream_sym_value_layer          = gensym("value_layer");
static t_symbol* cream_sym_knob_layer           = gensym("knob_layer");
static t_symbol* cream_sym_signal_layer         = gensym("signal_layer");
static t_symbol* cream_sym_leds_layer           = gensym("leds_layer");
static t_symbol* cream_sym_items_layer          = gensym("items_layer");
static t_symbol* cream_sym_needle_layer         = gensym("needle_layer");
static t_symbol* cream_sym_picked_layer         = gensym("picked_layer");
static t_symbol* cream_sym_hover_layer          = gensym("hover_layer");

static t_symbol* cream_sym_arcolor              = gensym("arcolor");
static t_symbol* cream_sym_bacolor              = gensym("bacolor");
static t_symbol* cream_sym_bgcolor              = gensym("bgcolor");
static t_symbol* cream_sym_bdcolor 				= gensym("bdcolor");
static t_symbol* cream_sym_crcolor 				= gensym("crcolor");
static t_symbol* cream_sym_btcolor              = gensym("btcolor");
static t_symbol* cream_sym_textcolor            = gensym("textcolor");
static t_symbol* cream_sym_ptcolor              = gensym("ptcolor");
static t_symbol* cream_sym_licolor              = gensym("licolor");
static t_symbol* cream_sym_coldcolor            = gensym("coldcolor");
static t_symbol* cream_sym_tepidcolor           = gensym("tepidcolor");
static t_symbol* cream_sym_warmcolor            = gensym("warmcolor");
static t_symbol* cream_sym_hotcolor             = gensym("hotcolor");
static t_symbol* cream_sym_overcolor            = gensym("overcolor");
static t_symbol* cream_sym_wkeycolor            = gensym("wkeycolor");
static t_symbol* cream_sym_bkeycolor            = gensym("bkeycolor");
static t_symbol* cream_sym_skeycolor            = gensym("skeycolor");
static t_symbol* cream_sym_kncolor              = gensym("kncolor");
static t_symbol* cream_sym_itcolor              = gensym("itcolor");
static t_symbol* cream_sym_secolor              = gensym("secolor");
static t_symbol* cream_sym_hocolor              = gensym("hocolor");
static t_symbol* cream_sym_necolor              = gensym("necolor");
static t_symbol* cream_sym_font                 = gensym("font");

static t_symbol* cream_sym_fontsize             = gensym("fontsize");
static t_symbol* cream_sym_fontname             = gensym("fontname");
static t_symbol* cream_sym_fontweight           = gensym("fontweight");
static t_symbol* cream_sym_fontslant            = gensym("fontslant");

static t_symbol* cream_sym_endless              = gensym("endless");
static t_symbol* cream_sym_ptsize               = gensym("ptsize");
static t_symbol* cream_sym_preset               = gensym("preset");
static t_symbol* cream_sym_atpreset             = gensym("@preset");
static t_symbol* cream_sym_atindex              = gensym("@index");
static t_symbol* cream_sym_interpolate          = gensym("interpolate");
static t_symbol* cream_sym_absrange             = gensym("absrange");
static t_symbol* cream_sym_ordrange             = gensym("ordrange");
static t_symbol* cream_sym_atpoints             = gensym("@points");
static t_symbol* cream_sym_function             = gensym("function");
static t_symbol* cream_sym_Linear               = gensym("Linear");
static t_symbol* cream_sym_Cosine               = gensym("Cosine");
static t_symbol* cream_sym_Cubic                = gensym("Cubic");
static t_symbol* cream_sym_outline              = gensym("outline");
static t_symbol* cream_sym_items                = gensym("items");
static t_symbol* cream_sym_orientation          = gensym("orientation");
static t_symbol* cream_sym_lowkey               = gensym("lowkey");
static t_symbol* cream_sym_highkey              = gensym("highkey");
static t_symbol* cream_sym_decimal              = gensym("decimal");

static t_symbol* cream_sym_nothing              = gensym("''");
static t_symbol* cream_sym_left_bracket         = gensym("[");
static t_symbol* cream_sym_right_bracket        = gensym("]");

static t_symbol* cream_sym_state                = gensym("state");

#endif
