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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "mimo_extract_subcarriers_impl.h"

namespace gr {
  namespace lte {

    mimo_extract_subcarriers::sptr
    mimo_extract_subcarriers::make(int N_rb_dl, int fftl, int rxant)
    {
      return gnuradio::get_initial_sptr
        (new mimo_extract_subcarriers_impl(N_rb_dl, fftl, rxant));
    }

    /*
     * The private constructor
     */
    mimo_extract_subcarriers_impl::mimo_extract_subcarriers_impl(int N_rb_dl, int fftl, int rxant)
      : gr::sync_block("mimo_extract_subcarriers",
              gr::io_signature::make( 1, 8, sizeof(gr_complex) * fftl),
              gr::io_signature::make( 1, 1, sizeof(gr_complex) * 12 * N_rb_dl * rxant)),
              d_N_rb_dl(N_rb_dl),
              d_fftl(fftl),
              d_rxant(rxant)
    {}

    /*
     * Our virtual destructor.
     */
    mimo_extract_subcarriers_impl::~mimo_extract_subcarriers_impl()
    {
    }

    int
    mimo_extract_subcarriers_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {

        int h_byte_tones_num = sizeof(gr_complex) * 6 * d_N_rb_dl;

        for(int rx=0; rx<d_rxant; rx++){
            gr_complex *out = (gr_complex *) output_items[0];
            const gr_complex *in = (const gr_complex *) input_items[rx];

            for (int i = 0; i < noutput_items; i++){
                memcpy(out+(rx*12*d_N_rb_dl), in+(d_fftl-(6*d_N_rb_dl)), h_byte_tones_num );
                memcpy(out+(6*d_N_rb_dl)+(rx*12*d_N_rb_dl), in+1, h_byte_tones_num);

                in  += d_fftl;
                out += 12 * d_N_rb_dl * d_rxant;
            }
		}

		// Tell runtime system how many output items we produced.
		return noutput_items;
    }

  } /* namespace lte */
} /* namespace gr */

