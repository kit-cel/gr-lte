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
     d_work_call(0)
{
     d_key=pmt::pmt_string_to_symbol("OFDM_symbol");
     d_tag_id=pmt::pmt_string_to_symbol(name() );
     set_tag_propagation_policy(TPP_DONT);
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

    // First thing to do get tags and calculate offset and half_frame_start!
    std::vector <gr_tag_t> v;
    get_tags_in_range(v, 0, nir, nir+noutput_items*(d_fftl+d_cpl0), pmt::pmt_string_to_symbol("slot") );
    for(int i = 0; i < v.size(); i++) {
        long offset = v[i].offset; // get position of tag
        long slot_num = pmt::pmt_to_long(v[i].value); // get tag value (represents slot number in a half frame)
        d_half_frame_start = (offset-(d_slotl*slot_num))%(10*d_slotl); // calculate half_frame_start
        printf("%s\thf_start = %ld\tslot_num = %ld\tabs_pos = %ld\n", name().c_str(), d_half_frame_start, slot_num, offset );
    }
    long slot_start = d_half_frame_start%d_slotl;

    int mod_pos = 0;
    for(int i = 0 ; i < nin ; i++){
        mod_pos = (nir+i)%(10*d_slotl);
        if( mod_pos == d_half_frame_start){
            d_slot_num = 0;
            d_symb_num = 0;
            //produce output
        }
        else if(mod_pos%d_slotl == slot_start ){
            d_slot_num = (d_slot_num+1)%10;
            d_symb_num = 0;
            //produce output
        }
        else if( (mod_pos + (d_fftl+d_cpl0) + d_symb_num*(d_fftl+d_cpl) )%d_slotl == slot_start){
            d_symb_num = (d_symb_num+1)%7;

            //produce output
        }
    }





    // Tell runtime system how many input items we consumed on
    // each input stream.
    consume_each (noutput_items*d_fftl);

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

void
lte_remove_cp2_cvc::forecast (int noutput_items, gr_vector_int &ninput_items_required)
{
    unsigned ninputs = ninput_items_required.size();
    for (unsigned i = 0; i < ninputs; i++)
         ninput_items_required[i] = ( d_fftl + d_cpl0 ) * noutput_items;
}

