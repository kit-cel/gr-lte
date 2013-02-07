/* -*- c++ -*- */
/*
 * Copyright 2012 Communications Engineering Lab (CEL) / Karlsruhe Institute of Technology (KIT)
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

#include <gr_io_signature.h>
#include <lte_pre_decoder_vcvc.h>
#include <cstdio>
#include <cmath>
#include <fftw3.h>
#include <volk/volk.h>

lte_pre_decoder_vcvc_sptr
lte_make_pre_decoder_vcvc (int N_ant,std::string style)
{
	return lte_pre_decoder_vcvc_sptr (new lte_pre_decoder_vcvc (N_ant, style));
}


lte_pre_decoder_vcvc::lte_pre_decoder_vcvc (int N_ant,std::string style)
	: gr_sync_block ("pre_decoder_vcvc",
		gr_make_io_signature (2,3, sizeof (gr_complex)*240 ),
		gr_make_io_signature (1,1, sizeof (gr_complex)*240 ))
{
    set_N_ant(N_ant);
    set_decoding_style(style);
}


lte_pre_decoder_vcvc::~lte_pre_decoder_vcvc ()
{
}


int
lte_pre_decoder_vcvc::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
	const gr_complex *in1 = (const gr_complex *) input_items[0];
    const gr_complex *in2 = (const gr_complex *) input_items[1];
	gr_complex *out = (gr_complex *) output_items[0];

	//get local copy of number of antenna ports
	int N_ant = d_N_ant;
	//get local copy of input 1 (received values)
	gr_complex frame[240];
	memcpy(frame,in1,240*sizeof(gr_complex) );
	//get local copy of input 2 (calculated CE values for antenna port 1)
	gr_complex ce1[240];
    memcpy(ce1,in2,240*sizeof(gr_complex) );
    // since antenna port 2 is an optional input, it is not yet copied.


	if (N_ant == 1){
	    decode_1_ant(out, frame, ce1, 240);
	}
	else if(N_ant == 2){
	    const gr_complex *in3 = (const gr_complex *) input_items[2];
	    gr_complex ce2[240];
        memcpy(ce2,in3,240*sizeof(gr_complex) );

        int len = 240;
        gr_complex* h0 = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*len/2);
        gr_complex* h1 = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*len/2);
        gr_complex* r0 = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*len/2);
        gr_complex* r1 = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*len/2);
        prepare_2_ant_vectors(h0, h1, r0, r1, frame, ce1, ce2, len);

        gr_complex* out0 = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*len/2);
        gr_complex* out1 = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*len/2);

        decode_2_ant(out0, out1, h0, h1,r0, r1, len/2);
        combine_output(out, out0, out1, len);
	}

	// Tell runtime system how many output items we produced.
	return 1;
}

inline void
lte_pre_decoder_vcvc::decode_1_ant(gr_complex* out,
                                   gr_complex* rx,
                                   gr_complex* h,
                                   int len)
{
    for(int i = 0 ; i < len ; i++ ){
        *(out+i) = rx[i]/h[i];
    }
}

inline void
lte_pre_decoder_vcvc::prepare_2_ant_vectors(gr_complex* h0,
                                            gr_complex* h1,
                                            gr_complex* r0,
                                            gr_complex* r1,
                                            gr_complex* rx,
                                            gr_complex* ce1,
                                            gr_complex* ce2,
                                            int len)
{
    for(int n = 0; n < len/2; n++){
        h0[n] = ce1[2*n];
        h1[n] = ce2[2*n];
        r0[n] = rx[2*n];
        r1[n] = rx[2*n+1];
    }
}

inline void
lte_pre_decoder_vcvc::decode_2_ant(gr_complex* out0,
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
    gr_complex* mult0 = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*len);
    gr_complex* mult1 = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*len);

    // e_x0
    volk_32fc_x2_multiply_conjugate_32fc_a(mult0, r0, h0, len);
    volk_32fc_x2_multiply_conjugate_32fc_a(mult1, h1, r1, len);
    volk_32f_x2_add_32f_a( (float*)out0, (float*)mult0, (float*)mult1, 2*len);

    //e_x1
    volk_32fc_x2_multiply_conjugate_32fc_a(mult0, r1, h0, len);
    volk_32fc_x2_multiply_conjugate_32fc_a(mult1, h1, r0, len);
    volk_32f_x2_subtract_32f_a( (float*)out1, (float*)mult0, (float*)mult1, 2*len);

    // Do correct scaling!
    gr_complex divsqrt2 = gr_complex(1.0/std::sqrt(2),0);
    volk_32fc_s32fc_multiply_32fc_a(out0, out0, divsqrt2, len);
    volk_32fc_s32fc_multiply_32fc_a(out1, out1, divsqrt2, len);
}

inline void
lte_pre_decoder_vcvc::combine_output(gr_complex* out,
                                     gr_complex* out0,
                                     gr_complex* out1,
                                     int len)
{
    for(int n = 0; n < len/2 ; n++){
        *(out+2*n  ) = out0[n];
        *(out+2*n+1) = out1[n];
    }
}


void
lte_pre_decoder_vcvc::set_N_ant(int N_ant)
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
lte_pre_decoder_vcvc::set_decoding_style(std::string style)
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




