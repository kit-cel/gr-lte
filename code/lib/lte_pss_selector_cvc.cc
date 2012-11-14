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
#include <lte_pss_selector_cvc.h>
#include <cstdio>


lte_pss_selector_cvc_sptr
lte_make_pss_selector_cvc (int fftl)
{
	return lte_pss_selector_cvc_sptr (new lte_pss_selector_cvc (fftl));
}


lte_pss_selector_cvc::lte_pss_selector_cvc (int fftl)
	: gr_block ("pss_selector_cvc",
		gr_make_io_signature (1,1, sizeof (gr_complex)),
		gr_make_io_signature (1,1, sizeof (gr_complex)*fftl)),
		d_fftl(fftl),
		d_cpl(144*fftl/2048),
		d_cpl0(160*fftl/2048),
		d_syml(fftl+d_cpl),
		d_syml0(fftl+d_cpl0),
		d_slotl(7*fftl+6*d_cpl+d_cpl0),
		d_sym_count(0),
		d_offset(0),
		d_sym_pos(0),
		d_ass_half_frame_start(40*d_slotl),
		d_off_sym_count(0),
		d_work_call(0),
		d_is_locked(false)
{
    set_tag_propagation_policy(TPP_DONT);
    d_key=pmt::pmt_string_to_symbol("offset_marker");
    d_sym_key=pmt::pmt_string_to_symbol("symbol");
	d_tag_id=pmt::pmt_string_to_symbol(name() );
}


lte_pss_selector_cvc::~lte_pss_selector_cvc ()
{
}

void
lte_pss_selector_cvc::forecast (int noutput_items, gr_vector_int &ninput_items_required)
{
  unsigned ninputs = ninput_items_required.size ();
  for (unsigned i = 0; i < ninputs; i++)
    ninput_items_required[i] = 2*d_syml0 * noutput_items + history() - 1;
}

void
lte_pss_selector_cvc::set_sym_pos(int offset, long nread)
{
    d_sym_pos = offset;
    while(nread+d_slotl > d_sym_pos){
        d_sym_pos += d_slotl;
    }
    if(d_sym_count > 0){
        d_sym_pos +=d_syml0;
    }
    for(int i = 0 ; i < 6 ; i++){
        if(d_sym_pos+d_syml < nread){
            d_sym_pos += d_syml;
        }
    }
}


int
lte_pss_selector_cvc::general_work (int noutput_items,
			       gr_vector_int &ninput_items,
			       gr_vector_const_void_star &input_items,
			       gr_vector_void_star &output_items)
{
    const gr_complex *in = (const gr_complex *) input_items[0];
    gr_complex *out = (gr_complex *) output_items[0];

    d_work_call++;
    int nin = ninput_items[0];
    long nir = nitems_read(0);
    if(d_is_locked){
        consume_each(nin);
        return 0;
    }
    
    //printf("%s\twork_call = %i\tnin = %i\tnoutput_items = %i\tnitems_read = %ld\n", name().c_str(), d_work_call, nin, noutput_items, nir );

    
    std::vector <gr_tag_t> v_off;
    get_tags_in_range(v_off,0,nir,nir+nin,d_sym_key);
    if (v_off.size() > 0){
        int offset = pmt::pmt_to_long(v_off[0].value);
        if(offset != d_offset){
            //printf("%s\toffset = %i changed\tabs_pos = %ld\n", name().c_str(), offset, nitems_read(0) );
            d_offset = offset;
        }
    }
    int offset = d_offset;
    
    if(offset == 0){
        consume_each(nin);
        return 0;
    }
    
   
    
    // The following few lines within if are just for debugging.
    if(nin < d_syml0){
        printf("nin < d_syml0\tnin = %i\tnoutput_items = %i\tNOT ENOUGH INPUT!\n",nin,noutput_items);
        consume_each(nin);
        return 0;
    }
    
    
    int consumed_items = 0;
    int nout = 0;
    long last_pos = nir;
    long pss_pos = (d_ass_half_frame_start+(6*d_fftl+5*d_cpl+d_cpl0) )%(10*d_slotl);
    //printf("%s\tnoutput_items = %i\n", name().c_str(), noutput_items);
    for(int i = 0 ; i+d_syml0 < nin ; i++){
        long abs_pos = nir+long(i);
        int mod_pss = abs( (int((abs_pos)-pss_pos))%(10*d_slotl) );
        if(d_ass_half_frame_start < 10* d_slotl && mod_pss < 10 ){
            memcpy(out,in+i+d_cpl-4,sizeof(gr_complex)*d_fftl);
            add_item_tag(0,nitems_written(0)+nout,d_key, pmt::pmt_from_long( abs_pos-4 ),d_tag_id);
            out+=d_fftl;

            consumed_items = i+1;
            nout++;
        
            
            //printf("%s\twork_call = %i\tabs_pos = %ld\tmod = %i\thalf_frame_start = %ld\n", name().c_str(), d_work_call, abs_pos-(6*d_fftl+5*d_cpl+d_cpl0)-4, mod_pss, d_ass_half_frame_start );
            last_pos = abs_pos;
            
        }
        
        else if ( (abs_pos)%d_slotl == offset){
            
            
            
            
            //printf("%s\twork_call = %i\tsyml%i\tnin = %i\tnout = %i\tabs_pos = %ld\tdiff = %ld\n", name().c_str(), d_work_call, d_sym_count, nin, nout, abs_pos, abs_pos-last_pos);
            last_pos = abs_pos;
        }
        //else if( ((abs(nitems_read(0)+i-offset)%d_slotl)-d_syml0)%d_syml == 0 ){
        else if( (((abs(abs_pos-offset))%d_slotl)-d_syml0)%d_syml == 0 ){
            memcpy(out,in+i+d_cpl,sizeof(gr_complex)*d_fftl);
            add_item_tag(0,nitems_written(0)+nout,d_key, pmt::pmt_from_long( abs_pos ),d_tag_id);
            out+=d_fftl;
            i += (d_syml-1);
            consumed_items = i+1;
            nout++;
            d_sym_count++;
            
            
            //printf("%s\twork_call = %i\tsyml%i\tnin = %i\tnout = %i\tabs_pos = %ld\tdiff = %ld\n", name().c_str(), d_work_call, d_sym_count, nin, nout, abs_pos, abs_pos-last_pos);
            last_pos = abs_pos;
        }
        if(nout == noutput_items){break;}
    }

    if(nout == 0 && nin < 2*d_syml0){
        printf("dropping items!\tabs_pos = %ld\n",nir );
        consumed_items = nin;
    }
    
    
    
  
    //printf("%s\tconsumed_items = %i\tnout = %i\n", name().c_str(), consumed_items, nout);    
    // Tell runtime system how many input items we consumed on
    // each input stream.
    consume_each (consumed_items);

    // Tell runtime system how many output items we produced.
    return nout;
}

