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
#include "mimo_pss_tagger_impl.h"

#include <cstdio>

namespace gr
{
namespace lte
{

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
                     gr::io_signature::make(1, 8, sizeof(gr_complex)),
                     gr::io_signature::make(1, 8, sizeof(gr_complex))),
    d_fftl(fftl),
    d_cpl(144*fftl/2048),
    d_cpl0(160*fftl/2048),
    d_slotl(7*fftl+6*d_cpl+d_cpl0),
    d_halffl(10*d_slotl),
    d_half_frame_start(-1),
    d_N_id_2(-1),
    d_slot_num(-1),
    d_is_locked(false)
{
    d_slot_key=pmt::string_to_symbol("slot");
    d_tag_id = pmt::string_to_symbol(this->name() );
    d_id_key = pmt::string_to_symbol("N_id_2");

    message_port_register_in(pmt::mp("lock"));
    set_msg_handler(pmt::mp("lock"), boost::bind(&mimo_pss_tagger_impl::handle_msg_lock, this, _1));
    message_port_register_in(pmt::mp("half_frame"));
    set_msg_handler(pmt::mp("half_frame"), boost::bind(&mimo_pss_tagger_impl::handle_msg_half_frame, this, _1));
    message_port_register_in(pmt::mp("N_id_2"));
    set_msg_handler(pmt::mp("N_id_2"), boost::bind(&mimo_pss_tagger_impl::handle_msg_N_id_2, this, _1));
}

/*
 * Our virtual destructor.
 */
mimo_pss_tagger_impl::~mimo_pss_tagger_impl()
{
}

void
mimo_pss_tagger_impl::handle_msg_lock(pmt::pmt_t msg)
{
    d_is_locked = (msg==pmt::PMT_T) ? true : false;
}

void
mimo_pss_tagger_impl::handle_msg_half_frame(pmt::pmt_t msg)
{
    d_half_frame_start = (int)pmt::to_long(msg);
}

void
mimo_pss_tagger_impl::handle_msg_N_id_2(pmt::pmt_t msg)
{
    d_N_id_2 = (int)pmt::to_long(msg);
}


int
mimo_pss_tagger_impl::work(int noutput_items,
                           gr_vector_const_void_star &input_items,
                           gr_vector_void_star &output_items)
{
    //copy input to output, block just set tags
    int rx = input_items.size();
    for(int i=0; i<rx; i++)
    {
        const gr_complex *in = (const gr_complex *) input_items[i];
        gr_complex *out = (gr_complex*) output_items[i];
        memcpy(out, in, sizeof(gr_complex)*noutput_items);
    }

    //no tags if there is no sync yet
    if(d_half_frame_start == -1){
        return noutput_items;
    }

    long nir = nitems_read(0);
    int offset = d_half_frame_start%d_slotl;



    for (int i = 0 ; i < noutput_items; i++)
    {
        if( (nir+i)%d_slotl == offset)
        {
            if((nir+i)%d_halffl == d_half_frame_start)
            {
                //printf("found half_frame_start\t num = %li\t0 < %li\n", nitems_read(0)+i,(nitems_read(0)+i-d_half_frame_start) );
                if(d_is_locked)
                {
                    //printf("%s\thalf_frame_start = %i\tabs_pos = %ld\n", name().c_str(), d_half_frame_start, nitems_read(0)+i );
                    //set a tag at the start of a half frame
                    add_item_tag(0,nir+i,d_id_key, pmt::from_long(d_N_id_2),d_tag_id);
                    d_slot_num=0;
                }
            }

            if (d_slot_num != -1){
                //printf("%s\tslot_num = %i\tabs_pos = %ld\n",name().c_str(),d_slot_num,nitems_read(0)+i );
                //set a tag with the slotnumber within a half frame at the start of a slot
                add_item_tag(0,nir+i,d_slot_key, pmt::from_long(d_slot_num),d_tag_id);
                d_slot_num = (d_slot_num+1)%10;
            }
            //step over next samples
            if(i+d_slotl < noutput_items)
            {
                i += (d_slotl-1);
            }
            else
            {
                i+=(noutput_items-i-1);
            }
        }
    }

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

} /* namespace lte */
} /* namespace gr */

