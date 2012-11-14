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

#ifndef INCLUDED_LTE_VECTOR_RESIZE_VBVB_H
#define INCLUDED_LTE_VECTOR_RESIZE_VBVB_H

#include <lte_api.h>
#include <gr_sync_block.h>

class lte_vector_resize_vbvb;
typedef boost::shared_ptr<lte_vector_resize_vbvb> lte_vector_resize_vbvb_sptr;

LTE_API lte_vector_resize_vbvb_sptr lte_make_vector_resize_vbvb (std::vector<int> map, int in_size, int out_size);

/*!
 * \brief <+description+>
 *
 */
class LTE_API lte_vector_resize_vbvb : public gr_sync_block
{
	friend LTE_API lte_vector_resize_vbvb_sptr lte_make_vector_resize_vbvb (std::vector<int> map, int in_size, int out_size);

	lte_vector_resize_vbvb (std::vector<int> map, int in_size, int out_size);

 public:
	~lte_vector_resize_vbvb ();


	int work (int noutput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items);
		
 private:
    std::vector<int> d_map;
    int d_in_size;
    int d_out_size;
    
};

#endif /* INCLUDED_LTE_VECTOR_RESIZE_VBVB_H */

