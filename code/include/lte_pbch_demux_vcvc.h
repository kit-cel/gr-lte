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

#ifndef INCLUDED_LTE_PBCH_DEMUX_VCVC_H
#define INCLUDED_LTE_PBCH_DEMUX_VCVC_H

#include <lte_api.h>
#include <gr_block.h>

class lte_pbch_demux_vcvc;
typedef boost::shared_ptr<lte_pbch_demux_vcvc> lte_pbch_demux_vcvc_sptr;

LTE_API lte_pbch_demux_vcvc_sptr lte_make_pbch_demux_vcvc (int N_rb_dl); //int cell_id,

/*!
 * \brief Demultiplex PBCH data from resource grid
 *
 */
class LTE_API lte_pbch_demux_vcvc : public gr_block
{
	friend LTE_API lte_pbch_demux_vcvc_sptr lte_make_pbch_demux_vcvc (int N_rb_dl); //int cell_id,

	lte_pbch_demux_vcvc (int N_rb_dl); //int cell_id,

 public:
	~lte_pbch_demux_vcvc ();

    void set_cell_id(int id);
    void set_cell_id_msg(pmt::pmt_t msg);


    int general_work (int noutput_items,
		    gr_vector_int &ninput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);

 private:
    int d_cell_id;
    int d_N_rb_dl;
    int d_sym_num;
    gr_complex* d_pbch_symbs;
	gr_complex* d_pbch_ce1_symbs;
	gr_complex* d_pbch_ce2_symbs;

	inline int calculate_n_process_items(gr_vector_int ninput_items, int noutput_items);
	void extract_pbch_values(const gr_complex* in1, const gr_complex* in2, const gr_complex* in3,
                    gr_complex* out1, gr_complex* out2, gr_complex* out3);
    inline int get_sym_num(std::vector<gr_tag_t> v);

};

#endif /* INCLUDED_LTE_PBCH_DEMUX_VCVC_H */

