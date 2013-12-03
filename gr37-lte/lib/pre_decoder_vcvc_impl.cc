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
#include "pre_decoder_vcvc_impl.h"

#include <cstdio>
#include <cmath>
#include <fftw3.h>
#include <volk/volk.h>

namespace gr {
  namespace lte {

    pre_decoder_vcvc::sptr
    pre_decoder_vcvc::make(int N_ant, int vlen, std::string style)
    {
      return gnuradio::get_initial_sptr
        (new pre_decoder_vcvc_impl(N_ant, vlen, style));
    }

    /*
     * The private constructor
     */
    pre_decoder_vcvc_impl::pre_decoder_vcvc_impl(int N_ant, int vlen, std::string style)
      : gr::sync_block("pre_decoder_vcvc",
              gr::io_signature::make( 2, 3, sizeof(gr_complex) * vlen),
              gr::io_signature::make( 1, 1, sizeof(gr_complex) * vlen)),
              d_vlen(vlen)
    {
	    set_N_ant(N_ant);
		set_decoding_style(style);
		setup_volk_vectors(vlen);

		pmt::pmt_t msg_buf = pmt::mp("N_ant");
		message_port_register_in(msg_buf);
		set_msg_handler(msg_buf, boost::bind(&pre_decoder_vcvc_impl::handle_msg, this, _1));	
	}

    /*
     * Our virtual destructor.
     */
    pre_decoder_vcvc_impl::~pre_decoder_vcvc_impl()
    {
    }

    int
    pre_decoder_vcvc_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
        const gr_complex *in1 = (const gr_complex *) input_items[0];
		const gr_complex *in2 = (const gr_complex *) input_items[1];
		gr_complex *out = (gr_complex *) output_items[0];

		if (d_N_ant == 1){
			for(int i = 0; i < noutput_items; i++){
				decode_1_ant(out, in1, in2, d_vlen);
				out += d_vlen;
				in1 += d_vlen;
				in2 += d_vlen;
			}
		}
		else if(d_N_ant == 2){
			const gr_complex *in3 = (const gr_complex *) input_items[2];
			for(int i = 0; i < noutput_items; i++){
				prepare_2_ant_vectors(d_h0, d_h1, d_r0, d_r1, in1, in2, in3, d_vlen);
				decode_2_ant(d_out0, d_out1, d_h0, d_h1, d_r0, d_r1, d_vlen/2);
				combine_output(out, d_out0, d_out1, d_vlen);
				out += d_vlen;
				in1 += d_vlen;
				in2 += d_vlen;
				in3 += d_vlen;
			}
		}

		// Tell runtime system how many output items we produced.
		return noutput_items;
    }
    		
	void
	pre_decoder_vcvc_impl::decode_1_ant(gr_complex* out,
									   const gr_complex* rx,
									   const gr_complex* h,
									   int len)
	{
		for(int i = 0 ; i < len ; i++ ){
			*(out+i) = rx[i]/h[i];
		}
	}

	void
	pre_decoder_vcvc_impl::prepare_2_ant_vectors(gr_complex* h0,
												gr_complex* h1,
												gr_complex* r0,
												gr_complex* r1,
												const gr_complex* rx,
												const gr_complex* ce1,
												const gr_complex* ce2,
												int len)
	{
		for(int n = 0; n < len/2; n++){
			h0[n] = ce1[2*n];
			h1[n] = ce2[2*n];
			r0[n] = rx[2*n];
			r1[n] = rx[2*n+1];
		}
	}

	void
	pre_decoder_vcvc_impl::decode_2_ant(gr_complex* out0,
									   gr_complex* out1,
									   gr_complex* h0,
									   gr_complex* h1,
									   gr_complex* r0,
									   gr_complex* r1,
									   int len)
	{
		/*
		alamouti Coding
			time0   time1
		ant0  x0    x1
		ant1 -x1*   x0*

		RX
		r0 = h0 x0 - x1* h1
		r1 = h0 x1 + h1 x0*

		estimate
		e_x0 = h0* r0 + h1 r1*
		e_x1 = h0* r1 - h1 r0*
		*/

		// e_x0
		volk_32fc_x2_multiply_conjugate_32fc_a(d_mult0, r0, h0, len);
		volk_32fc_x2_multiply_conjugate_32fc_a(d_mult1, h1, r1, len);
		volk_32f_x2_add_32f_a( (float*)out0, (float*)d_mult0, (float*)d_mult1, 2*len);

		//e_x1
		volk_32fc_x2_multiply_conjugate_32fc_a(d_mult0, r1, h0, len);
		volk_32fc_x2_multiply_conjugate_32fc_a(d_mult1, h1, r0, len);
		volk_32f_x2_subtract_32f_a( (float*)out1, (float*)d_mult0, (float*)d_mult1, 2*len);

		// Do correct scaling!
		gr_complex divsqrt2 = gr_complex(1.0/std::sqrt(2),0);
		volk_32fc_s32fc_multiply_32fc_a(out0, out0, divsqrt2, len);
		volk_32fc_s32fc_multiply_32fc_a(out1, out1, divsqrt2, len);
	}

	void
	pre_decoder_vcvc_impl::combine_output(gr_complex* out,
										 gr_complex* out0,
										 gr_complex* out1,
										 int len)
	{
		memcpy(out, out0, sizeof(gr_complex) * len/2 );
		memcpy(out+len/2, out1, sizeof(gr_complex) * len/2 );
	}


	void
	pre_decoder_vcvc_impl::set_N_ant(int N_ant)
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
	pre_decoder_vcvc_impl::handle_msg(pmt::pmt_t msg)
	{
		//pmt::pmt_t msg_ant = pmt::pmt_cons(d_port_N_ant, pmt::pmt_from_long(long(d_state_info.N_ant) ) );
		//printf("is pair %s\n", pmt::pmt_is_pair(msg) ? "true" : "false");
		//pmt::pmt_t car = pmt::pmt_car(msg);
		pmt::pmt_t cdr = pmt::cdr(msg);
		//printf("pair car = %s\tcdr = %ld\n", pmt::pmt_symbol_to_string(car).c_str(), pmt::pmt_to_long(cdr) );
		set_N_ant(int( pmt::to_long(cdr) ));
	}

	void
	pre_decoder_vcvc_impl::set_decoding_style(std::string style)
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

	//gr_complex* d_h0, d_h1, d_r0, d_r1, d_out0, d_out1, d_mult0, d_mult1;
	void
	pre_decoder_vcvc_impl::setup_volk_vectors(int len)
	{
		d_h0 = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*d_vlen/2);
		d_h1 = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*d_vlen/2);
		d_r0 = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*d_vlen/2);
		d_r1 = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*d_vlen/2);

		d_out0 = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*d_vlen/2);
		d_out1 = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*d_vlen/2);

		d_mult0 = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*d_vlen/2);
		d_mult1 = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*d_vlen/2);
	}


  } /* namespace lte */
} /* namespace gr */

