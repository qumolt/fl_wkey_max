/* -------------------------------------------------------------------------- */
#include "ext.h"				// standard Max include, always required
#include "ext_obex.h"			// required for new style Max object

#include "jpatcher_api.h"		// includes data structures and accessor functions required by UI objects.
#include "jgraphics.h"			// includes data structures and functions for drawing.
#include "ext_boxstyle.h"		// class_attr_style_rgba_preview
#include "z_dsp.h"				// msp, t_double

//#include "jpatcher_syms.h"		// textfield

//#include "common/commonsyms.c"
//#include "commonsyms.h"	
//#include "ext_common.h"
//#include "ext_parameter.h"
//#include "ext_drag.h"

//#include <time.h>				// clock
//#include <ext_linklist.h>		// linked lists
//#include "ext_dictionary.h"	// textfield
//#include "ext_critical.h"		// threads

/* class and symbols ---------------------------------------------------------*/
static t_class *s_fl_wkey_class;

/*---------------------------------------------------------------*/
#define DFLT_OCTAVES 5
#define DFLT_DIVOCT 12

//(2W * div * octs + 2FRAME) (3H + SUPMARG + 2FRAME) 
//(2 * W * 12 * 5 + 4) (3 * H + 10 + 4)
#define BOX_SIZE_CHAR "0. 0. 604. 104."
#define HNOTEW 5.
#define TNOTEH 30.
#define SUPMARGIN 10.
#define ALLFRAME 2.
#define C4OCTAVE 2
#define ZEROMARKD 3. 
#define LENPOLYNOTES 32
#define POLYMARKE 6.

/*------------------------------------------------------------*/
enum INLETS { I_INPUT, NUM_INLETS };	
enum OUTLETS { O_OUTPUT, NUM_OUTLETS };

/*--------------------------------------------------------------- */
typedef struct _fl_gnote {
	short white;
	long prev_exten;
	long next_exten;
}fl_gnote;

typedef struct _fl_wkey
{
	t_jbox	j_box;
	
	t_jrgba u_background;   // background color

	long polynotes[LENPOLYNOTES];
	long total_polynotes;

	fl_gnote *gnotes;
	long oct_div;
	long n_oct;
	long c4_oct;

	short is_key_pressed;
	long sel_key;

	void *m_outlet;

} t_fl_wkey;

/* methods ------------------------------------------------------------------- */
void *fl_wkey_new(t_symbol *s, long argc, t_atom *argv);
void fl_wkey_assist(t_fl_wkey *x, void *b, long m, long a, char *s);

void fl_wkey_list(t_fl_wkey *x, t_symbol *s, long argc, t_atom *argv);
void fl_wkey_int(t_fl_wkey *x, long n);
void fl_wkey_float(t_fl_wkey *x, double f);
void fl_wkey_bang(t_fl_wkey *x);
void fl_wkey_message(t_fl_wkey *x, t_symbol *s, long argc, t_atom *argv);
void fl_wkey_octave(t_fl_wkey *x, t_symbol *s, long argc, t_atom *argv);

void fl_wkey_free(t_fl_wkey *x);

void fl_wkey_paint(t_fl_wkey *x, t_object *patcherview);

void fl_wkey_mousedrag(t_fl_wkey *x, t_object *patcherview, t_pt pt, long modifiers);
void fl_wkey_mouseup(t_fl_wkey *x, t_object *patcherview, t_pt pt, long modifiers);
void fl_wkey_mousedown(t_fl_wkey *x, t_object *patcherview, t_pt pt, long modifiers);
void mouse_sendkey(t_fl_wkey *x, t_object *patcherview, t_pt pt);
//t_max_err fl_wkey_notify(t_fl_wkey *x, t_symbol *s, t_symbol *msg, void *sender, void *data);

long z_mod(long x, long base);
t_double huetorgb(double p, double q, double t);
t_jrgb hsltorgb(double h, double s, double l);

