/* -*- c++ -*- */
/* 
 * Copyright 2012 Communications Engineering Lab (CEL) / Karlsruhe Institute of Technology (KIT)
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_io_signature.h>
#include <lte_cell_id_daemon.h>


lte_cell_id_daemon_sptr
lte_make_cell_id_daemon (eq_sptr &eq,demux_sptr &demux,descr_sptr &descr)
{
	return lte_cell_id_daemon_sptr (new lte_cell_id_daemon (eq, demux, descr));
}


lte_cell_id_daemon::lte_cell_id_daemon (eq_sptr &eq,demux_sptr &demux,descr_sptr &descr)
	: gr_sync_block ("cell_id_daemon",
		gr_make_io_signature (0, 0, 0),
		gr_make_io_signature (0, 0, 0)),
		d_eq(eq),
		d_demux(demux),
		d_descr(descr)
{
}


lte_cell_id_daemon::~lte_cell_id_daemon ()
{
}

void
lte_cell_id_daemon::set_cell_id(int cell_id)
{
    (*d_eq).set_cell_id(cell_id);
    (*d_demux).set_cell_id(cell_id);
    (*d_descr).set_cell_id(cell_id);
}


int
lte_cell_id_daemon::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
	const float *in = (const float *) input_items[0];
	float *out = (float *) output_items[0];

	// Do <+signal processing+>

	// Tell runtime system how many output items we produced.
	return noutput_items;
}

