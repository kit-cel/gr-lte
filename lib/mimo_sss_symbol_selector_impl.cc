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
#include <cstdio>

namespace gr {
  namespace lte {

    mimo_sss_symbol_selector::sptr
    mimo_sss_symbol_selector::make(int rxant, int N_rb_dl)
    {
      return gnuradio::get_initial_sptr
        (new mimo_sss_symbol_selector_impl(rxant, N_rb_dl));
    }

    /*
     * The private constructor
     */
    mimo_sss_symbol_selector_impl::mimo_sss_symbol_selector_impl(int rxant, int N_rb_dl)
      : gr::block("mimo_sss_symbol_selector",
              gr::io_signature::make(1, 8, sizeof(gr_complex) * N_rb_dl * 12 * rxant),
              gr::io_signature::make(1, 8, sizeof(gr_complex) * 72 * rxant)),
                d_rxant(rxant),
                d_slot_num(0),
                d_sym_num(0),
                d_abs_pos(0),
                d_offset(0),
                d_N_rb_dl(N_rb_dl)
    {
        set_relative_rate(1.0/70);
        //printf("rel_rate = %f\n",relative_rate());
        set_tag_propagation_policy(TPP_DONT);
        d_key_offset = pmt::string_to_symbol("offset");
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
        {
        gr_complex *out = (gr_complex *) output_items[0];
        const gr_complex *in = (const gr_complex *) input_items[0];

        std::vector <gr::tag_t> v;
        pmt::pmt_t symbol_key=pmt::string_to_symbol("slot");
        get_tags_in_range(v,0,nitems_read(0),nitems_read(0)+ninput_items[0],symbol_key);
        if (v.size() > 0){
            d_sym_num = get_sym_num(v);//int(pmt::to_long(v[0].value));
        }

        int n_carriers = 12 * d_N_rb_dl;
        int sss_pos = n_carriers/2 - 72/2;
        int produced_items = 0;


        for (int i = 0; i < noutput_items; i++){
            //printf("symnum: %i\n", d_sym_num);
            if(d_sym_num == 5){
                for(int rx=0; rx<d_rxant; rx++){
                    memcpy(out, in + sss_pos, sizeof(gr_complex) * 72 );
                    out += 72;
                    in += n_carriers;
                }
                long nir = nitems_read(0)+i;
                long niw = nitems_written(0)+produced_items;
                //printf("nir: %li\n", nir);
                //add item tag with offset position in main stream
                add_item_tag(0, niw, d_key_offset, pmt::from_long(nir), d_tag_id);
                produced_items++;
            }else{
                in += n_carriers * d_rxant;
            }

            d_sym_num = (d_sym_num+1)%70;
        }

        consume_each(noutput_items);

		// Tell runtime system how many output items we produced.
		return produced_items;


    }

    inline int
    mimo_sss_symbol_selector_impl::get_sym_num(std::vector<gr::tag_t> v)
    {
        int sym_num = 0;
        if(v.size() > 0){
            int value = int(pmt::to_long(v[0].value) );
            int rel_offset = v[0].offset - nitems_read(0);
            sym_num = (value+70-rel_offset)%70;
        }
        else{
            sym_num = d_sym_num;
        }
        return sym_num;
    }

  } /* namespace lte */
} /* namespace gr */
