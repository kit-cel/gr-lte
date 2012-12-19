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
#include <lte_remove_cp2_cvc.h>


lte_remove_cp2_cvc_sptr
lte_make_remove_cp2_cvc (int fftl)
{
     return lte_remove_cp2_cvc_sptr (new lte_remove_cp2_cvc (fftl));
}


lte_remove_cp2_cvc::lte_remove_cp2_cvc (int fftl)
     : gr_block ("remove_cp2_cvc",
                 gr_make_io_signature (1, 1, sizeof(gr_complex)      ),
                 gr_make_io_signature (1, 1, sizeof(gr_complex)*fftl )),
     d_fftl(fftl),
     d_cpl((144*fftl)/2048),
     d_cpl0((160*fftl)/2048),
     d_slotl(7*fftl+6*d_cpl+d_cpl0),
     d_symb_num(0),
     d_slot_num(0),
     d_half_frame_start(0),
     d_work_call(0),
     d_N_id_2(-1),
     d_next_symbol(0)
{
     d_key=pmt::pmt_string_to_symbol("OFDM_symbol");
     d_id_key = pmt::pmt_string_to_symbol("N_id_2");
     d_tag_id=pmt::pmt_string_to_symbol(name() );
     set_tag_propagation_policy(TPP_DONT);

     d_ofdm_symbol_count = 0;
     d_next_symb_pos = 0;
}


lte_remove_cp2_cvc::~lte_remove_cp2_cvc ()
{
}


int
lte_remove_cp2_cvc::general_work (int noutput_items,
                                  gr_vector_int &ninput_items,
                                  gr_vector_const_void_star &input_items,
                                  gr_vector_void_star &output_items)
{
    const gr_complex *in = (const gr_complex *) input_items[0];
    gr_complex *out = (gr_complex *) output_items[0];

    d_work_call++;

    // some needed variables
    long nir = nitems_read(0);
    int nin = ninput_items[0];

    // This part is to extract the N_id_2 of the preceding PSS blocks.
    // N_id_2 is needed by SSS blocks to calculate Cell ID!
    std::vector <gr_tag_t> v_id;
    get_tags_in_range(v_id, 0, nir, nir+noutput_items*(d_fftl+d_cpl0), d_id_key );
    for(int i = 0; i < v_id.size(); i++) {
        int N_id_2 = int( pmt::pmt_to_long(v_id[i].value) ); // get tag value , contains N_id_2 value
        if(N_id_2 != d_N_id_2){
            printf("%s\tNEW ID_TAG_2 = %i\n", name().c_str(), N_id_2);
            d_N_id_2 = N_id_2;
        }
    }
    if(d_N_id_2 < 0){
        consume_each(nin);
        return noutput_items;
    }

    // First thing to do get tags and calculate offset and half_frame_start!
    std::vector <gr_tag_t> v;
    get_tags_in_range(v, 0, nir, nir+noutput_items*(d_fftl+d_cpl0), pmt::pmt_string_to_symbol("slot") );
    for(int i = 0; i < v.size(); i++) {
        long offset = v[i].offset; // get position of tag
        long slot_num = pmt::pmt_to_long(v[i].value); // get tag value (represents slot number in a half frame)
        //long half_frame_start = (offset-(d_slotl*slot_num) )%(10*d_slotl); // calculate half_frame_start
        long half_frame_start = (offset+(d_slotl*(10-slot_num) ))%(10*d_slotl); // calculate half_frame_start
        if(half_frame_start != d_half_frame_start){
            printf("%s\thf_start = %ld\tslot_num = %ld\tabs_pos = %ld\n", name().c_str(), half_frame_start, slot_num, offset );
        }
        d_half_frame_start = half_frame_start;
    }

    // Produce output find beginning of each vector and extract it!
    long slot_start = d_half_frame_start%d_slotl;
    long symbol_start = (slot_start+(d_fftl+d_cpl0))%(d_fftl+d_cpl);
    long mod_pos = 0;
    int consumed_items = 0;
    int nout = 0;
    int f_sym = 0;
    for(int i = 0 ; i < nin-(d_fftl+d_cpl0) ; i++){
        mod_pos = (nir+i)%(10*d_slotl); // mod_pos represents current offset relative to first half_frame
        //if(d_next_symb_pos != nir+i){
        //    printf("%s\tnext_symb = %ld\tnir = %ld\ti = %i\n", name().c_str(), nir+i, nir, i);
        //}
        if( mod_pos == d_half_frame_start){ // found start of new half frame
            d_slot_num = 0;
            d_symb_num = 0;
            f_sym = 7 * d_slot_num + d_symb_num;
            d_next_symbol = nir+i+d_fftl+d_cpl0;
            //printf("%s\toutput hf_start = %ld\tlast = %ld\tframe_symbol = %i\n", name().c_str(), nir+i, nir+i-d_slotl, f_sym);
            produce_output(out, in+i+d_cpl0, f_sym, nout);
            consumed_items += (d_fftl+d_cpl0);
            //i+=(d_fftl+d_cpl);
            d_next_symb_pos = nir+i;
            if(d_N_id_2 >= 0){// propagate N_id_2 if value is valid
                add_item_tag(0, nitems_written(0)+nout-1 ,d_id_key, pmt::pmt_from_long( d_N_id_2 ), d_tag_id);
            }
        }
        else if(mod_pos%d_slotl == slot_start ){ // found start of new slot within half frame
            d_slot_num = (d_slot_num+1)%10;
            d_symb_num = 0;
            f_sym = 7 * d_slot_num + d_symb_num;
            d_next_symbol = nir+i+d_fftl+d_cpl0;
            //printf("%s\telseif slot_start f_sym = %i\tpos = %ld\tlast = %ld\tll = %ld\n", name().c_str(), f_sym, nir+i, nir+i-d_fftl-d_cpl, nir+i-2*d_fftl-2*d_cpl);
            //printf("%s\toutput slot_str = %ld\tlast = %ld\tframe_symbol = %i\n", name().c_str(), nir+i, nir+i-d_slotl, f_sym);
            produce_output(out, in+i+d_cpl0, f_sym, nout);
            consumed_items += (d_fftl+d_cpl0);
            //i += (d_fftl+d_cpl0-5);
            d_next_symb_pos = nir+i;
        }
        else if(is_symbol_start(mod_pos, slot_start)){ // found symbol start within slot
        //This else if line does currently not work as expected! FIX ME!!!
      //else if( (mod_pos-d_fftl-d_cpl0-d_symb_num*(d_fftl+d_cpl))%d_slotl == slot_start ){ // found symbol start within slot
            d_symb_num = (d_symb_num+1)%7;
            f_sym = 7 * d_slot_num + d_symb_num;
            d_next_symbol = nir+i+d_fftl+d_cpl;
            //printf("%s\tmod_pos = %ld\thf_start = %ld\tslot_start = %ld\n", name().c_str(), mod_pos, d_half_frame_start, slot_start);
            //printf("%s\telseif symbol_start f_sym = %i\tpos = %ld\tlast = %ld\n", name().c_str(), f_sym, nir+i, nir+i-d_fftl-d_cpl);
            //printf("%s\toutput symbol_s = %ld\tlast = %ld\tframe_symbol = %i\n", name().c_str(), nir+i, nir+i-(d_fftl+d_cpl0), f_sym );
            produce_output(out, in+i+d_cpl, f_sym, nout);
            consumed_items += (d_fftl+d_cpl);
            //i += (d_fftl+d_cpl-5);
            d_next_symb_pos = nir+i;
        }
        //consumed_items = i-1;
        if(nout==noutput_items){//output buffer full! return and continue on next general_work call!
            //consumed_items = nout*(d_fftl+d_cpl);
            break;
        }
    }
    //consumed_items -= 5;

    // Still debugging stuff
    //printf("%s\tconsumed_items = %i\tnout = %i\tf_sym = %i\tlast = %i\n", name().c_str(), consumed_items, nout, f_sym, d_ofdm_symbol_count);
    if( f_sym != (d_ofdm_symbol_count+1)%70 ){
        printf("%s\tf_sym = %i\texpected = %i\n", name().c_str(), f_sym, (d_ofdm_symbol_count+1)%70 );
    }
    d_ofdm_symbol_count = f_sym;

    // Tell runtime system how many input items we consumed on
    // each input stream.
    consume_each (consumed_items);
    // Tell runtime system how many output items we produced.
    return noutput_items;
}

void
lte_remove_cp2_cvc::forecast (int noutput_items,
                              gr_vector_int &ninput_items_required)
{
    unsigned ninputs = ninput_items_required.size();
    for (unsigned i = 0; i < ninputs; i++)
         ninput_items_required[i] = 2* ( d_fftl + d_cpl0 ) * noutput_items;
}

// convenience function! be careful pointer to input buffer is not moved!
inline void
lte_remove_cp2_cvc::produce_output(gr_complex *&out, const gr_complex *in, int f_sym, int &nout)
{
    memcpy(out,in,sizeof(gr_complex)*d_fftl); //copy samples to output buffer!
    add_item_tag(0,nitems_written(0)+nout,d_key, pmt::pmt_from_long( f_sym ),d_tag_id);
    nout++; // 1 output vector produced
    out+=d_fftl; //move pointer to output buffer by the size of one vector

    //Debugging part
/*
    if( f_sym != (d_ofdm_symbol_count+1)%70 ){
        //printf("%s\tf_sym = %i\texpected = %i\n", name().c_str(), f_sym, (d_ofdm_symbol_count+1)%70 );
        //printf("%s\t%i\tclass_count = %i\n", name().c_str(), symb_num, d_ofdm_symbol_count); // debugging line!
    }
    //d_ofdm_symbol_count = f_sym;
*/
}

inline bool
lte_remove_cp2_cvc::is_symbol_start(long mod_pos, long slot_start)
{
    long val = mod_pos+d_slotl-d_fftl-d_cpl0;
    if(val%d_slotl == slot_start){return true;}
    if((val-1*(d_fftl+d_cpl))%d_slotl == slot_start){return true;}
    if((val-2*(d_fftl+d_cpl))%d_slotl == slot_start){return true;}
    if((val-3*(d_fftl+d_cpl))%d_slotl == slot_start){return true;}
    if((val-4*(d_fftl+d_cpl))%d_slotl == slot_start){return true;}
    if((val-5*(d_fftl+d_cpl))%d_slotl == slot_start){return true;}
    if((val-6*(d_fftl+d_cpl))%d_slotl == slot_start){return true;}
    return false;
}


