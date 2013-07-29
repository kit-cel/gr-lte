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


#ifndef INCLUDED_LTE_PCFICH_DESCRAMBLER_VFVF_H
#define INCLUDED_LTE_PCFICH_DESCRAMBLER_VFVF_H

#include <lte_api.h>
#include <gr_sync_block.h>

#include <fftw3.h>
#include <volk/volk.h>

class lte_pcfich_descrambler_vfvf;

typedef boost::shared_ptr<lte_pcfich_descrambler_vfvf> lte_pcfich_descrambler_vfvf_sptr;

LTE_API lte_pcfich_descrambler_vfvf_sptr lte_make_pcfich_descrambler_vfvf (std::string tag_key, std::string msg_buf_name);

/*!
 * \brief Descrambling block for PCFICH
 * \ingroup lte
 *
 */
class LTE_API lte_pcfich_descrambler_vfvf : public gr_sync_block
{
 private:
	friend LTE_API lte_pcfich_descrambler_vfvf_sptr lte_make_pcfich_descrambler_vfvf (std::string tag_key, std::string msg_buf_name);

	lte_pcfich_descrambler_vfvf(std::string tag_key, std::string msg_buf_name);

	pmt::pmt_t d_tag_key;
	pmt::pmt_t d_msg_buf;
	int d_cell_id;
	int d_subframe;

	std::vector<float*> scr_seq_vec;

	inline void handle_msg(pmt::pmt_t msg);
	inline void setup_descr_seqs(int cell_id);
    inline float* generate_scr_seq(int cell_id, int ns);
    inline void pn_seq_generator(float* vec, int len, int cinit);
    void encode_nrz(float* out, float* in, int len);

 public:
  ~lte_pcfich_descrambler_vfvf();

    inline void set_cell_id(int cell_id);
    std::vector<std::vector<float> > get_descr_seqs();

	// Where all the action really happens
	int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif /* INCLUDED_LTE_PCFICH_DESCRAMBLER_VFVF_H */

