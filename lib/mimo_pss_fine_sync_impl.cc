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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "mimo_pss_fine_sync_impl.h"

#include <cstdio>
#include <cmath>
#include <fftw3.h>
#include <volk/volk.h>

namespace gr {
  namespace lte {

    mimo_pss_fine_sync::sptr
    mimo_pss_fine_sync::make(int fftl)
    {
      return gnuradio::get_initial_sptr
        (new mimo_pss_fine_sync_impl(fftl));
    }

    /*
     * The private constructor
     */
    mimo_pss_fine_sync_impl::mimo_pss_fine_sync_impl(int fftl)
      : gr::sync_block("mimo_pss_fine_sync",
              gr::io_signature::make(2, 2, sizeof(gr_complex)),
              gr::io_signature::make(0, 0, 0)),
	      d_N_id_2(-1),
              d_coarse_pss(-1),
              d_fftl(fftl),               
              d_cpl(144*fftl/2048),
              d_cpl0(160*fftl/2048),
              d_slotl(7*fftl+6*d_cpl+d_cpl0),
              d_half_frame_start(0),
	      d_corr_val(0),
              d_is_locked(false)
    {

      
	size_t alig = volk_get_alignment();
        d_pss0_t = (gr_complex*) volk_malloc(sizeof(gr_complex)*d_fftl, alig);
        d_pss1_t = (gr_complex*) volk_malloc(sizeof(gr_complex)*d_fftl, alig);
        d_pss2_t = (gr_complex*) volk_malloc(sizeof(gr_complex)*d_fftl, alig);
	
        //init pss sequences in time domain
	mimo_pss_helper::gen_pss_t(d_pss0_t,0, d_fftl);
	mimo_pss_helper::gen_pss_t(d_pss1_t,1, d_fftl);
	mimo_pss_helper::gen_pss_t(d_pss2_t,2, d_fftl);

       message_port_register_in(pmt::mp("N_id_2"));
       set_msg_handler(pmt::mp("N_id_2"), boost::bind(&mimo_pss_fine_sync_impl::handle_msg_N_id_2, this, _1));

       

    }

    /*
     * Our virtual destructor.
     */
    mimo_pss_fine_sync_impl::~mimo_pss_fine_sync_impl()
    {
    }


    void mimo_pss_fine_sync_impl::handle_msg_N_id_2(pmt::pmt_t msg)
    {
	set_N_id_2((int) pmt::to_long(msg));
    }    

    void mimo_pss_fine_sync_impl::handle_msg_coarse_pss(pmt::pmt_t msg)
    {
	set_coarse_pss((int) pmt::to_long(msg));
    }


    int
    mimo_pss_fine_sync_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
        const gr_complex *in1 = (const gr_complex *) input_items[0];
        const gr_complex *in2 = (const gr_complex *) input_items[1];

        // Do <+signal processing+>

        // Tell runtime system how many output items we produced.
        return noutput_items;
    }

  } /* namespace lte */
} /* namespace gr */

