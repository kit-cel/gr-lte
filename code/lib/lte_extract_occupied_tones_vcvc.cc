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
#include <lte_extract_occupied_tones_vcvc.h>
#include <cstdio>


lte_extract_occupied_tones_vcvc_sptr
lte_make_extract_occupied_tones_vcvc (int N_rb_dl, int fftl)
{
	return lte_extract_occupied_tones_vcvc_sptr (new lte_extract_occupied_tones_vcvc (N_rb_dl, fftl));
}


lte_extract_occupied_tones_vcvc::lte_extract_occupied_tones_vcvc (int N_rb_dl, int fftl)
	: gr_sync_block ("extract_occupied_tones_vcvc",
		gr_make_io_signature (1,1, sizeof (gr_complex)*fftl),
		gr_make_io_signature (1,1, sizeof (gr_complex)*12*N_rb_dl)),
		d_N_rb_dl(N_rb_dl),
		d_fftl(fftl)
{
}


lte_extract_occupied_tones_vcvc::~lte_extract_occupied_tones_vcvc ()
{
}


int
lte_extract_occupied_tones_vcvc::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
	const gr_complex *in = (const gr_complex *) input_items[0];
	gr_complex *out = (gr_complex *) output_items[0];
    
    int N_rb_dl = d_N_rb_dl;
    int fftl = d_fftl;
    int h_byte_tones_num = sizeof(gr_complex)*6*N_rb_dl;
    
    for (int i = 0; i < noutput_items; i ++){
        // In this block DC carrier will be removed.
        //gr_complex vec_f[fftl];
        //memcpy(vec_f,in,sizeof(gr_complex)*fftl );
        //gr_complex vec_d[12*N_rb_dl];
        // first half of data samples at the end of in vector second half at the beginning
        
        //memcpy(vec_d,&vec_f[fftl-(6*N_rb_dl)], h_byte_tones_num );
        //memcpy(vec_d,vec_f+(fftl-(6*N_rb_dl)), h_byte_tones_num );
        memcpy(out,in+(fftl-(6*N_rb_dl)),h_byte_tones_num );
        
        //memcpy(&vec_d[6*N_rb_dl],&vec_f[1], h_byte_tones_num );
        //memcpy(vec_d+(6*N_rb_dl),vec_f+1, h_byte_tones_num );
        memcpy(out+(6*N_rb_dl),in+1,h_byte_tones_num);
        
        
        //memcpy(out,vec_d, 2*h_byte_tones_num );
        
        in  += fftl;
        out += 12*N_rb_dl;
    
    }


	// Tell runtime system how many output items we produced.
	return noutput_items;
}

