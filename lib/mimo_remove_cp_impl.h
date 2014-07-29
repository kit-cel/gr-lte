/* -*- c++ -*- */
/*
 * Copyright 2014 Communications Engineering Lab (CEL) / Karlsruhe Institute of Technology (KIT)
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

#ifndef INCLUDED_LTE_MIMO_REMOVE_CP_IMPL_H
#define INCLUDED_LTE_MIMO_REMOVE_CP_IMPL_H

#include <lte/mimo_remove_cp.h>

namespace gr {
  namespace lte {

    struct sym_info{
        int num;
        int dump;
    };

    class mimo_remove_cp_impl : public mimo_remove_cp
    {
     private:
		int d_fftl;
		int d_rxant;
		int d_cpl;
		int d_cpl0;
		int d_slotl;
		int d_symb;     //symbol number within slot
		int d_sym_num;  //symbol number within frame
        int d_symbols_per_frame;
		pmt::pmt_t d_key;
		pmt::pmt_t d_tag_id;
		long d_work_call;
		bool d_found_frame_start;
		long d_half_frame_start;

		long copy_samples_from_in_to_out(gr_vector_void_star &output_items,
                            const gr_vector_const_void_star &input_items,
                            int noutput_items,
                            int sync_delay);
		//void add_tags_to_vectors(int noutput_items, int sym_num, int symbols_per_frame);
        void add_tags_to_vectors(int noutput_items);


     public:
      mimo_remove_cp_impl(int fftl, int rxant, std::string key);
      ~mimo_remove_cp_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_MIMO_REMOVE_CP_IMPL_H */

