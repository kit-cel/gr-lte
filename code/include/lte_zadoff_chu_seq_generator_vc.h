/* -*- c++ -*- */
/* 
 * Copyright 2012 <+YOU OR YOUR COMPANY+>.
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

#ifndef INCLUDED_LTE_ZADOFF_CHU_SEQ_GENERATOR_VC_H
#define INCLUDED_LTE_ZADOFF_CHU_SEQ_GENERATOR_VC_H

#include <lte_api.h>
#include <gr_sync_block.h>

class lte_zadoff_chu_seq_generator_vc;
typedef boost::shared_ptr<lte_zadoff_chu_seq_generator_vc> lte_zadoff_chu_seq_generator_vc_sptr;

LTE_API lte_zadoff_chu_seq_generator_vc_sptr lte_make_zadoff_chu_seq_generator_vc (int len, int init_val);

/*!
 * \brief <+description+>
 *
 */
class LTE_API lte_zadoff_chu_seq_generator_vc : public gr_sync_block
{
	friend LTE_API lte_zadoff_chu_seq_generator_vc_sptr lte_make_zadoff_chu_seq_generator_vc (int len, int init_val);

	lte_zadoff_chu_seq_generator_vc (int len, int init_val);

 public:
	~lte_zadoff_chu_seq_generator_vc ();


	int work (int noutput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items);
		
 private:
    int d_len;
    int d_init_val;
    static const float d_PI;
    static const gr_complex d_C_I;
    gr_complex* d_seq;
};

#endif /* INCLUDED_LTE_ZADOFF_CHU_SEQ_GENERATOR_VC_H */

