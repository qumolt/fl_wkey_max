#include "flwkey.h"

/*
void fl_wkey_tick(t_fl_wkey *x)
{
	if (x->pos_cursor_norm != x->fasor_cursor) {
		x->pos_cursor_norm = x->fasor_cursor;
		jbox_invalidate_layer((t_object *)x, NULL, ps_cursor_layer);
		jbox_redraw((t_jbox *)x);
	}

	if (sys_getdspstate()) {	// if the dsp is still on, schedule a next pictmeter_tick() call
		clock_fdelay(x->cursor_clock, 50);
	}
}
*/

void fl_wkey_paint(t_fl_wkey *x, t_object *patcherview)
{
	t_rect rect;
	t_jrgba c;
	t_jgraphics *g;
	fl_gnote *pnotes = x->gnotes;

	long oct_div = x->oct_div;
	long n_oct = x->n_oct;
	long total_notes = n_oct * oct_div + 1;

	long idx;
	long idx_prev;
	long idx_next;
	double aux_val;
	double posx, posy;
	double sq_w, sq_h;

	short is_key_pressed = x->is_key_pressed;
	long sel_key = x->sel_key;

	//resize
	jbox_get_patching_rect((t_object *)x, &rect);
	//newrect.x = oldrect.x;
	//newrect.y = oldrect.y;
	rect.width = 2 * HNOTEW * (oct_div * n_oct + 1) + 2 * ALLFRAME;
	rect.height = 3 * TNOTEH + SUPMARGIN + 2 * ALLFRAME;
	jbox_set_patching_rect((t_object *)x, &rect);

	g = (t_jgraphics *)patcherview_get_jgraphics(patcherview);
	jbox_get_rect_for_view((t_object *)x, patcherview, &rect);

	//background
	object_attr_getjrgba((t_object *)x, _sym_bgcolor, &c);
	jgraphics_set_source_jrgba(g, &c);
	jgraphics_rectangle_fill_fast(g, 0, 0, rect.width, rect.height);

	//notes
	rect.width = 2 * HNOTEW * (oct_div * n_oct + 1);
	rect.height = 3 * TNOTEH;
	g = jbox_start_layer((t_object *)x, patcherview, gensym("notes_layer"), rect.width, rect.height);
	if (g) {

		//keys
		for (long i = 0; i < total_notes; i++) {
			idx = z_mod(i, oct_div);

			aux_val = pnotes[idx].white ? 1. : 0;
			c.red = aux_val;
			c.green = aux_val;
			c.blue = aux_val;
			c.alpha = 1.;

			if (!pnotes[idx].white) {
				posx = i * 2. * HNOTEW;
				posy = 0.;
				sq_w = 2 * HNOTEW;
				sq_h = 2 * TNOTEH;
				jgraphics_set_source_jrgba(g, &c);
				jgraphics_rectangle_fill_fast(g, posx, posy, sq_w, sq_h);
			}
			else {
				posx = i * 2. * HNOTEW;
				posy = 0.;
				sq_w = 2 * HNOTEW;
				sq_h = 2 * TNOTEH;
				jgraphics_set_source_jrgba(g, &c);
				jgraphics_rectangle_fill_fast(g, posx, posy, sq_w, sq_h);

				posx = i * 2. * HNOTEW;
				posy = 2 * TNOTEH;
				sq_w = 2 * HNOTEW;
				sq_h = TNOTEH;
				if (pnotes[idx].prev_ext) {
					posx -= HNOTEW;
					sq_w += HNOTEW;
				}
				if (pnotes[idx].next_ext) {
					sq_w += HNOTEW;
				}
				jgraphics_set_source_jrgba(g, &c);
				jgraphics_rectangle_fill_fast(g, posx, posy, sq_w, sq_h);

			}
		}

		//-------------------------------------------------------------------
		//lines
		c.red = 0.;
		c.green = 0.;
		c.blue = 0.;
		c.alpha = 1.;

		//object_attr_getjrgba((t_object *)x, gensym("elementcolor"), &c);
		jgraphics_set_source_jrgba(g, &c);
		jgraphics_set_line_width(g, 1.);

		for (long i = 1; i < total_notes; i++) {
			
			idx = z_mod(i, oct_div);
			idx_prev = z_mod(i - 1, oct_div);
			idx_next = z_mod(i + 1, oct_div);
			if (!pnotes[idx].white) {
				if (pnotes[idx_prev].white && pnotes[idx_next].white) {
					posx = i * 2. * HNOTEW + HNOTEW;
					posy = 2 * TNOTEH;

					jgraphics_move_to(g, posx, posy);
					jgraphics_line_to(g, posx, 3 * TNOTEH);
					jgraphics_stroke(g);
				}
			}
			else {
				if (!pnotes[idx].prev_ext) {
					posx = i * 2. * HNOTEW;
					posy = 0;

					jgraphics_move_to(g, posx, posy);
					jgraphics_line_to(g, posx, 3 * TNOTEH);
					jgraphics_stroke(g);
				}
			}
		}

		//-------------------------------------------------------------------
		//pressed key
		if (is_key_pressed) {
			idx = z_mod(sel_key, oct_div);

			//aux_val = pnotes[idx].white ? 1. : 0;
			aux_val = 0.5;
			c.red = aux_val;
			c.green = aux_val;
			c.blue = aux_val;
			c.alpha = 1.;

			if (!pnotes[idx].white) {
				posx = sel_key * 2. * HNOTEW;
				posy = 0.;
				sq_w = 2 * HNOTEW;
				sq_h = 2 * TNOTEH;
				jgraphics_set_source_jrgba(g, &c);
				jgraphics_rectangle_fill_fast(g, posx, posy, sq_w, sq_h);
			}
			else {
				posx = sel_key * 2. * HNOTEW;
				posy = 0.;
				sq_w = 2 * HNOTEW;
				sq_h = 2 * TNOTEH;
				jgraphics_set_source_jrgba(g, &c);
				jgraphics_rectangle_fill_fast(g, posx, posy, sq_w, sq_h);

				posx = sel_key * 2. * HNOTEW;
				posy = 2 * TNOTEH;
				sq_w = 2 * HNOTEW;
				sq_h = TNOTEH;
				if (pnotes[idx].prev_ext) {
					posx -= HNOTEW;
					sq_w += HNOTEW;
				}
				if (pnotes[idx].next_ext) {
					sq_w += HNOTEW;
				}
				jgraphics_set_source_jrgba(g, &c);
				jgraphics_rectangle_fill_fast(g, posx, posy, sq_w, sq_h);
			}
		}

		jbox_end_layer((t_object *)x, patcherview, gensym("notes_layer"));
	}
	jbox_paint_layer((t_object *)x, patcherview, gensym("notes_layer"), ALLFRAME, SUPMARGIN);	// position of the layer
}

void fl_wkey_mousedrag(t_fl_wkey *x, t_object *patcherview, t_pt pt, long modifiers)
{
	x->is_key_pressed = 1;
	mouse_sendkey(x, patcherview, pt);
}

void fl_wkey_mousedown(t_fl_wkey *x, t_object *patcherview, t_pt pt, long modifiers)
{
	x->is_key_pressed = 1;
	x->sel_key = -1;
	mouse_sendkey(x, patcherview, pt);
}

void fl_wkey_mouseup(t_fl_wkey *x, t_object *patcherview, t_pt pt, long modifiers)
{
	x->is_key_pressed = 0;

	jbox_invalidate_layer((t_object *)x, NULL, gensym("notes_layer"));
	jbox_redraw((t_jbox *)x);
}

void mouse_sendkey(t_fl_wkey *x, t_object *patcherview, t_pt pt) 
{
	t_rect rect;
	double allwidth, allheight;
	long sel_note = -1;
	long sel_notex, sel_notey;
	long note_side;
	long idx, idx_prev, idx_next;

	short is_over_key = 0;
	short is_key_pressed = x->is_key_pressed;
	long last_over = x->sel_key;

	long oct_div = x->oct_div;
	long n_oct = x->n_oct;
	fl_gnote *pnotes = x->gnotes;
	long transp_note;

	jbox_get_rect_for_view((t_object *)x, patcherview, &rect);

	allwidth = 2 * HNOTEW * (oct_div * n_oct + 1) + 2 * ALLFRAME;
	allheight = 3 * TNOTEH + SUPMARGIN + 2 * ALLFRAME;

	if (pt.x > ALLFRAME && pt.x < (allwidth - ALLFRAME) && pt.y >(SUPMARGIN + ALLFRAME) && pt.y < (allheight - ALLFRAME)) {

		sel_notey = (long)(pt.y - ALLFRAME - SUPMARGIN) / (long)(2. * TNOTEH); //0 up, 1 down
		sel_notex = (long)(pt.x - ALLFRAME) / (long)(2 * HNOTEW);
		note_side = ((long)(pt.x - ALLFRAME) / (long)(HNOTEW));
		note_side = z_mod(note_side, 2); //0 left, 1 right

		idx = z_mod(sel_notex, oct_div);
		idx_prev = z_mod(sel_notex - 1, oct_div);
		idx_next = z_mod(sel_notex + 1, oct_div);

		if (sel_notey) {
			if (pnotes[idx].white) { 
				sel_note = sel_notex; 
				is_over_key = 1;
			}
			else {
				if (note_side) {
					if (pnotes[idx_next].prev_ext) {
						sel_note = sel_notex + 1;
						is_over_key = 1;
					}
					else { is_over_key = 0; }
				}
				else {
					if (pnotes[idx_prev].next_ext) {
						sel_note = sel_notex - 1;
						is_over_key = 1;
					}
					else { is_over_key = 0; }
				}
			}
		}
		else {
			sel_note = sel_notex;
			is_over_key = 1;
		}
	}
	else { is_over_key = 0; }
	
	x->sel_key = sel_note;
	transp_note = sel_note + oct_div * (5 - x->c4_oct);

	if (is_key_pressed && is_over_key && last_over != sel_note) {
		outlet_int(x->m_outlet, transp_note);

		jbox_invalidate_layer((t_object *)x, NULL, gensym("notes_layer"));
		jbox_redraw((t_jbox *)x);
	}
}