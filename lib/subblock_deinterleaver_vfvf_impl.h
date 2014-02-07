/* -*- c++ -*- */
/* 
 * Copyright 2013 Communications Engineering Lab (CEL) / Karlsruhe Institute of Technology (KIT)
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

#ifndef INCLUDED_LTE_SUBBLOCK_DEINTERLEAVER_VFVF_IMPL_H
#define INCLUDED_LTE_SUBBLOCK_DEINTERLEAVER_VFVF_IMPL_H

#include <lte/subblock_deinterleaver_vfvf.h>

namespace gr {
  namespace lte {

    class subblock_deinterleaver_vfvf_impl : public subblock_deinterleaver_vfvf
    {
     private:
		int d_num_groups;
		int d_items_per_group;
		static const int d_perm_vec[];
		std::vector<int> d_interleaved_pos;
		std::vector<int> subblock_interleaver_prototype(int len);

     public:
      subblock_deinterleaver_vfvf_impl(int num_groups, int items_per_group, std::string& name);
      ~subblock_deinterleaver_vfvf_impl();

      // Where all the action really happens
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_SUBBLOCK_DEINTERLEAVER_VFVF_IMPL_H */

