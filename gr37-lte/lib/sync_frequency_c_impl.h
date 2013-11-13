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

#ifndef INCLUDED_LTE_SYNC_FREQUENCY_C_IMPL_H
#define INCLUDED_LTE_SYNC_FREQUENCY_C_IMPL_H

#include <lte/sync_frequency_c.h>

namespace gr {
  namespace lte {

    class sync_frequency_c_impl : public sync_frequency_c
    {
     private:
        // basic attributes
        boost::shared_ptr<gr::analog::sig_source_c> d_sig;
        int d_fftl;
        int d_cpl;
        int d_cpl0;
        int d_slotl;
        int d_samp_rate;
        int d_offset;
        gr_complex* d_buffer;

        float d_f_av;
        int d_samp_num;
        int d_work_call;

        //methods for further calculations
        void calc_f_off_av();

        gr_complex corr(gr_complex *res, gr_complex *x, gr_complex *y, int len);

     public:
      sync_frequency_c_impl(boost::shared_ptr<gr::analog::sig_source_c> &sig, int fftl);
      ~sync_frequency_c_impl();

      // Where all the action really happens
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_SYNC_FREQUENCY_C_IMPL_H */

