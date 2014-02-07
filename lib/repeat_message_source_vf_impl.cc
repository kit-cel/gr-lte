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
#include "repeat_message_source_vf_impl.h"

#include <cstdio>

namespace gr {
  namespace lte {

    repeat_message_source_vf::sptr
    repeat_message_source_vf::make(int vector_len, std::string name)
    {
      return gnuradio::get_initial_sptr
        (new repeat_message_source_vf_impl(vector_len, name));
    }

    /*
     * The private constructor
     */
    repeat_message_source_vf_impl::repeat_message_source_vf_impl(int vector_len, std::string& name)
      : gr::sync_block(name,
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make( 1, 1, sizeof(float) * vector_len)),
              d_vector_len(vector_len),
              d_is_initialized(false)
    {
		d_port = pmt::string_to_symbol("vector");
		message_port_register_in(d_port);
		set_msg_handler(pmt::mp("vector"), boost::bind(&repeat_message_source_vf_impl::handle_msg, this, _1));
	}

    /*
     * Our virtual destructor.
     */
    repeat_message_source_vf_impl::~repeat_message_source_vf_impl()
    {
    }

    int
    repeat_message_source_vf_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
        if(d_is_initialized){
			float *out = (float *) output_items[0];
			//~ printf("produce output\n");
			for(int i = 0 ; i < noutput_items ; i++ ){
				memcpy(out, d_out_vector, sizeof(float) * d_vector_len);
				out += d_vector_len;
			}
		}
		else{
			return 0;
		}
        // Tell runtime system how many output items we produced.
        return noutput_items;
    }
    
    void
    repeat_message_source_vf_impl::handle_msg(pmt::pmt_t msg)
    {
		if(pmt::length(msg) != d_vector_len){
			printf("WARNING! Message list length != vector length\n");
			return;
		}
		printf("received message vector\n");

		d_out_vector = new float[d_vector_len];
		for(int c = 0; c < pmt::length(msg); c++ ){
			float val = (float) pmt::to_double( pmt::nth(c, msg) );
			d_out_vector[c] = val;
			//~ printf("%+1.2f\t%+1.2f\tdiff = %+1.2f\n", d_out_vector[c], val, val - d_out_vector[c]);
		}
		d_is_initialized = true;
	}

  } /* namespace lte */
} /* namespace gr */

