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

#ifndef INCLUDED_LTE_REMOVE_CP_CVC_IMPL_H
#define INCLUDED_LTE_REMOVE_CP_CVC_IMPL_H

#include <lte/remove_cp_cvc.h>

namespace gr {
  namespace lte {
    struct sym_info{
        int num;
        int dump;
    };
    
    class remove_cp_cvc_impl : public remove_cp_cvc
    {
     private:
		int d_fftl;
		int d_cpl;
		int d_cpl0;
		int d_slotl;
		int d_symb;
		int d_sym_num;
        int d_symbols_per_frame;
		pmt::pmt_t d_key;
		pmt::pmt_t d_tag_id;
		int d_work_call;
		bool d_found_frame_start;
		long d_frame_start;

		long copy_samples_from_in_to_out(gr_complex* out, const gr_complex* in, int noutput_items);
		int add_tags_to_vectors(int noutput_items, int sym_num, int symbols_per_frame);
		long get_frame_start(std::vector <gr::tag_t> v);
        sym_info get_sym_num_info(long frame_start, long nitems_read, int symbols_per_frame );
        int leading_items_to_dump(int slot_items, int slot_sym);
	

     public:
      remove_cp_cvc_impl(int fftl, std::string key);
      ~remove_cp_cvc_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_REMOVE_CP_CVC_IMPL_H */

