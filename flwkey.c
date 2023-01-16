#include "flwkey.h"
#include "common/commonsyms.c"

void ext_main(void *r)
{
	t_class *c;

	common_symbols_init();

	c = class_new("flwkey", (method)fl_wkey_new, (method)fl_wkey_free, sizeof(t_fl_wkey), (method)NULL, A_GIMME, 0);

	c->c_flags |= CLASS_FLAG_NEWDICTIONARY;
	jbox_initclass(c, JBOX_FIXWIDTH
					| JBOX_COLOR
					//| JBOX_TEXTFIELD 
					//| JBOX_FONTATTR 
					); 
	//class_dspinitjbox(c);

	class_addmethod(c, (method)fl_wkey_paint, "paint", A_CANT, 0);
	class_addmethod(c, (method)fl_wkey_assist, "assist", A_CANT, 0);
	//class_addmethod(c, (method)jbox_notify, "notify", A_CANT, 0);
	class_addmethod(c, (method)fl_wkey_mousedrag, "mousedrag", A_CANT, 0);
	class_addmethod(c, (method)fl_wkey_mouseup, "mouseup", A_CANT, 0);
	class_addmethod(c, (method)fl_wkey_mousedown, "mousedown", A_CANT, 0);
	
	class_addmethod(c, (method)fl_wkey_int, "int", A_LONG, 0);
	//class_addmethod(c, (method)fl_wkey_float, "float", A_float, 0);
	//class_addmethod(c, (method)fl_wkey_bang, "bang", 0);

	class_addmethod(c, (method)fl_wkey_message, "div", A_GIMME, 0);
	
	CLASS_STICKY_CATEGORY(c, 0, "Color");

	CLASS_ATTR_STYLE_RGBA_PREVIEW(c, "bgcolor", 0, t_fl_wkey, u_background, "Background Color", "rect_fill");
	CLASS_ATTR_BASIC(c, "bgcolor", 0);
	//CLASS_ATTR_STYLE_ALIAS_RGBA_LEGACY(c, "bgcolor", "brgba");
	CLASS_ATTR_STYLE_ALIAS_RGBA_LEGACY(c, "bgcolor", "brgb");
	CLASS_ATTR_LEGACYDEFAULT(c, "bgcolor", "bgcolor", 0, "1. 1. 1. 1.");

	CLASS_ATTR_STYLE_RGBA_PREVIEW(c, "elementcolor", 0, t_fl_wkey, u_lineas, "Off Color", "side_arch");
	CLASS_ATTR_BASIC(c, "elementcolor", 0);
	CLASS_ATTR_STYLE_ALIAS_NOSAVE(c, "elementcolor", "crgb");
	CLASS_ATTR_LEGACYDEFAULT(c, "elementcolor", "elementcolor", 0, "0.9 0.8 0.8 1.");
	
	CLASS_ATTR_STYLE_RGBA_PREVIEW(c, "cursorcolor", 0, t_fl_wkey, u_cursor, "On Color", "side_arch");
	CLASS_ATTR_BASIC(c, "cursorcolor", 0);
	class_attr_stylemap(c, "cursorcolor", "color");
	CLASS_ATTR_LEGACYDEFAULT(c, "cursorcolor", "cursorcolor", 0, "0.2 0.2 0.2 1.");
	CLASS_ATTR_STYLE_ALIAS_NOSAVE(c, "cursorcolor", "frgba"); // ej: I doubt anyone used this but color instead so leaving it as a normal alias (will use class_attr_style_alias if something comes up)

	CLASS_ATTR_RGBA(c, "textcolor", 0, t_fl_wkey, j_textcolor);
	CLASS_ATTR_DEFAULT_SAVE_PAINT(c, "textcolor", 0, "0.9 0.9 0.9 1.");
	CLASS_ATTR_STYLE_LABEL(c, "textcolor", 0, "rgba", "Text Color");
	CLASS_ATTR_CATEGORY(c, "textcolor", 0, "Color");

	CLASS_STICKY_CATEGORY_CLEAR(c);

	/*
	CLASS_ATTR_RGBA(c, "rectcolor", 0, t_fl_wkey, j_rectcolor);
	CLASS_ATTR_STYLE_LABEL(c, "rectcolor", 0, "rgba", "Rectangle Color");
	CLASS_ATTR_DEFAULT_SAVE_PAINT(c, "rectcolor", 0, "0. 0. 0. 1.");

	CLASS_ATTR_RGBA(c, "overcolor", 0, t_fl_wkey, j_overcolor);
	CLASS_ATTR_STYLE_LABEL(c, "overcolor", 0, "rgba", "Rectangle Over Color");
	CLASS_ATTR_DEFAULT_SAVE_PAINT(c, "overcolor", 0, "1. 1. 1. 1.");
	*/

	CLASS_ATTR_INTRODUCED(c, "elementcolor", 0, "7.0.0");
	
	CLASS_ATTR_DEFAULT(c, "patching_rect", 0, BOX_SIZE_CHAR);

	class_register(CLASS_BOX, c);
	s_fl_wkey_class = c;
}

void *fl_wkey_new(t_symbol *s, long argc, t_atom *argv)
{
	t_fl_wkey *x = NULL;
	t_dictionary *d = NULL;
	t_max_err err = MAX_ERR_GENERIC;
	long boxflags;

	if (!(d = object_dictionaryarg(argc, argv)))
		return NULL;

	x = (t_fl_wkey *)object_alloc(s_fl_wkey_class);
	if (!x) { return x; }

	boxflags = 0
		| JBOX_DRAWFIRSTIN
		| JBOX_NODRAWBOX
		| JBOX_DRAWINLAST
		| JBOX_TRANSPARENT
		| JBOX_NOGROW
		//		| JBOX_GROWY
		//		| JBOX_GROWBOTH
		| JBOX_HILITE
		//      | JBOX_BACKGROUND
		| JBOX_DRAWBACKGROUND
		//      | JBOX_NOFLOATINSPECTOR
		//      | JBOX_MOUSEDRAGDELTA
		//		| JBOX_TEXTFIELD
		;

	err = jbox_new((t_jbox *)x, boxflags, argc, argv);	//	(t_jbox *)x	//	&x->j_box
	x->m_outlet = intout((t_object *)x);

	x->is_key_pressed = 0;
	x->sel_key = -1;

	x->c4_oct = C4OCTAVE;
	x->oct_div = DFLT_DIVOCT;
	x->n_oct = DFLT_OCTAVES;
	x->gnotes = (fl_gnote *)sysmem_newptr(DFLT_DIVOCT * sizeof(fl_gnote));
	if (!x->gnotes) { object_error((t_object *)x, "out of memory for pnotes"); return x; }
	
	for (int i = 0; i < DFLT_DIVOCT; i++) {
		x->gnotes[i].white = 0;
		x->gnotes[i].prev_ext = 0;
		x->gnotes[i].next_ext = 0;
	}
	x->gnotes[0].white = 1;
	x->gnotes[0].next_ext = 1;
	x->gnotes[2].white = 1;
	x->gnotes[2].prev_ext = 1;
	x->gnotes[2].next_ext = 1;
	x->gnotes[4].white = 1;
	x->gnotes[4].prev_ext = 1;
	x->gnotes[5].white = 1;
	x->gnotes[5].next_ext = 1;
	x->gnotes[7].white = 1;
	x->gnotes[7].prev_ext = 1;
	x->gnotes[7].next_ext = 1;
	x->gnotes[9].white = 1;
	x->gnotes[9].prev_ext = 1;
	x->gnotes[9].next_ext = 1;
	x->gnotes[11].white = 1;
	x->gnotes[11].prev_ext = 1;

	//-------
	x->j_box.b_firstin = (void *)x;
	attr_dictionary_process(x, d);
	jbox_ready((t_jbox *)x);
	
	return x;
}
void fl_wkey_assist(t_fl_wkey *x, void *b, long msg, long arg, char *dst)
{
	if (msg == ASSIST_INLET) {
		switch (arg) {
		case I_INPUT: sprintf(dst, "(messages)");
			break;
		}
	}
}

void fl_wkey_free(t_fl_wkey *x)
{
	sysmem_freeptr(x->gnotes);

	jbox_free((t_jbox *)x);
}

void fl_wkey_int(t_fl_wkey *x, long n)
{
	if (n != n) { return; }
}

void fl_wkey_message(t_fl_wkey *x, t_symbol *s, long argc, t_atom *argv)
{
	long ac = argc;
	t_atom *ap = argv;
	long lenwscale;
	long idx;

	if (ac < 4) { object_error((t_object *)x, "at least 4 args: (oct_div) 'wk' (list)"); return; }

	if (atom_gettype(ap) != A_LONG) { object_error((t_object *)x, "octave division must be an integer greater than 4"); return; }
	if (atom_gettype(ap+1) != A_SYM) { object_error((t_object *)x, "separator must be a non-numeric string"); return; }
	for (long i = 2; i < ac; i++) {
		if (atom_gettype(ap + i) != A_LONG) { object_error((t_object *)x, "list must be made of integers"); return; }
	}

	long oct_div = (long)atom_getlong(ap);
	if (oct_div < 5) { object_error((t_object *)x, "octave division must be an integer greater than 4"); return; }

	lenwscale = ac - 2;
	long *pwscale = (long *)sysmem_newptr(lenwscale * sizeof(long));
	if (!pwscale) { object_error((t_object *)x, "out of memory for pwscale"); return; }
	for (long i = 0; i < lenwscale; i++) { *pwscale = (long)atom_getlong(ap + 2 + i); }

	fl_gnote *pnotes = (fl_gnote *)sysmem_newptr(oct_div * sizeof(fl_gnote));
	if (!pnotes) { object_error((t_object *)x, "out of memory for pnotes"); sysmem_freeptr(pwscale); return; }
	for (long i = 0; i < oct_div; i++) { 
		pnotes->white = 0; 
		pnotes->prev_ext = 0;
		pnotes->next_ext = 0;
	}
	
	for (long i = 0; i < oct_div; i++) {
		for (long j = 0; j < lenwscale; j++) {
			if (i == pwscale[j]) {
				pnotes->white = 1; 
				
				idx = z_mod(j - 1, lenwscale);
				if (pwscale[idx] != z_mod((i - 1), oct_div)) { pnotes->prev_ext = 1; }

				idx = z_mod(j + 1, lenwscale);
				if (pwscale[idx] != z_mod((i + 1), oct_div)) { pnotes->next_ext = 1; }
			}
		}
	}

	sysmem_freeptr(pwscale);

	if (x->gnotes) { sysmem_freeptr(x->gnotes); }
	x->gnotes = pnotes;
	x->oct_div = oct_div;

	//jbox_invalidate_layer((t_object *)x, NULL, gensym("info_layer"));
	//jbox_redraw((t_jbox *)x); 
}

/* outlets ------------------------------------------------------------------- */
/*
void fl_wkey_bang(t_fl_wkey *x)
{
	outlet_bang(x->outlet_bang);
}
void fl_wkey_outmensaje(t_fl_wkey  *x)
{
	fl_nota **hnotas = x->notas_out;
	long total_notas_out = x->total_notas_out;
	
	for (int i = 0; i < total_notas_out; i++) {
		outlet_list(x->outlet_mevent, NULL, (short)hnotas[i]->cnota, hnotas[i]->pnota);
	}
}
void fl_wkey_outcompas(t_fl_wkey  *x)
{
	long n_bar = x->n_bar;

	outlet_int(x->outlet_ibar, n_bar);

	x->jn_bar = n_bar;
	jbox_invalidate_layer((t_object*)x, NULL, ps_bars);
	jbox_redraw((t_jbox *)x);
}
void fl_wkey_outcifra(t_fl_wkey  *x)
{
	outlet_float(x->outlet_fcifra, x->negras);
}
*/

/* aux ui -------------------------------------------------------------------- */
long z_mod(long x, long base)
{
	long y = x;
	long b = labs(base);
	while (y < 0) { y += b; }
	y = y % b;
	return y;
}

t_jrgb hsltorgb(double h, double s, double l) {
	t_jrgb color_rgb;
	double r, g, b, p, q, hue, sat, lig;
	hue = h;
	sat = s;
	lig = l;

	if (sat == 0) {
		color_rgb.red = color_rgb.green = color_rgb.blue = lig; // achromatic
	}
	else {
		q = lig < 0.5 ? lig * (1. + sat) : lig + sat - lig * sat;
		p = 2. * lig - q;
		r = huetorgb(p, q, hue + 1. / 3.);
		g = huetorgb(p, q, hue);
		b = huetorgb(p, q, hue - 1. / 3.);

		color_rgb.red = r;
		color_rgb.green = g;
		color_rgb.blue = b;
	}
	return color_rgb;
}
t_double huetorgb(double p, double q, double t) {
	if (t < 0.) t += 1.;
	if (t > 1.) t -= 1.;
	if (t < 1. / 6.) return p + (q - p) * 6. * t;
	if (t < 1. / 2.) return q;
	if (t < 2. / 3.) return p + (q - p) * (2. / 3. - t) * 6.;
	return p;
}