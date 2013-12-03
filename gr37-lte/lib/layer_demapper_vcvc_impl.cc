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
#include "layer_demapper_vcvc_impl.h"
#include <cstdio>

namespace gr {
  namespace lte {

    layer_demapper_vcvc::sptr
    layer_demapper_vcvc::make(int N_ant, int vlen, std::string style)
    {
      return gnuradio::get_initial_sptr
        (new layer_demapper_vcvc_impl(N_ant, vlen, style));
    }

    /*
     * The private constructor
     */
    layer_demapper_vcvc_impl::layer_demapper_vcvc_impl(int N_ant, int vlen, std::string style)
      : gr::sync_block("layer_demapper_vcvc",
              gr::io_signature::make( 1, 1, sizeof(gr_complex) * vlen),
              gr::io_signature::make( 1, 1, sizeof(gr_complex) * vlen)),
              d_N_ant(0),
			  d_vlen(vlen),
			  d_style("null")
    {
		set_N_ant(N_ant);
		set_decoding_style(style);

		pmt::pmt_t msg_buf = pmt::mp("N_ant");
		message_port_register_in(msg_buf);
		set_msg_handler(msg_buf, boost::bind(&layer_demapper_vcvc_impl::handle_msg, this, _1));
	}

    /*
     * Our virtual destructor.
     */
    layer_demapper_vcvc_impl::~layer_demapper_vcvc_impl()
    {
    }

    int
    layer_demapper_vcvc_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
        const gr_complex *in = (const gr_complex *) input_items[0];
        gr_complex *out = (gr_complex *) output_items[0];

        for(int i = 0 ; i < noutput_items; i++){
			if(d_N_ant == 1){
				demap_1_ant(out, in, d_vlen);
			}
			else if(d_N_ant == 2){
				demap_2_ant(out, in, d_vlen);
			}
			else if(d_N_ant == 4){
				demap_4_ant(out, in, d_vlen);
			}
			in+=d_vlen;
			out+=d_vlen;
		}

        // Tell runtime system how many output items we produced.
        return noutput_items;
    }
    
    void
	layer_demapper_vcvc_impl::demap_1_ant(gr_complex* out, const gr_complex * in, int len)
	{
		memcpy(out, in, len*sizeof(gr_complex) );
	}

	void
	layer_demapper_vcvc_impl::demap_2_ant(gr_complex* out, const gr_complex * in, int len)
	{
		int len2 = len/2;
		for(int i = 0 ; i < len2 ; i++ ){
			*(out+2*i+0) = in[i    ];
			*(out+2*i+1) = in[i+len2];
		}
	}

	void
	layer_demapper_vcvc_impl::demap_4_ant(gr_complex* out, const gr_complex * in, int len)
	{
		int len4 = len/4;

		for(int i = 0 ; i < len4 ; i++ ){
			*(out+4*i+0) = in[i    ];
			*(out+4*i+1) = in[i+len4 ];
			*(out+4*i+2) = in[i+2*len4];
			*(out+4*i+3) = in[i+3*len4];
		}
	}

	void
	layer_demapper_vcvc_impl::set_N_ant(int N_ant)
	{
		if(N_ant != 1 && N_ant != 2 && N_ant != 4){
			printf("%s\t N_ant = %i is INVALID!\n", name().c_str(), N_ant);
		}
		else{
			printf("%s\tset N_ant to %i\n",name().c_str(), N_ant);
			d_N_ant = N_ant;
		}
	}

	void
	layer_demapper_vcvc_impl::handle_msg(pmt::pmt_t msg)
	{
		pmt::pmt_t cdr = pmt::cdr(msg);
		set_N_ant(int( pmt::to_long(cdr) ));
	}

	void
	layer_demapper_vcvc_impl::set_decoding_style(std::string style)
	{
		if(style != "tx_diversity"){
			if (style == "spatial_multiplexing"){
				printf("\"%s\" decoding style is valid but not supported\n", style.c_str() );
			}
			else{
				printf("\"%s\" decoding style is invalid\n", style.c_str() );
			}
		}
		else{
			printf("%s\tset decoding style to \"%s\"\n", name().c_str(), style.c_str() );
			d_style = style;
		}
	}

  } /* namespace lte */
} /* namespace gr */

