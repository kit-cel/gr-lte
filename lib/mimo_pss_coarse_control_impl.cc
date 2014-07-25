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
#include "mimo_pss_coarse_control_impl.h"

#include <cstdio>

namespace gr {
  namespace lte {

    mimo_pss_coarse_control::sptr
    mimo_pss_coarse_control::make()
    {
      return gnuradio::get_initial_sptr
        (new mimo_pss_coarse_control_impl());
    }

    /*
     * The private constructor
     */
    mimo_pss_coarse_control_impl::mimo_pss_coarse_control_impl()
      : gr::block("mimo_pss_coarse_control",
              gr::io_signature::make(2, 2, sizeof(gr_complex)),
              gr::io_signature::make(2, 2, sizeof(gr_complex))),
              d_control(false)
    {
        message_port_register_in(pmt::mp("control"));
        set_msg_handler(pmt::mp("control"), boost::bind(&mimo_pss_coarse_control_impl::handle_msg_control, this, _1));

    }

    void
    mimo_pss_coarse_control_impl::handle_msg_control(pmt::pmt_t msg){

        if(msg == pmt::PMT_T)
        {
            d_control=true;
        }
        else{
            d_control=false;
        }
    }

    /*
     * Our virtual destructor.
     */
    mimo_pss_coarse_control_impl::~mimo_pss_coarse_control_impl()
    {
    }

    int
    mimo_pss_coarse_control_impl::general_work(int noutput_items,
                   gr_vector_int &ninput_items,
                   gr_vector_const_void_star &input_items,
                   gr_vector_void_star &output_items)

    {
        const gr_complex *in1 = (const gr_complex *) input_items[0];
        const gr_complex *in2 = (const gr_complex *) input_items[1];
        gr_complex *out1 = (gr_complex *) output_items[0];
        gr_complex *out2 = (gr_complex *) output_items[1];

        consume_each(noutput_items);

        if(!d_control){
            memcpy(out1, in1, sizeof(gr_complex)*noutput_items);
            memcpy(out2, in2, sizeof(gr_complex)*noutput_items);
            return noutput_items;
        }

        return 0;
        // Tell runtime system how many output items we produced.

    }

  } /* namespace lte */
} /* namespace gr */

