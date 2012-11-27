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
#include <lte_qpsk_soft_demod_vcvf.h>
#include <cstdio>


lte_qpsk_soft_demod_vcvf_sptr
lte_make_qpsk_soft_demod_vcvf ()
{
	return lte_qpsk_soft_demod_vcvf_sptr (new lte_qpsk_soft_demod_vcvf ());
}


lte_qpsk_soft_demod_vcvf::lte_qpsk_soft_demod_vcvf ()
	: gr_sync_block ("qpsk_soft_demod_vcvf",
		gr_make_io_signature (1,1, sizeof(gr_complex)*240),
		gr_make_io_signature (1,1, sizeof (float)*480))
{
    //printf("%s\tsizeof(gr_complex) = %ld\n", name().c_str(), sizeof(gr_complex));

}


lte_qpsk_soft_demod_vcvf::~lte_qpsk_soft_demod_vcvf ()
{
}


int
lte_qpsk_soft_demod_vcvf::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
	const gr_complex *in = (const gr_complex *) input_items[0];
	float *out = (float *) output_items[0];
	
	for (int c = 0; c < noutput_items ; c++){
		for (int i = 0 ; i < 240 ; i++ ) {
	        *(out+2*i  ) = (*(in+i)).real();
	        *(out+2*i+1) = (*(in+i)).imag();
	    }
	    out += 480;
	    in  += 240;
	}


	// Tell runtime system how many output items we produced.
	return 1;
}

