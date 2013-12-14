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

#ifndef INCLUDED_LTE_PCFICH_DEMUX_VCVC_IMPL_H
#define INCLUDED_LTE_PCFICH_DEMUX_VCVC_IMPL_H

#include <lte/pcfich_demux_vcvc.h>
#include <cstdio>

namespace gr {
  namespace lte {

    class pcfich_demux_vcvc_impl : public pcfich_demux_vcvc
    {
     private:
        pmt::pmt_t d_key;
        pmt::pmt_t d_out_key;
        pmt::pmt_t d_tag_id;
        pmt::pmt_t d_msg_buf;
        int d_N_rb_dl;

        int d_cell_id;
        
        // Handle new incoming messages to set cell_id
        void handle_msg(pmt::pmt_t msg){ set_cell_id( int(pmt::to_long(msg)) );}

        int d_sym_num;

        int calculate_n_process_items(gr_vector_int ninput_items);
        inline int get_sym_num(std::vector<gr::tag_t> v);

        std::vector<int> d_pcfich_pos;
        void update_pcfich_pos(int N_rb_dl, int cell_id);

        void extract_pcfich(gr_complex* out0, const gr_complex* in0);


     public:
      pcfich_demux_vcvc_impl(int N_rb_dl, std::string key, std::string out_key, std::string msg_buf_name);
      ~pcfich_demux_vcvc_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);
      
      // This is public to have access to it in QA code.
      // In normal operation it is supposed to be called via handle_msg method.
      void set_cell_id(int id){
        d_cell_id = id;
        update_pcfich_pos(d_N_rb_dl, d_cell_id);
        printf("%s\t set cell_id = %i\n", name().c_str(), d_cell_id);
      }

      int general_work(int noutput_items,
                gr_vector_int &ninput_items,
                gr_vector_const_void_star &input_items,
                gr_vector_void_star &output_items);
    };

  } // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_PCFICH_DEMUX_VCVC_IMPL_H */

