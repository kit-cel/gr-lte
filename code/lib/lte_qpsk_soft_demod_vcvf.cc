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

	float* demodulated = (float*)fftwf_malloc(sizeof(float)*480*noutput_items);
    for (int i = 0 ; i < 240*noutput_items ; i++ ) {
	    *(demodulated+2*i  ) = (*(in+i)).real();
	    *(demodulated+2*i+1) = (*(in+i)).imag();
	}
	const float SQRT2 = std::sqrt(2);
    volk_32f_s32f_multiply_32f_u(demodulated, demodulated, SQRT2, 480*noutput_items);
    memcpy(out, demodulated, sizeof(float)*480*noutput_items);

	// Tell runtime system how many output items we produced.
	return noutput_items;
}

