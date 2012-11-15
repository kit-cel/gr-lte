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

lte_pre_decoder_vcvc_sptr
lte_make_pre_decoder_vcvc (int N_ant,std::string style)
{
	return lte_pre_decoder_vcvc_sptr (new lte_pre_decoder_vcvc (N_ant, style));
}


lte_pre_decoder_vcvc::lte_pre_decoder_vcvc (int N_ant,std::string style)
	: gr_sync_block ("pre_decoder_vcvc",
		gr_make_io_signature (2,3, sizeof (gr_complex)*240 ),
		gr_make_io_signature (1,1, sizeof (gr_complex)*240 )),
		d_N_ant(N_ant),
		d_style(style)
{
    if(d_N_ant != 1 && d_N_ant != 2 && d_N_ant != 4){
        printf("%i antenna ports are not supported/invalid\n", d_N_ant);
    }
    if(style != "tx_diversity"){
        if (style == "spatial_multiplexing"){
            printf("%s decoding style is valid but not supported\n", style.c_str() );
        }
        else{
            printf("%s decoding style is invalid\n", style.c_str() );
        }
    }
    printf("%i antenna ports are used\t%s decoding style is used\n",d_N_ant, style.c_str() );
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

	    for(int i = 0 ; i < 240 ; i++ ){
	        *(out+i) = frame[i]/ce1[i];
	    }
	}
	else if(N_ant == 2){
	    const gr_complex *in3 = (const gr_complex *) input_items[2];
	    gr_complex ce2[240];
        memcpy(ce2,in3,240*sizeof(gr_complex) );
        for (int i = 0 ; i < 120 ; i++ ){
            //printf();
            float ce_n = pow(ce1[2*i].real(),2) + pow(ce1[2*i].imag(),2) + pow(ce2[2*i].real(),2) + pow(ce2[2*i].imag(),2);
            *(out+2*i  ) = ( std::conj(ce1[2*i]) * frame[2*i] + ce2[2*i+1] * std::conj(frame[2*i+1]) ) / ce_n;
            *(out+2*i+1) = std::conj( gr_complex(-1) * std::conj(ce2[2*i]) * frame[2*i] + ce1[2*i] * std::conj(frame[2*i+1]) ) / ce_n;
        }
	    
	}

	// Do <+signal processing+>

	// Tell runtime system how many output items we produced.
	return 1;
}

