/* -*- c++ -*- */
/*
 * Copyright 2013 <+YOU OR YOUR COMPANY+>.
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


#ifndef INCLUDED_LTE_PCFICH_DEMUX_VCVC_H
#define INCLUDED_LTE_PCFICH_DEMUX_VCVC_H

#include <lte_api.h>
#include <gr_block.h>

#include <fftw3.h>
#include <volk/volk.h>
#include <cmath>
#include <cstdio>
#include <string>

class lte_pcfich_demux_vcvc;

typedef boost::shared_ptr<lte_pcfich_demux_vcvc> lte_pcfich_demux_vcvc_sptr;

LTE_API lte_pcfich_demux_vcvc_sptr lte_make_pcfich_demux_vcvc (int N_rb_dl, std::string key, std::string out_key, std::string msg_buf_name);

/*!
 * \brief Demux PCFICH from resource grid
 * \ingroup lte
 *
 */
class LTE_API lte_pcfich_demux_vcvc : public gr_block
{
 private:
	friend LTE_API lte_pcfich_demux_vcvc_sptr lte_make_pcfich_demux_vcvc (int N_rb_dl, std::string key, std::string out_key, std::string msg_buf_name);

	lte_pcfich_demux_vcvc(int N_rb_dl, std::string key, std::string out_key, std::string msg_buf_name);

	pmt::pmt_t d_key;
	pmt::pmt_t d_out_key;
	pmt::pmt_t d_tag_id;
	pmt::pmt_t d_msg_buf;
	int d_N_rb_dl;

	int d_cell_id;
	void handle_msg(pmt::pmt_t msg);

	int d_sym_num;

	inline int calculate_n_process_items(gr_vector_int ninput_items);
	inline int get_sym_num(std::vector<gr_tag_t> v);

    std::vector<int> d_pcfich_pos;
	void update_pcfich_pos(int N_rb_dl, int cell_id);

	void extract_pcfich(gr_complex* out0, gr_complex* out1, gr_complex* out2, const gr_complex* in0, const gr_complex* in1, const gr_complex* in2);

 public:
  ~lte_pcfich_demux_vcvc();

	//void forecast (int noutput_items, gr_vector_int &ninput_items_required);
    void set_cell_id(int id);
	// Where all the action really happens
	int general_work (int noutput_items,
	    gr_vector_int &ninput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif /* INCLUDED_LTE_PCFICH_DEMUX_VCVC_H */

