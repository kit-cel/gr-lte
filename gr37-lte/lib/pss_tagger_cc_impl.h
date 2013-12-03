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

#ifndef INCLUDED_LTE_PSS_TAGGER_CC_IMPL_H
#define INCLUDED_LTE_PSS_TAGGER_CC_IMPL_H

#include <lte/pss_tagger_cc.h>

namespace gr {
  namespace lte {

    class pss_tagger_cc_impl : public pss_tagger_cc
    {
     private:
        int d_fftl;
        int d_cpl;
        int d_cpl0;
        int d_slotl;
        int d_half_frame_start;
        int d_N_id_2;
        int d_slot_num;
        bool d_is_locked;
        pmt::pmt_t d_key;
        pmt::pmt_t d_id_key;
        pmt::pmt_t d_tag_id;
        
        void handle_msg_lock(pmt::pmt_t msg);
        void handle_msg_half_frame_start(pmt::pmt_t msg);
        void handle_msg_N_id_2(pmt::pmt_t msg);

     public:
      pss_tagger_cc_impl(int fftl);
      ~pss_tagger_cc_impl();

      // Where all the action really happens
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
         
        void set_half_frame_start(int start){d_half_frame_start = start;}
        void set_N_id_2(int nid2){d_N_id_2 = nid2;}
        void lock(){d_is_locked = true;}
        void unlock(){d_is_locked = false;}      
    };

  } // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_PSS_TAGGER_CC_IMPL_H */

