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
lte_make_qpsk_soft_demod_vcvf (int vlen)
{
	return lte_qpsk_soft_demod_vcvf_sptr (new lte_qpsk_soft_demod_vcvf (vlen));
}


lte_qpsk_soft_demod_vcvf::lte_qpsk_soft_demod_vcvf (int vlen)
	: gr_sync_block ("qpsk_soft_demod_vcvf",
		gr_make_io_signature (1,1, sizeof(gr_complex)*vlen),
		gr_make_io_signature (1,1, sizeof (float)*(2*vlen))),
		d_vlen(vlen)
{
    d_demodulated = (float*)fftwf_malloc(sizeof(float)*2*d_vlen);
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

	for(int i = 0 ; i < noutput_items ; i++){
        memcpy(d_demodulated, (float*) in,  sizeof(float) * 2 * d_vlen);
        volk_32f_s32f_multiply_32f_u(d_demodulated, d_demodulated, d_SQRT2, (2*d_vlen));
        memcpy(out, d_demodulated, sizeof(float)*(2*d_vlen));
        in += d_vlen;
        out += (2*d_vlen);
	}

	// Tell runtime system how many output items we produced.
	return noutput_items;
}

