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
#include <lte_descrambling_vfvf.h>

#include <cstdio>
#include <fstream>

lte_descrambling_vfvf_sptr
lte_make_descrambling_vfvf ()
{
	return lte_descrambling_vfvf_sptr (new lte_descrambling_vfvf ());
}


lte_descrambling_vfvf::lte_descrambling_vfvf ()
	: gr_sync_interpolator ("descrambling_vfvf",
		gr_make_io_signature (1, 1, sizeof (float)*480),
		gr_make_io_signature (1, 1, sizeof (float)*120), 32),
		d_cell_id(-1),
		d_work_call(0)
{
    // set PMT blob info	
	d_key=pmt::pmt_string_to_symbol("descr_part");
	d_tag_id=pmt::pmt_string_to_symbol(name() );	
}


lte_descrambling_vfvf::~lte_descrambling_vfvf ()
{
    delete[]d_pn_seq;
}

// WORK function. 1 input vector generates 32 output vectors.
int
lte_descrambling_vfvf::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
	const float *in = (const float *) input_items[0];
	float *out = (float *) output_items[0];
	
	// If Cell ID is not set, do not process anything!
	if(d_cell_id < 0){
	    return 0;
	}
	
	d_work_call++;

	// Read in vector and make copies for a 1920-element vector.
	float scr[d_pn_seq_len];
	int len_in=d_pn_seq_len/4;

	memcpy(scr         ,in,sizeof(float)*len_in );
	memcpy(scr+len_in  ,in,sizeof(float)*len_in );
	memcpy(scr+2*len_in,in,sizeof(float)*len_in );
	memcpy(scr+3*len_in,in,sizeof(float)*len_in );

	// Calculate actual descrambled sequence
	// Write output to
	for (int i = 0 ; i < d_pn_seq_len; i++){
		scr[i]=scr[i]*d_pn_seq[i];
		//value=i;
	}
	memcpy(out+1920,scr,sizeof(float)*d_pn_seq_len);
	//Do Soft combining
	for (int i = 0 ; i < 120 ; i++){
		scr[i      ]=(scr[i]+scr[i+120]+scr[i+240]+scr[i+360])/4;
		scr[i+480  ]=(scr[i+480]+scr[i+120+480]+scr[i+240+480]+scr[i+360+480])/4;
		scr[i+480*2]=(scr[i+480*2]+scr[i+120+480*2]+scr[i+240+480*2]+scr[i+360+480*2])/4;
		scr[i+480*3]=(scr[i+480*3]+scr[i+120+480*3]+scr[i+240+480*3]+scr[i+360+480*3])/4;
	}
	
	// Write to out.
	for (int i = 0 ; i < 4 ; i++ ){
		memcpy(out+120*i      ,scr+0    ,sizeof(float) *120);
		memcpy(out+120*i+480  ,scr+480  ,sizeof(float) *120);
		memcpy(out+120*i+480*2,scr+480*2,sizeof(float) *120);
		memcpy(out+120*i+480*3,scr+480*3,sizeof(float) *120);
	}
	
	for (int i = 0 ; i < 32 ; i++){
		add_item_tag(0,nitems_written(0)+i,d_key, pmt::pmt_from_long(i),d_tag_id);
	}
    	
	// Tell runtime system how many output items we produced.
	return 32; // noutput_items;
}


char*
lte_descrambling_vfvf::pn_seq_generator(int len, int cell_id)
{	
	const int Nc=1600; //Constant is defined in standard
	int cinit=cell_id;
	
	char x2[3*len+Nc];
	for (int i = 0; i<31; i++){
	    	char val = cinit%2;
    		cinit = floor(cinit/2);
    		x2[i] = val;	
	}

	char *c=new char[len];
	char x1[3*len+Nc];
	// initialization for first 35 elements is needed. (Otherwise READ BEFORE WRITE ERROR)
	for (int i = 0 ; i < 35 ; i++){
		x1[i]=0;
	}
	x1[0]=1;

	for (int n=0; n <2*len+Nc-3;n++){
		x1[n+31]=(x1[n+3]+x1[n])%2;
		x2[n+31]=(x2[n+3]+x2[n+2]+x2[n+1]+x2[n])%2;
	}

	for (int n=0;n<len;n++){
		c[n]=(x1[n+Nc]+x2[n+Nc])%2;
	}

	return c;
}


void
lte_descrambling_vfvf::set_cell_id(int id)
{
    d_cell_id = id;
    printf("%s\tset_cell_id = %i\n", name().c_str(), d_cell_id);
    int len=1920;
	d_pn_seq_len=len;
	char *pn_seq0 = pn_seq_generator(len,d_cell_id);
	d_pn_seq = new char[len];
	//NRZ coding of pn sequence
	for (int i = 0 ; i<len; i++){
		d_pn_seq[i]=1-2*pn_seq0[i];
	}
}







