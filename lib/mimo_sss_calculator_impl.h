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

#ifndef INCLUDED_LTE_MIMO_SSS_CALCULATOR_IMPL_H
#define INCLUDED_LTE_MIMO_SSS_CALCULATOR_IMPL_H

#include <lte/mimo_sss_calculator.h>

namespace gr {
  namespace lte {

    struct sss_info{
        int pos;
        int N_id_1;
    };


    class mimo_sss_calculator_impl : public mimo_sss_calculator
    {
     private:
        int d_N_id_2;
        int d_cell_id;
        int d_fftl;
        int d_rxant;
        int d_slotl;
        char d_cX[31];
        gr_complex d_sref[62];
        char d_zX[31];
        int d_v_m0[168];
        int d_v_m1[168];
        float d_max_val_new;
        float d_max_val_old;
        long d_offset;
        int d_sss_pos;
        long d_frame_start;
        bool d_is_locked;
        int d_unchanged_id;

        pmt::pmt_t d_key_id;
        pmt::pmt_t d_key_offset;

        // calculation functions!
        int calc_m(gr_complex **s0m0, int rxant);
        int get_N_id_1(int m0, int m1);
        sss_info get_sss_info(gr_complex** &even, gr_complex** &odd, int N_id_2, int rxant);
        gr_complex corr(gr_complex *x,gr_complex *y, int len);
        void xcorr(std::vector<float> &v, gr_complex **x,gr_complex *y, int len, int rxant);

        pmt::pmt_t d_port_cell_id;
        pmt::pmt_t d_port_frame_start;
        void publish_cell_id(int cell_id);
        void publish_frame_start(long frame_start);
        void msg_set_sector_id(pmt::pmt_t msg);

     public:
      mimo_sss_calculator_impl(int fftl, int rxant);
      ~mimo_sss_calculator_impl();

      // Where all the action really happens
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);

      int get_cell_id(){return d_cell_id;}
      long get_frame_start(){return d_frame_start;}
    };

  } // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_MIMO_SSS_CALCULATOR_IMPL_H */

