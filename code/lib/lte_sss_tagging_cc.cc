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
#include <lte_sss_tagging_cc.h>
#include <cstdio>


lte_sss_tagging_cc_sptr
lte_make_sss_tagging_cc (int fftl)
{
	return lte_sss_tagging_cc_sptr (new lte_sss_tagging_cc (fftl));
}


lte_sss_tagging_cc::lte_sss_tagging_cc (int fftl)
	: gr_sync_block ("sss_tagging_cc",
		gr_make_io_signature (1,1, sizeof (gr_complex) ),
		gr_make_io_signature (1,1, sizeof (gr_complex) )),
		d_fftl(fftl),
		d_cpl(144*fftl/2048),
		d_cpl0(160*fftl/2048),
		d_slotl(7*fftl+6*d_cpl+d_cpl0),
		d_framel(20*d_slotl),
		d_offset_0(0),
		d_frame_start(0),
		d_slot_num(41)

{
    set_tag_propagation_policy(TPP_DONT);
    d_key=pmt::pmt_string_to_symbol("slot");
	d_tag_id=pmt::pmt_string_to_symbol(name() );
}


lte_sss_tagging_cc::~lte_sss_tagging_cc ()
{
}



int
lte_sss_tagging_cc::work (int noutput_items,
			       gr_vector_const_void_star &input_items,
			       gr_vector_void_star &output_items)
{
    const gr_complex *in = (const gr_complex *) input_items[0];
    gr_complex *out = (gr_complex *) output_items[0];


    //This block does not change data. It just adds new itemtags!
    memcpy(out,in,sizeof(gr_complex)*noutput_items);

    long nin = nitems_read(0);


    std::vector <gr_tag_t> v;
    get_tags_in_range(v,0,nin,nin+noutput_items);
    if (v.size() > 0){
        //printf("\n\n\n%s tag: found\tvalue = %ld\toffset = %ld\n",name().c_str(),half_frame_slot,v[0].offset );
        long offset_mod = (v[0].offset)%d_slotl;
        if(offset_mod != d_offset_0){
            d_offset_0 = offset_mod;
            //printf("%s\toffset = %ld changed\tabs_pos = %ld\n", name().c_str(), d_offset_0, v[0].offset);

        }
	}

	// as long as frame start is unknown add dummy tags, so freq estimate can work!
	if(d_frame_start == 0){
		for (int i = 0 ; i < noutput_items ; i++){
	        if( (nin+i)%d_slotl == d_offset_0 ){ //removed abs
	            //printf("%s\tslot_num = %i\tabs_pos = %ld\tframe_start = %ld\n", name().c_str() ,d_slot_num, nitems_read(0)+i ,d_frame_start);
	            add_item_tag(0,nin+i,d_key, pmt::pmt_from_long( d_slot_num ),d_tag_id);
	        }
	    }
        return noutput_items;
    } //wait till first value found!


	//This loop adds new tags to the stream.
	for (int i = 0 ; i < noutput_items ; i++){
	    if( (nin+i)%d_slotl == d_offset_0 ){ //removed abs
	        if((nin+i)%d_framel == d_frame_start ){ // removed abs
	            //printf("%s\toffset = %ld\tframe_start = %ld\tabs_pos = %ld\n", name().c_str(), d_offset_0, d_frame_start, nitems_read(0)+i);
	            d_slot_num = 0;
	        }

	        //printf("%s\tslot_num = %i\tabs_pos = %ld\tframe_start = %ld\n", name().c_str() ,d_slot_num, nitems_read(0)+i ,d_frame_start);
	        add_item_tag(0,nin+i,d_key, pmt::pmt_from_long( d_slot_num ),d_tag_id);
	        if(i+d_slotl < noutput_items){
	            i += (d_slotl-1);
	        }
	        else{
	            i+=(noutput_items-i-1);
	        }

	        // prepare values for next iteration.
	        d_slot_num++;

	    }
	}

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

void
lte_sss_tagging_cc::set_frame_start(long frame_start)
{
    d_frame_start = frame_start;
}














