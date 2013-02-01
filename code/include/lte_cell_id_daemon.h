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

#ifndef INCLUDED_LTE_CELL_ID_DAEMON_H
#define INCLUDED_LTE_CELL_ID_DAEMON_H

#include <lte_api.h>
#include <gr_sync_block.h>
#include <lte_linear_OFDM_estimator_vcvc.h>
#include <lte_pbch_demux_vcvc.h>
#include <lte_descrambling_vfvf.h>

class lte_cell_id_daemon;
typedef boost::shared_ptr<lte_cell_id_daemon> lte_cell_id_daemon_sptr;

typedef boost::shared_ptr<lte_linear_OFDM_estimator_vcvc> eq_sptr;
typedef boost::shared_ptr<lte_pbch_demux_vcvc> demux_sptr;
typedef boost::shared_ptr<lte_descrambling_vfvf> descr_sptr;


LTE_API lte_cell_id_daemon_sptr lte_make_cell_id_daemon (eq_sptr &eq,demux_sptr &demux,descr_sptr &descr);

/*!
 * \brief <+description+>
 *
 */
class LTE_API lte_cell_id_daemon : public gr_sync_block
{
	friend LTE_API lte_cell_id_daemon_sptr lte_make_cell_id_daemon (eq_sptr &eq,demux_sptr &demux,descr_sptr &descr);

	lte_cell_id_daemon (eq_sptr &eq,demux_sptr &demux,descr_sptr &descr);

 public:
	~lte_cell_id_daemon ();


    void set_cell_id(int cell_id);
    void set_eq(eq_sptr eq){d_eq = eq;}
    void set_demux(demux_sptr demux){d_demux = demux;}
    void set_descr(descr_sptr descr){d_descr = descr;}

	int work (int noutput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items);

 private:
    eq_sptr d_eq;
    demux_sptr d_demux;
    descr_sptr d_descr;

    pmt::pmt_t d_port_id;
};

#endif /* INCLUDED_LTE_CELL_ID_DAEMON_H */

