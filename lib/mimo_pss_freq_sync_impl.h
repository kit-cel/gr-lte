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

#ifndef INCLUDED_LTE_MIMO_PSS_FREQ_SYNC_IMPL_H
#define INCLUDED_LTE_MIMO_PSS_FREQ_SYNC_IMPL_H

#include <lte/mimo_pss_freq_sync.h>

namespace gr {
  namespace lte {

    class mimo_pss_freq_sync_impl : public mimo_pss_freq_sync
    {
     private:
       int d_fftl;
       int d_rxant;
       int d_N_id_2;
       int d_sampl;
       float d_f_est;
       uint64_t d_pss_pos;
       uint64_t d_f_count;

       gr_complex *d_pssX;
       gr_complex **d_buf_pss;

       pmt::pmt_t d_id_key;
       boost::shared_ptr<gr::analog::sig_source_c> d_sig;

       void mult_memcpy(gr_complex** &out,
                const gr_vector_const_void_star &in,
                int out_pos, int in_pos,
                int multi, size_t n);
       void calc_freq_off();


     public:
      mimo_pss_freq_sync_impl(int fftl, int rxant, boost::shared_ptr<gr::analog::sig_source_c> &sig);
      ~mimo_pss_freq_sync_impl();


      // Where all the action really happens
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_MIMO_PSS_FREQ_SYNC_IMPL_H */

