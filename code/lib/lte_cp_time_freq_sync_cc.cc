/* -*- c++ -*- */
/* 
 * Copyright 2012 Johannes Demel
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
#include <lte_cp_time_freq_sync_cc.h>
#include <cstdio>


lte_cp_time_freq_sync_cc_sptr
lte_make_cp_time_freq_sync_cc (int fftl)
{
	return lte_cp_time_freq_sync_cc_sptr (new lte_cp_time_freq_sync_cc (fftl));
}


lte_cp_time_freq_sync_cc::lte_cp_time_freq_sync_cc (int fftl)
	: gr_sync_block ("cp_time_freq_sync_cc",
		gr_make_io_signature (1,1, sizeof (gr_complex)),
		gr_make_io_signature (1,1, sizeof (gr_complex))),
		d_fftl(fftl),
		d_cpl(144*fftl/2048),
		d_cpl0(160*fftl/2048),
		d_slotl(7*fftl+6*d_cpl+d_cpl0),
		d_sym_pos(0),
		d_corr_val(0.0),
		d_work_call(0)
{
    d_key=pmt::pmt_string_to_symbol("symbol");
	d_tag_id=pmt::pmt_string_to_symbol(name() );
	
	d_cp0 = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*d_cpl0);
	d_cp1 = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*d_cpl0);
	d_res = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*d_cpl0);
}


lte_cp_time_freq_sync_cc::~lte_cp_time_freq_sync_cc ()
{
    fftwf_free(d_cp0);
    fftwf_free(d_cp1);
    fftwf_free(d_res);
}

// simple correlation between 2 arrays. returns complex value.
gr_complex
lte_cp_time_freq_sync_cc::corr(gr_complex *x,gr_complex *y, int len)
{
    gr_complex val = 0;
    for(int i = 0 ; i < len ; i++){
        val = val + (x[i]*conj(y[i]) );
    }
    return val;
}

gr_complex
lte_cp_time_freq_sync_cc::corr(gr_complex *res, gr_complex *x, gr_complex *y, int len)
{
    gr_complex val = 0;
    volk_32fc_conjugate_32fc_a(y, y, len);
    volk_32fc_x2_multiply_32fc_a(res, x, y, len);
    //printf("%s\tlen = %i\n", name().c_str(), len);
/*
    for(int i = 0; i < len; i++){
        val += *(res+i);
    }
*/
    int al_mult = volk_get_alignment() / sizeof(gr_complex) ;
    if(len%volk_get_alignment() != 0){
        int rest = len%volk_get_alignment();
        len -= len%volk_get_alignment();
        for(int i = len/2; i > al_mult ; i=i/2 ){
            volk_32f_x2_add_32f_a((float*)res, (float*)res, (float*)(res+i), i*2);
        }
        for(int i = 0; i < al_mult; i++){
            val += *(res+i);
        }
        for(int i = 0; i < rest; i++){
            val += *(res+len+i);
        }
    }
    else{
        for(int i = len/2; i > 3 ; i=i/2 ){
            volk_32f_x2_add_32f_a((float*)res, (float*)res, (float*)(res+i), i*2);
        }
        for(int i = 0; i < 4; i++){
            val += *(res+i);
        }
    }
    return val;
}

int
lte_cp_time_freq_sync_cc::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
	const gr_complex *in = (const gr_complex *) input_items[0];
	gr_complex *out = (gr_complex *) output_items[0];
	
	//printf("%s.work\tnoutput_items = %i\tnitems_read = %ld\n", name().c_str(), noutput_items, nitems_read(0) );
	
	if(nitems_read(0) > 100000){
	    add_item_tag(0,nitems_read(0)+5,d_key, pmt::pmt_from_long(d_sym_pos),d_tag_id);
        d_work_call++;
        memcpy(out,in,sizeof(gr_complex)*noutput_items );
        return noutput_items;
	}
		
	int stp = d_cpl0/4;	
	int nout = 0;
	
	//if(noutput_items < d_fftl+d_cpl){
    //    printf("%s\tnoutput_items = %i\tnitems_read = %ld\tnot enough items!\n",name().c_str(), noutput_items, nitems_read(0) );
    //}
	
	int coarse_pos = 0;
	gr_complex it_val = 0;
	for(int i = 0; i < noutput_items - (d_fftl+d_cpl+stp); i+=stp){
	    memcpy(d_cp0,in+i       ,sizeof(gr_complex)*d_cpl );
	    memcpy(d_cp1,in+i+d_fftl,sizeof(gr_complex)*d_cpl );
	    gr_complex val = corr(d_res,d_cp0,d_cp1,d_cpl);
	    if(abs(it_val) < abs(val) ){
	        coarse_pos = i;
	        if(d_corr_val < abs(val) ){
	            d_corr_val = abs(val);
	            d_sym_pos = (nitems_read(0) + coarse_pos)%d_slotl;
	            //printf("%s %i\tNEW coarse max!\tval = %f\tcoarse_pos = %i\t",name().c_str(), d_work_call, abs(val), coarse_pos );
	            //printf("corr_val = %f\tsym_pos = %ld\n", d_corr_val, d_sym_pos);
	        }
	        it_val = val;
	    }
	    //if(nitems_read(0) < 10000){
	    //    printf("%s\tcorr_val = %f\tnitems_read = %ld\n", name().c_str(), abs(val), nitems_read(0) );
	    //}
	    nout = i;
	}
	
	if(nout > 0){
	    if(coarse_pos < stp){
	        coarse_pos = stp;
	    }
	    int fine_pos = coarse_pos;
	    for(int i = coarse_pos-stp ; i < coarse_pos+stp; i++){
	        memcpy(d_cp0,in+i       ,sizeof(gr_complex)*d_cpl );
	        memcpy(d_cp1,in+i+d_fftl,sizeof(gr_complex)*d_cpl );
	        gr_complex val = corr(d_res,d_cp0,d_cp1,d_cpl);
	        
	        if(abs(it_val) < abs(val) ){
	            fine_pos = i;
	            if(d_corr_val < abs(val) ){
	                d_corr_val = abs(val);
	                long abs_pos = nitems_read(0) + fine_pos;
	                d_sym_pos = (nitems_read(0) + fine_pos)%d_slotl;
	                //printf("%s\tsym_pos = %ld\n",name().c_str(), d_sym_pos );
	                //printf("corr_val = %f\tsym_pos = %ld\tabs_pos = %ld\n", d_corr_val, d_sym_pos, abs_pos);
	            }
	        }
	    }
	}

    //The next if-statement exists for the sole purpose to avoid an infinity loop caused by the scheduler and not enough input items.
    if(nout == 0){       
        //printf("%s\tnout = %i\tnoutput_items = %i\tnitems_read = %ld\titems dumped!\n",name().c_str(), nout, noutput_items, nitems_read(0) );
        nout = noutput_items;
    }
    
    // actually the next block doesn't care about the exact tag position. Only the value and key are important.
    memcpy(out, in, sizeof(gr_complex)*nout );
    add_item_tag(0,nitems_read(0)+5,d_key, pmt::pmt_from_long(d_sym_pos),d_tag_id);
    d_work_call++;
	// Tell runtime system how many output items we produced.
	return nout;
}

