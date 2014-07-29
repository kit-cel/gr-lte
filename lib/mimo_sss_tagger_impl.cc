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
#include "mimo_sss_tagger_impl.h"

#include <cstdio>

namespace gr {
  namespace lte {

    mimo_sss_tagger::sptr
    mimo_sss_tagger::make(int rxant, int N_rb_dl)
    {
      return gnuradio::get_initial_sptr
        (new mimo_sss_tagger_impl(rxant, N_rb_dl));
    }

    /*
     * The private constructor
     */
    mimo_sss_tagger_impl::mimo_sss_tagger_impl(int rxant, int N_rb_dl)
      : gr::sync_block("mimo_sss_tagger",
              gr::io_signature::make( 1, 8, sizeof(gr_complex) * N_rb_dl * 12 * rxant),
              gr::io_signature::make( 1, 8, sizeof(gr_complex) * N_rb_dl * 12 * rxant)),
                d_rxant(rxant),
                d_N_rb_dl(N_rb_dl),
                d_frame_start(-1),
                d_sym_num(-1)
    {
        set_tag_propagation_policy(TPP_DONT);
        d_key = pmt::string_to_symbol("slot");
        d_tag_id = pmt::string_to_symbol(this->name() );

        message_port_register_in(pmt::mp("frame_start"));
		set_msg_handler(pmt::mp("frame_start"), boost::bind(&mimo_sss_tagger_impl::handle_msg_frame_start, this, _1));

    }


    /*
     * Our virtual destructor.
     */
    mimo_sss_tagger_impl::~mimo_sss_tagger_impl()
    {
    }

    void
    mimo_sss_tagger_impl::handle_msg_frame_start(pmt::pmt_t msg)
    {
        d_frame_start = int( pmt::to_long(msg) );
    }


    int
    mimo_sss_tagger_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {

        //This block does not change data. It just adds new itemtags!
        memcpy(output_items[0],input_items[0],sizeof(gr_complex)*d_N_rb_dl*12*d_rxant*noutput_items);

        //no tags if there is no sync
        if(d_frame_start == -1)
            return noutput_items;


        long nin = nitems_read(0);
        d_sym_num = (int)(nin - d_frame_start +140) % 140;

        for(int i=0; i < noutput_items; i++){

            if(d_sym_num % 7 == 0){
                add_item_tag(0,nin+i,d_key, pmt::from_long( d_sym_num ),d_tag_id);
             }
             d_sym_num = (d_sym_num+1)%140;
        }

        // Tell runtime system how many output items we produced.
        return noutput_items;
    }

  } /* namespace lte */
} /* namespace gr */
