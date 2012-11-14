/* -*- c++ -*- */
/* 
 * Copyright 2012 <+YOU OR YOUR COMPANY+>.
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
#include <lte_tag_symbol_cc.h>
#include <cstdio>


lte_tag_symbol_cc_sptr
lte_make_tag_symbol_cc (int offset, int fftl)
{
	return lte_tag_symbol_cc_sptr (new lte_tag_symbol_cc (offset,fftl));
}


lte_tag_symbol_cc::lte_tag_symbol_cc (int offset, int fftl)
	: gr_sync_block ("tag_symbol_cc",
		gr_make_io_signature (1,1, sizeof (gr_complex)),
		gr_make_io_signature (1,1, sizeof (gr_complex))),
		d_offset(offset),
		d_fftl(fftl)
		
{
    int cpl = (144*fftl)/2048;
    int cpl0 = (160*fftl)/2048;
    int slotl = 7*fftl+6*cpl+cpl0;
    d_slot_num = (20-int(floor(offset/slotl)))%20;
    d_key=pmt::pmt_string_to_symbol("symbol");
	d_tag_id=pmt::pmt_string_to_symbol(name() );
	d_id_key = pmt::pmt_string_to_symbol("N_id_2");
}


lte_tag_symbol_cc::~lte_tag_symbol_cc ()
{
}


int
lte_tag_symbol_cc::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
	const gr_complex *in = (const gr_complex *) input_items[0];
	gr_complex *out = (gr_complex *) output_items[0];
	
	if(nitems_written(0) != nitems_read(0) ){
	    printf("nitems_written(0) != nitems_read(0)");
	}
	
	//nitems_read(0);
    int fftl = d_fftl;
    int cpl = (144*fftl)/2048;
    int cpl0 = (160*fftl)/2048;
    int slotl = 7*fftl+6*cpl+cpl0;
    int framel = 20*slotl;

    for (int i = 0 ; i < noutput_items; i++){
        if(abs(nitems_read(0)+i-d_offset)%framel == 0){
            //printf("found frame_start\t num = %li\t0 < %li\t", nitems_read(0)+i,(nitems_read(0)+i-d_offset) );
            d_slot_num=0;
        }
        if( abs(nitems_read(0)+i-d_offset)%slotl == 0){
/*            if ( abs(nitems_read(0)+i-d_offset) < 60000){
                printf("%s found slot_start\t num = %li\tslot_num = %i\t0 < %li\n",name().c_str(), nitems_read(0)+i,d_slot_num, nitems_read(0)+i-d_offset);
            }
*/          
            //printf("%s\toffset_value = %i\tabs_pos = %ld\n", name().c_str(), d_offset, nitems_read(0)+i );  
            add_item_tag(0,nitems_read(0)+i,d_key, pmt::pmt_from_long(d_offset),d_tag_id);
            //if (d_slot_num == 0){
            //    add_item_tag(0,nitems_read(0)+i,d_id_key, pmt::pmt_from_long(1),d_tag_id);// for test purposes only a hardcoded value!
            //}
            
            //d_slot_num = (d_slot_num+1)%10;
        }
        
    }
    
    memcpy(out,in,sizeof(gr_complex)*noutput_items );

	// Tell runtime system how many output items we produced.
	return noutput_items;
}

