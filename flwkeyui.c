#include "flwkey.h"

void fl_wkey_paint(t_fl_wkey *x, t_object *patcherview)
{
	t_rect rect;
	t_jrgba c, c2, c3;
	t_jgraphics *g;
	fl_gnote *pnotes = x->gnotes;

	long *ppoly = x->polynotes;
	long total_poly = x->total_polynotes;

	long oct_div = x->oct_div;
	long n_oct = x->n_oct;
	long total_notes = n_oct * oct_div + 1;

	long idx, idx_next;
	double aux_val;
	double posx, posy;
	double sq_w, sq_h;

	short is_key_pressed = x->is_key_pressed;
	long sel_key = x->sel_key;

	//resize
	jbox_get_patching_rect((t_object *)x, &rect);
	//rect.x = rect.x; rect.y = rect.y;
	rect.width = 2 * HNOTEW * (oct_div * n_oct + 1) + 2 * ALLFRAME;
	rect.height = 3 * TNOTEH + SUPMARGIN + 2 * ALLFRAME;
	jbox_set_patching_rect((t_object *)x, &rect);

	g = (t_jgraphics *)patcherview_get_jgraphics(patcherview);
	jbox_get_rect_for_view((t_object *)x, patcherview, &rect);

	//background
	object_attr_getjrgba((t_object *)x, _sym_bgcolor, &c);
	jgraphics_set_source_jrgba(g, &c);
	jgraphics_rectangle_fill_fast(g, 0, 0, rect.width, rect.height);

	//polynotes
	c2.red = 1.; c2.green = 0.; c2.blue = 0.5; c2.alpha = 1.;
	//zeromark
	c3.red = 1.; c3.green = 1.; c3.blue = 1.; c3.alpha = 1.;

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
				posx = 2. * HNOTEW * i;
				posy = 0.;
				sq_w = 2 * HNOTEW;
				sq_h = 2 * TNOTEH;
				jgraphics_set_source_jrgba(g, &c);
				jgraphics_rectangle_fill_fast(g, posx, posy, sq_w, sq_h);
			}
			else {
				posx = 2. * HNOTEW * i;
				posy = 0.;
				sq_w = 2 * HNOTEW;
				sq_h = 2 * TNOTEH;
				jgraphics_set_source_jrgba(g, &c);
				jgraphics_rectangle_fill_fast(g, posx, posy, sq_w, sq_h);

				posx = HNOTEW * (2. * i - pnotes[idx].prev_exten);
				posy = 2 * TNOTEH;
				sq_w = HNOTEW * (2 + pnotes[idx].next_exten + pnotes[idx].prev_exten);
				sq_h = TNOTEH;
				jgraphics_set_source_jrgba(g, &c);
				jgraphics_rectangle_fill_fast(g, posx, posy, sq_w, sq_h);
			}

			//zeromark
			if (!idx) {
				posx = i * 2. * HNOTEW + HNOTEW - (0.5 * ZEROMARKD);
				posy = 2.5 * TNOTEH;
				sq_w = ZEROMARKD;
				sq_h = ZEROMARKD;
				if (pnotes[idx].prev_exten) { posx -= 0.5 * HNOTEW; }
				if (pnotes[idx].next_exten) { posx += 0.5 * HNOTEW; }
				jgraphics_set_source_jrgba(g, &c3);
				jgraphics_ellipse(g, posx, posy, sq_w, sq_h);
			}
		}

		//-------------------------------------------------------------------
		//lines
		for (long i = 0; i < total_notes; i++) {

			idx = z_mod(i, oct_div);
			idx_next = z_mod(i + 1, oct_div);
			if (!pnotes[idx].white) {
				
				c.red = 1.;
				c.green = 1.;
				c.blue = 1.;
				c.alpha = 1.;
				//object_attr_getjrgba((t_object *)x, gensym("elementcolor"), &c);
				jgraphics_set_source_jrgba(g, &c);
				jgraphics_set_line_width(g, 1.);

				if (!pnotes[idx_next].white) {
					posx = 2. * HNOTEW * (i + 1);
					posy = 0.;

					jgraphics_move_to(g, posx, posy);
					jgraphics_line_to(g, posx, 2. * TNOTEH);
					jgraphics_stroke(g);
				}
			}
			else {

				c.red = 0.;
				c.green = 0.;
				c.blue = 0.;
				c.alpha = 1.;
				//object_attr_getjrgba((t_object *)x, gensym("elementcolor"), &c);
				jgraphics_set_source_jrgba(g, &c);
				jgraphics_set_line_width(g, 1.);

				if (pnotes[idx_next].white) {
					posx = 2. * HNOTEW * (i + 1);
					posy = 0;

					jgraphics_move_to(g, posx, posy);
					jgraphics_line_to(g, posx, 3 * TNOTEH);
					jgraphics_stroke(g);
				}
				else {
					posx = HNOTEW * (2. * (i + 1) + pnotes[idx].next_exten);
					posy = 2. * TNOTEH;

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

				posx = HNOTEW * (2. * sel_key - pnotes[idx].prev_exten);
				posy = 2 * TNOTEH;
				sq_w = HNOTEW * (2 + pnotes[idx].next_exten + pnotes[idx].prev_exten);
				sq_h = TNOTEH;
				jgraphics_set_source_jrgba(g, &c);
				jgraphics_rectangle_fill_fast(g, posx, posy, sq_w, sq_h);
			}
		}

		//polynotes
		for (long i = 0; i < total_poly; i++) {
			
			sel_key = ppoly[i] - oct_div * (5 - x->c4_oct);

			if (sel_key < 0 || sel_key > total_notes) { continue; }

			idx = z_mod(sel_key, oct_div);

			sq_w = POLYMARKE;
			sq_h = POLYMARKE;

			if (!pnotes[idx].white) {
				posx = sel_key * 2. * HNOTEW + HNOTEW - (0.5 * POLYMARKE);
				posy = 1.5 * TNOTEH - (0.5 * POLYMARKE);
				jgraphics_set_source_jrgba(g, &c2);
				jgraphics_rectangle_fill_fast(g, posx, posy, sq_w, sq_h);
			}
			else {
				posx = sel_key * 2. * HNOTEW + HNOTEW - (0.5 * POLYMARKE);
				posy = 2.5 * TNOTEH - (0.5 * POLYMARKE);
				if (pnotes[idx].prev_exten) { posx -= 0.5 * HNOTEW; }
				if (pnotes[idx].next_exten) { posx += 0.5 * HNOTEW; }
				jgraphics_set_source_jrgba(g, &c2);
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
	long sel_key = -1;
	long sel_notex, sel_notey;
	long note_side;
	long idx, idx_prev, idx_next;
	long count_prev, count_next;

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
		sel_notex = (long)((pt.x - ALLFRAME) / (2. * HNOTEW));
		note_side = ((long)(pt.x - ALLFRAME) / (long)(HNOTEW));
		note_side = z_mod(note_side, 2); //0 left, 1 right

		idx = z_mod(sel_notex, oct_div);
		idx_prev = idx;
		idx_next = idx;
		count_next = 0;
		count_prev = 0;

		if (sel_notey) { //down
			if (pnotes[idx].white) { 
				sel_key = sel_notex; 
				is_over_key = 1;
			}
			else {

				do {
					idx_next = z_mod(idx_next + 1, oct_div);
					count_next++;
				} while (!pnotes[idx_next].white);
				
				do {
					idx_prev = z_mod(idx_prev - 1, oct_div);
					count_prev++;
				} while (!pnotes[idx_prev].white);

				if (count_prev == count_next) {
					if (note_side) { sel_key = sel_notex + count_next; is_over_key = 1; }
					else { sel_key = sel_notex - count_prev; }
				}
				else if (count_prev < count_next) { sel_key = sel_notex - count_prev; }
				else { sel_key = sel_notex + count_next; }
				
				is_over_key = 1;
			}
		}
		else { //up
			sel_key = sel_notex;
			is_over_key = 1;
		}
	}
	else { is_over_key = 0; }
	
	x->sel_key = sel_key;
	transp_note = sel_key + oct_div * (5 - x->c4_oct);

	if (is_key_pressed && is_over_key && last_over != sel_key) {
		outlet_int(x->m_outlet, transp_note);

		jbox_invalidate_layer((t_object *)x, NULL, gensym("notes_layer"));
		jbox_redraw((t_jbox *)x);
	}
}