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

#ifndef INCLUDED_LTE_SSS_TAGGING2_VCVC_H
#define INCLUDED_LTE_SSS_TAGGING2_VCVC_H

#include <lte_api.h>
#include <gr_sync_block.h>
#include <cstdio>

class lte_sss_tagging2_vcvc;
typedef boost::shared_ptr<lte_sss_tagging2_vcvc> lte_sss_tagging2_vcvc_sptr;

LTE_API lte_sss_tagging2_vcvc_sptr lte_make_sss_tagging2_vcvc (int fftl);

/*!
 * \brief This is the second version of the SSS Tag Block
 * It is designed to have a vector input corresponding to one OFDM-symbol
 *
 */
class LTE_API lte_sss_tagging2_vcvc : public gr_sync_block
{
	friend LTE_API lte_sss_tagging2_vcvc_sptr lte_make_sss_tagging2_vcvc (int fftl);

	lte_sss_tagging2_vcvc (int fftl);

 public:
	~lte_sss_tagging2_vcvc ();


	int work (int noutput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items);

    void set_frame_start(long frame_start){d_frame_start = frame_start; }

 private:
    int d_fftl;
    long d_frame_start;
    pmt::pmt_t d_key;
	pmt::pmt_t d_tag;
};

#endif /* INCLUDED_LTE_SSS_TAGGING2_VCVC_H */

