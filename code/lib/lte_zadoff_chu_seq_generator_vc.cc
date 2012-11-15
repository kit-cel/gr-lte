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
#include <lte_zadoff_chu_seq_generator_vc.h>


lte_zadoff_chu_seq_generator_vc_sptr
lte_make_zadoff_chu_seq_generator_vc (int len, int init_val)
{
	return lte_zadoff_chu_seq_generator_vc_sptr (new lte_zadoff_chu_seq_generator_vc (len, init_val));
}


lte_zadoff_chu_seq_generator_vc::lte_zadoff_chu_seq_generator_vc (int len, int init_val)
	: gr_sync_block ("zadoff_chu_seq_generator_vc",
		gr_make_io_signature (0, 0, 0),
		gr_make_io_signature (1,1, sizeof (gr_complex)*len)),
		d_len(len),
		d_init_val(init_val)
{
    d_seq = new gr_complex[len];
    
    float u = float(init_val);
    float val = 0.0;
    for(int n = 0; n < len; n++){
        d_seq[n]=exp(d_C_I* gr_complex(d_PI*u* float(-1*n    *(n+1))/float(len) ) );
        val = d_PI*u* (float(-1*n    *(n+1))/float(len) );
        
        d_seq[n] = gr_complex( cos( val ), sin( val ) );
        //printf("%+8.6f %+8.6f\texp(j*%f)\n",zc[n].real(),zc[n].imag(),d_PI*u* float(-1*n    *(n+1))/63.0);
    }

}


lte_zadoff_chu_seq_generator_vc::~lte_zadoff_chu_seq_generator_vc ()
{
    delete[] d_seq;
}

// define a constant value for PI!
const float
lte_zadoff_chu_seq_generator_vc::d_PI = float(3.14159265358979323846);

// define a constant for imaginary unit.
const gr_complex
lte_zadoff_chu_seq_generator_vc::d_C_I = gr_complex(0,1);


int
lte_zadoff_chu_seq_generator_vc::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
	//const float *in = (const float *) input_items[0];
	gr_complex *out = (gr_complex *) output_items[0];
	
	int sizeof_gr_complex = sizeof(gr_complex);
    for (int i = 0 ; i < noutput_items; i++){
        memcpy(out,d_seq,sizeof_gr_complex*d_len);
        out += d_len;
    }

	// Tell runtime system how many output items we produced.
	return noutput_items;
}

