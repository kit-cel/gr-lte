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
#include <lte_pss_tagging_cc.h>
#include <cstdio>


lte_pss_tagging_cc_sptr
lte_make_pss_tagging_cc (int fftl)
{
	return lte_pss_tagging_cc_sptr (new lte_pss_tagging_cc (fftl));
}


lte_pss_tagging_cc::lte_pss_tagging_cc (int fftl)
	: gr_sync_block ("pss_tagging_cc",
		gr_make_io_signature (1,1, sizeof (gr_complex)),
		gr_make_io_signature (1,1, sizeof (gr_complex))),
		d_fftl(fftl),
        d_cpl(144*fftl/2048),
        d_cpl0(160*fftl/2048),
        d_slotl(7*fftl+6*d_cpl+d_cpl0),
        d_half_frame_start(0),
        d_N_id_2(-1),
        d_slot_num(0),
        d_is_locked(false)
{
    set_tag_propagation_policy(TPP_DONT);
    d_key=pmt::pmt_string_to_symbol("slot");
	d_tag_id=pmt::pmt_string_to_symbol(name() );
	d_id_key = pmt::pmt_string_to_symbol("N_id_2");
}


lte_pss_tagging_cc::~lte_pss_tagging_cc ()
{
}


int
lte_pss_tagging_cc::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
	const gr_complex *in = (const gr_complex *) input_items[0];
	gr_complex *out = (gr_complex *) output_items[0];
	
	memcpy(out,in,sizeof(gr_complex)*noutput_items);
	
	long nin = nitems_read(0);
	
	int half_framel = 10*d_slotl;
	int offset = d_half_frame_start%d_slotl;
	
	// as long as no data about offset and N_id_2 are available just copy from input to output!
	//if(d_N_id_2 < 0) {
	    //printf("%s dump data! half_frame_start and N_id_2 not yet set\n", name().c_str() );
	//    return noutput_items;
	//}

	
	for (int i = 0 ; i < noutput_items; i++){
        
        
        if( (nin+i)%d_slotl == offset){ // removed abs
            if((nin+i)%half_framel == d_half_frame_start){ // removed abs
                //printf("found half_frame_start\t num = %li\t0 < %li\n", nitems_read(0)+i,(nitems_read(0)+i-d_half_frame_start) );
                if(d_is_locked){
                    //printf("%s\thalf_frame_start = %i\tabs_pos = %ld\n", name().c_str(), d_half_frame_start, nitems_read(0)+i );
                    add_item_tag(0,nin+i,d_id_key, pmt::pmt_from_long(d_N_id_2),d_tag_id);
                    d_slot_num=0;
                }
            }
        
        
          
            //printf("%s\tslot_num = %i\tabs_pos = %ld\n",name().c_str(),d_slot_num,nitems_read(0)+i );  
            add_item_tag(0,nin+i,d_key, pmt::pmt_from_long(d_slot_num),d_tag_id);            
            d_slot_num = (d_slot_num+1)%10;
            
            if(i+d_slotl < noutput_items){
	            i += (d_slotl-1);
	        }
	        else{
	            i+=(noutput_items-i-1);
	        }
        }
        
    }
	

	
	// Tell runtime system how many output items we produced.
	return noutput_items;
}

