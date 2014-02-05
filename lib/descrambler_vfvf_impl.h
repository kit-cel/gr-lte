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

#ifndef INCLUDED_LTE_DESCRAMBLER_VFVF_IMPL_H
#define INCLUDED_LTE_DESCRAMBLER_VFVF_IMPL_H

#include <lte/descrambler_vfvf.h>

namespace gr {
  namespace lte {

    class descrambler_vfvf_impl : public descrambler_vfvf
    {
     private:
        pmt::pmt_t d_tag_key;
        pmt::pmt_t d_msg_buf;
        int d_len;

        std::vector<float*> d_scr_seq_vec;
        int d_scr_seq_len;
        int d_num_seqs;
        int d_seq_index;
        int d_part;

        inline void handle_msg(pmt::pmt_t msg);
        float* get_aligned_sequence(std::vector<float> seq);
        int get_seq_num(int idx);

     public:
      descrambler_vfvf_impl(std::string& name, std::string tag_key, std::string msg_buf_name, int len);
      ~descrambler_vfvf_impl();

      // Where all the action really happens
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
      
      void set_descr_seqs(std::vector<std::vector<float> > seqs);
    };

  } // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_DESCRAMBLER_VFVF_IMPL_H */

