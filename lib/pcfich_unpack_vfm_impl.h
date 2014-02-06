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

#ifndef INCLUDED_LTE_PCFICH_UNPACK_VFM_IMPL_H
#define INCLUDED_LTE_PCFICH_UNPACK_VFM_IMPL_H

#include <lte/pcfich_unpack_vfm.h>

namespace gr {
  namespace lte {

    class pcfich_unpack_vfm_impl : public pcfich_unpack_vfm
    {
     private:
        struct cfi_result {
            int cfi;
            float val;
        };
        
        pmt::pmt_t d_port_cfi;
        pmt::pmt_t d_key;
        float* d_in_seq;
        std::vector<float*> d_ref_seqs;
        int d_subframe;
        void initialize_ref_seqs();
        
        cfi_result calculate_cfi(float* in_seq);
        float correlate(float* in0, float* in1, int len);

        void publish_cfi(int subframe, cfi_result cfi);
        
        // Helpers to write test code
        bool d_dbg;
        void activate_debug_mode(bool ena){d_dbg = ena;}
        std::vector<int> d_cfi_results;
        std::vector<int> cfi_results() {return d_cfi_results;}

     public:
      pcfich_unpack_vfm_impl(std::string key, std::string msg_buf_name, std::string& name);
      ~pcfich_unpack_vfm_impl();

      // Where all the action really happens
      int work(int noutput_items,
            gr_vector_const_void_star &input_items,
            gr_vector_void_star &output_items);
    };

  } // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_PCFICH_UNPACK_VFM_IMPL_H */

