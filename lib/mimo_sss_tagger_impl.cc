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

namespace gr {
  namespace lte {

    mimo_sss_tagger::sptr
    mimo_sss_tagger::make(int fftl, int rxant)
    {
      return gnuradio::get_initial_sptr
        (new mimo_sss_tagger_impl(fftl, rxant));
    }

    /*
     * The private constructor
     */
    mimo_sss_tagger_impl::mimo_sss_tagger_impl(int fftl, int rxant)
      : gr::sync_block("mimo_sss_tagger",
              gr::io_signature::make( 1, 8, sizeof(gr_complex)),
              gr::io_signature::make( 1, 8, sizeof(gr_complex))),
                d_fftl(fftl),
                d_rxant(rxant),
                d_cpl(144*fftl/2048),
                d_cpl0(160*fftl/2048),
                d_slotl(7*fftl+6*d_cpl+d_cpl0),
                d_framel(20*d_slotl),
                d_offset_0(0),
                d_frame_start(0),
                d_slot_num(41)
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
        set_frame_start(pmt::to_long(msg) );
    }


    int
    mimo_sss_tagger_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {

        //This block does not change data. It just adds new itemtags!
        for(int i=0; i<d_rxant; i++)
            memcpy(output_items[i],input_items[i],sizeof(gr_complex)*noutput_items);

        long nin = nitems_read(0);


        std::vector <gr::tag_t> v;
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
                    add_item_tag(0,nin+i,d_key, pmt::from_long( d_slot_num ),d_tag_id);
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
                add_item_tag(0,nin+i,d_key, pmt::from_long( d_slot_num ),d_tag_id);
                if(i+d_slotl < noutput_items){
                    i += (d_slotl-1);
                }
                else{
                    i+=(noutput_items-i-1);
                }

                // prepare values for next iteration.
                d_slot_num = (d_slot_num + 1) % 20;

            }
        }

        // Tell runtime system how many output items we produced.
        return noutput_items;
    }

    void
    mimo_sss_tagger_impl::set_frame_start(long frame_start)
    {
        d_frame_start = frame_start;
    }

  } /* namespace lte */
} /* namespace gr */

