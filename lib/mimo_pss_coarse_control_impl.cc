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
    mimo_pss_coarse_control::make(int rxant)
    {
      return gnuradio::get_initial_sptr
        (new mimo_pss_coarse_control_impl(rxant));
    }

    /*
     * The private constructor
     */
    mimo_pss_coarse_control_impl::mimo_pss_coarse_control_impl(int rxant)
      : gr::block("mimo_pss_coarse_control",
              gr::io_signature::make(1, 8, sizeof(gr_complex)),
              gr::io_signature::make(1, 8, sizeof(gr_complex))),
              d_control(false),
              d_rxant(rxant)
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
         consume_each(noutput_items);

        if(!d_control){
            for(int rx=0; rx<d_rxant; rx++){
                const gr_complex* in = (gr_complex*) input_items[rx];
                gr_complex* out = (gr_complex*) output_items[rx];
                memcpy(out, in, sizeof(gr_complex)*noutput_items);
            }
            return noutput_items;
        }

        return 0;
        // Tell runtime system how many output items we produced.

    }

  } /* namespace lte */
} /* namespace gr */

