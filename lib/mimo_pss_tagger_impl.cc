/* -*- c++ -*- */
/*
 * Copyright 2013 Communications Engineering Lab (CEL) / Karlsruhe Institute of Technology (KIT)
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
#include "mimo_pss_tagger_impl.h"

#include <cstdio>

namespace gr {
  namespace lte {

    mimo_pss_tagger::sptr
    mimo_pss_tagger::make(int fftl)
    {
      return gnuradio::get_initial_sptr
        (new mimo_pss_tagger_impl(fftl));
    }

    /*
     * The private constructor
     */
    mimo_pss_tagger_impl::mimo_pss_tagger_impl(int fftl)
      : gr::sync_block("mimo_pss_tagger",
              gr::io_signature::make(2, 2, sizeof(gr_complex)),
              gr::io_signature::make(2, 2, sizeof(gr_complex))),
              d_is_locked(false)
    {


        message_port_register_in(pmt::mp("lock"));
        set_msg_handler(pmt::mp("lock"), boost::bind(&mimo_pss_tagger_impl::handle_msg_lock, this, _1));
        message_port_register_in(pmt::mp("half_frame"));
        set_msg_handler(pmt::mp("lock"), boost::bind(&mimo_pss_tagger_impl::handle_msg_half_frame, this, _1));
        message_port_register_in(pmt::mp("N_id_2"));
        set_msg_handler(pmt::mp("lock"), boost::bind(&mimo_pss_tagger_impl::handle_msg_N_id_2, this, _1));

       printf("tagger");


    }

    /*
     * Our virtual destructor.
     */
    mimo_pss_tagger_impl::~mimo_pss_tagger_impl()
    {
    }

    void mimo_pss_tagger_impl::handle_msg_lock(pmt::pmt_t msg){
        if(msg==pmt::PMT_T){
            d_is_locked=true;
        }else{
            d_is_locked=false;
        }
    }

    void mimo_pss_tagger_impl::handle_msg_half_frame(pmt::pmt_t msg){

    }

    void mimo_pss_tagger_impl::handle_msg_N_id_2(pmt::pmt_t msg){

    }


    int
    mimo_pss_tagger_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
        const gr_complex *in1 = (const gr_complex *) input_items[0];
        const gr_complex *in2 = (const gr_complex *) input_items[1];
        gr_complex *out1 = (gr_complex *) output_items[0];
        gr_complex *out2 = (gr_complex *) output_items[1];

        memcpy(out1, in1, sizeof(gr_complex)*noutput_items);
        memcpy(out2, in2, sizeof(gr_complex)*noutput_items);


//        if((nir+i)%d_slotl==d_half_frame_start%d_slotl){
//                if((nir+i)%d_halffl==d_half_frame_start){
//                //attach tag with Nid2 at start of half frame
//                    add_item_tag(0,nir+i,d_id_key, pmt::from_long(d_N_id_2),d_tag_id);
//                    d_slot_num=0;
//                }
//                //attach tag with slotnumber at start of slot
//                add_item_tag(0,nir+i,d_slot_key, pmt::from_long(d_slot_num),d_tag_id);
//                d_slot_num = (d_slot_num+1)%10;
//
//        }


        // Tell runtime system how many output items we produced.
        return noutput_items;
    }

  } /* namespace lte */
} /* namespace gr */

