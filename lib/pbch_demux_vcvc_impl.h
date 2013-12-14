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

#ifndef INCLUDED_LTE_PBCH_DEMUX_VCVC_IMPL_H
#define INCLUDED_LTE_PBCH_DEMUX_VCVC_IMPL_H

#include <lte/pbch_demux_vcvc.h>

namespace gr {
  namespace lte {

    class pbch_demux_vcvc_impl : public pbch_demux_vcvc
    {
     private:
		int d_cell_id;
		int d_N_rb_dl;
		int d_sym_num;
		gr_complex* d_pbch_symbs;
		gr_complex* d_pbch_ce1_symbs;
		gr_complex* d_pbch_ce2_symbs;

		int calculate_n_process_items(gr_vector_int ninput_items, int noutput_items);
		void extract_pbch_values(gr_complex* out, const gr_complex* in);
		int get_sym_num(std::vector<gr::tag_t> v);
		
		void set_cell_id_msg(pmt::pmt_t msg);

     public:
      pbch_demux_vcvc_impl(int N_rb_dl);
      ~pbch_demux_vcvc_impl();

	  void set_cell_id(int id);
      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_PBCH_DEMUX_VCVC_IMPL_H */

