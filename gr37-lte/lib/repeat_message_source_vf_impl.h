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

#ifndef INCLUDED_LTE_REPEAT_MESSAGE_SOURCE_VF_IMPL_H
#define INCLUDED_LTE_REPEAT_MESSAGE_SOURCE_VF_IMPL_H

#include <lte/repeat_message_source_vf.h>

namespace gr {
  namespace lte {

    class repeat_message_source_vf_impl : public repeat_message_source_vf
    {
     private:
      int d_vector_len;
      bool d_is_initialized;
      float* d_out_vector;
      pmt::pmt_t d_port;
      
      void handle_msg(pmt::pmt_t msg);

     public:
      repeat_message_source_vf_impl(int vector_len);
      ~repeat_message_source_vf_impl();

      // Where all the action really happens
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_REPEAT_MESSAGE_SOURCE_VF_IMPL_H */

