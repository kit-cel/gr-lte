/* -*- c++ -*- */
/*
 * Copyright 2014 Communications Engineering Lab (CEL) / Karlsruhe Institute of Technology (KIT)
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

#include <gnuradio/io_signature.h>
#include "mimo_sss_symbol_selector_impl.h"

namespace gr {
  namespace lte {

    mimo_sss_symbol_selector::sptr
    mimo_sss_symbol_selector::make(int fftl, int rxant)
    {
      return gnuradio::get_initial_sptr
        (new mimo_sss_symbol_selector_impl(fftl, rxant));
    }

    /*
     * The private constructor
     */
    mimo_sss_symbol_selector_impl::mimo_sss_symbol_selector_impl(int fftl, int rxant)
      : gr::block("mimo_sss_symbol_selector",
              gr::io_signature::make(1, 8, sizeof(gr_complex)),
              gr::io_signature::make(1, 8, sizeof(gr_complex)* fftl)),
                d_fftl(fftl),
                d_rxant(rxant),
                d_cpl(144*fftl/2048),
                d_cpl0(160*fftl/2048),
                d_slotl(7*fftl+6*d_cpl+d_cpl0),
                d_slot_num(0),
                d_sym_num(0),
                d_abs_pos(0),
                d_offset(0),
                d_N_id_2(-1)
    {
        set_relative_rate(1.0/double(d_fftl+d_cpl0));
        //printf("rel_rate = %f\n",relative_rate());
        set_tag_propagation_policy(TPP_DONT);
        d_key = pmt::string_to_symbol("offset_marker");
        d_id_key = pmt::string_to_symbol("N_id_2");
        d_tag_id = pmt::string_to_symbol(this->name());
    }

    /*
     * Our virtual destructor.
     */
    mimo_sss_symbol_selector_impl::~mimo_sss_symbol_selector_impl()
    {
    }

    void
    mimo_sss_symbol_selector_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        for(int i = 0 ; i < ninput_items_required.size() ; i++){
            ninput_items_required[i] = noutput_items;
        }
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    int
    mimo_sss_symbol_selector_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)

        //printf("work! nitems_read+ninput_items = %ld\t", nitems_read(0) + ninput_items[0]);
        std::vector <gr::tag_t> v_id;
        get_tags_in_range(v_id,0,nitems_read(0),nitems_read(0)+ninput_items[0],d_id_key);
        if (v_id.size() > 0){
            d_N_id_2 = int(pmt::to_long(v_id[0].value));
            long id_off = v_id[0].offset;
            //printf("%s found N_id_2 = %i\t id_off = %ld\n",name().c_str(), d_N_id_2, id_off);
        }

        pmt::pmt_t slot_key=pmt::string_to_symbol("slot");
        std::vector <gr::tag_t> v;
        get_tags_in_range(v,0,nitems_read(0),nitems_read(0)+ninput_items[0],slot_key);
        if (v.size() > 0){

            long value = pmt::to_long(v[0].value);
            //printf("%s tag: found\tvalue = %ld\toffset = %ld\n",name().c_str(),value,v[0].offset );
            if(value == 0){
                d_offset = v[0].offset;
                d_abs_pos = v[0].offset+5*d_fftl+5*d_cpl+d_cpl0; // points at the exact beginning of a SSS symbol.
                //printf("\n%s tag: d_abs_pos = %ld\n",name().c_str(), d_abs_pos);
            }
        }

        if(d_abs_pos >= nitems_read(0) && d_N_id_2 >= 0 ){
            //printf("found:  d_abs_pos = %ld >= %ld = nitems_read\n", d_abs_pos, nitems_read(0) );


            if(d_abs_pos+d_fftl < nitems_read(0)+ninput_items[0]){
                //printf("\nPRODUCE real output!\tdiff = %ld\tpos = %ld\n\n",d_abs_pos-nitems_read(0), d_abs_pos);

                for(int i=0; i<d_rxant; i++
                {
                    const gr_complex *in = (const gr_complex *) input_items[i];
                    gr_complex *out = (gr_complex *) output_items[i];
                    memcpy(out,in+(d_abs_pos-nitems_read(0) ),sizeof(gr_complex)*d_fftl);
                }

                add_item_tag(0,nitems_written(0),d_key   , pmt::from_long( d_offset ),d_tag_id);
                add_item_tag(0,nitems_written(0),d_id_key, pmt::from_long( d_N_id_2 ),d_tag_id);
                consume_each(ninput_items[0]);
                return 1;
            }
            else{
                int diff = int(d_abs_pos-nitems_read(0));
                //printf("nitems_read + diff = %ld\n",nitems_read(0)+diff );
                if(diff > ninput_items[0] ){
                    consume_each(ninput_items[0]);
                }
                else{
                    consume_each( diff );
                }
                return 0;
            }

        }
        else{
            //printf("missed: d_abs_pos = %ld  < %ld = nitems_read\n", d_abs_pos, nitems_read(0) );
            consume_each(ninput_items[0]);
            return 0;
        }

        // Tell runtime system how many input items we consumed on
        // each input stream.
        //consume_each (noutput_items);

        // Tell runtime system how many output items we produced.
        //return noutput_items;

    }



  } /* namespace lte */
} /* namespace gr */

