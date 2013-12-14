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

#ifndef INCLUDED_LTE_SSS_TAGGER_CC_IMPL_H
#define INCLUDED_LTE_SSS_TAGGER_CC_IMPL_H

#include <lte/sss_tagger_cc.h>

namespace gr {
  namespace lte {

    class sss_tagger_cc_impl : public sss_tagger_cc
    {
     private:
        int d_fftl;
        int d_cpl;
        int d_cpl0;
        int d_slotl;
        int d_framel;
        int d_slot_num;
        uint64_t d_offset_0;
        pmt::pmt_t d_key;
        pmt::pmt_t d_tag_id;
        long d_frame_start;
        
        void set_frame_start(long frame_start);
        void handle_msg_frame_start(pmt::pmt_t msg);

     public:
      sss_tagger_cc_impl(int fftl);
      ~sss_tagger_cc_impl();

      // Where all the action really happens
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_SSS_TAGGER_CC_IMPL_H */

