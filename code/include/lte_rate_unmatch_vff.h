/* -*- c++ -*- */
/* 
 * Copyright 2012 Johannes Demel
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

#ifndef INCLUDED_LTE_RATE_UNMATCH_VFF_H
#define INCLUDED_LTE_RATE_UNMATCH_VFF_H

#include <lte_api.h>
#include <gr_sync_block.h>

class lte_rate_unmatch_vff;
typedef boost::shared_ptr<lte_rate_unmatch_vff> lte_rate_unmatch_vff_sptr;

LTE_API lte_rate_unmatch_vff_sptr lte_make_rate_unmatch_vff ();//int cell_id

/*!
 * \brief <+description+>
 *
 */
class LTE_API lte_rate_unmatch_vff : public gr_sync_block
{
	friend LTE_API lte_rate_unmatch_vff_sptr lte_make_rate_unmatch_vff ();//int cell_id

	lte_rate_unmatch_vff ();//int cell_id

 public:
	~lte_rate_unmatch_vff ();

	int work (int noutput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items);

 private:
	//int d_cell_id;
	float *d_matrix;
	static const int d_ic_perm[]; 
	
	unsigned char d_interl_pos[40];
	int d_interl_pos_len;
	
	int d_work_calls;
	int d_tag_v_size;

	void subblock_interleaver_prototype(int len);
};

#endif /* INCLUDED_LTE_RATE_UNMATCH_VFF_H */

