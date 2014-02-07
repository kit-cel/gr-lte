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
#include "pss_tagger_cc_impl.h"

namespace gr {
  namespace lte {

    pss_tagger_cc::sptr
    pss_tagger_cc::make(int fftl, std::string name)
    {
      return gnuradio::get_initial_sptr
        (new pss_tagger_cc_impl(fftl, name));
    }

    /*
     * The private constructor
     */
    pss_tagger_cc_impl::pss_tagger_cc_impl(int fftl, std::string& name)
      : gr::sync_block(name,
              gr::io_signature::make( 1, 1, sizeof(gr_complex)),
              gr::io_signature::make( 1, 1, sizeof(gr_complex))),
                d_fftl(fftl),
                d_cpl(144*fftl/2048),
                d_cpl0(160*fftl/2048),
                d_slotl(7*fftl+6*d_cpl+d_cpl0),
                d_half_frame_start(0),
                d_N_id_2(-1),
                d_slot_num(0),
                d_is_locked(false)
    {
        set_tag_propagation_policy(TPP_DONT);
        d_key=pmt::string_to_symbol("slot");
        d_tag_id=pmt::string_to_symbol(this->name() );
        d_id_key = pmt::string_to_symbol("N_id_2");
        
        message_port_register_in(pmt::mp("lock"));
		set_msg_handler(pmt::mp("lock"), boost::bind(&pss_tagger_cc_impl::handle_msg_lock, this, _1));

        message_port_register_in(pmt::mp("half_frame"));
		set_msg_handler(pmt::mp("half_frame"), boost::bind(&pss_tagger_cc_impl::handle_msg_half_frame_start, this, _1));

        message_port_register_in(pmt::mp("N_id_2"));
		set_msg_handler(pmt::mp("N_id_2"), boost::bind(&pss_tagger_cc_impl::handle_msg_N_id_2, this, _1));
    }
    
    void
    pss_tagger_cc_impl::handle_msg_N_id_2(pmt::pmt_t msg)
    {
        set_N_id_2( (int) pmt::to_long(msg) );
    }
    
    void
    pss_tagger_cc_impl::handle_msg_half_frame_start(pmt::pmt_t msg)
    {
        set_half_frame_start(pmt::to_long(msg) );
    }
    
    void
    pss_tagger_cc_impl::handle_msg_lock(pmt::pmt_t msg)
    {
        if(msg == pmt::PMT_T)
        {
            lock();
        }
        else{
            unlock();
        }
    }

    /*
     * Our virtual destructor.
     */
    pss_tagger_cc_impl::~pss_tagger_cc_impl()
    {
    }

    int
    pss_tagger_cc_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
        const gr_complex *in = (const gr_complex *) input_items[0];
        gr_complex *out = (gr_complex *) output_items[0];

        memcpy(out,in,sizeof(gr_complex)*noutput_items);

        long nin = nitems_read(0);
        int half_framel = 10*d_slotl;
        int offset = d_half_frame_start%d_slotl;

        for (int i = 0 ; i < noutput_items; i++){

            if( (nin+i)%d_slotl == offset){ // removed abs
                if((nin+i)%half_framel == d_half_frame_start){ // removed abs
                    //printf("found half_frame_start\t num = %li\t0 < %li\n", nitems_read(0)+i,(nitems_read(0)+i-d_half_frame_start) );
                    if(d_is_locked){
                        //printf("%s\thalf_frame_start = %i\tabs_pos = %ld\n", name().c_str(), d_half_frame_start, nitems_read(0)+i );
                        add_item_tag(0,nin+i,d_id_key, pmt::from_long(d_N_id_2),d_tag_id);
                        d_slot_num=0;
                    }
                }

                //printf("%s\tslot_num = %i\tabs_pos = %ld\n",name().c_str(),d_slot_num,nitems_read(0)+i );
                add_item_tag(0,nin+i,d_key, pmt::from_long(d_slot_num),d_tag_id);
                d_slot_num = (d_slot_num+1)%10;

                if(i+d_slotl < noutput_items){
                    i += (d_slotl-1);
                }
                else{
                    i+=(noutput_items-i-1);
                }
            }
        }

        // Tell runtime system how many output items we produced.
        return noutput_items;
    }

  } /* namespace lte */
} /* namespace gr */

