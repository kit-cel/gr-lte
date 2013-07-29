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


#ifndef INCLUDED_LTE_CFI_UNPACK_VF_H
#define INCLUDED_LTE_CFI_UNPACK_VF_H

#include <lte_api.h>
#include <gr_sync_block.h>

#include <fftw3.h>
#include <volk/volk.h>

class lte_cfi_unpack_vf;

typedef boost::shared_ptr<lte_cfi_unpack_vf> lte_cfi_unpack_vf_sptr;

LTE_API lte_cfi_unpack_vf_sptr lte_make_cfi_unpack_vf (std::string key);

/*!
 * \brief <+description+>
 * \ingroup lte
 *
 */
class LTE_API lte_cfi_unpack_vf : public gr_sync_block
{
 private:
	friend LTE_API lte_cfi_unpack_vf_sptr lte_make_cfi_unpack_vf (std::string key);

	lte_cfi_unpack_vf(std::string key);

	pmt::pmt_t d_port_cfi;
	pmt::pmt_t d_key;
	float* d_in_seq;
	std::vector<float*> d_ref_seqs;
	void initialize_ref_seqs();
	float correlate(float* in0, float* in1, int len);

 public:
  ~lte_cfi_unpack_vf();

	// Where all the action really happens
	int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif /* INCLUDED_LTE_CFI_UNPACK_VF_H */

