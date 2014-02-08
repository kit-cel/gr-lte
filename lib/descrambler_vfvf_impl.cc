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
#include "descrambler_vfvf_impl.h"
#include <cstdio>
#include <fftw3.h>
#include <volk/volk.h>
#include <boost/format.hpp>

namespace gr {
  namespace lte {

    descrambler_vfvf::sptr
    descrambler_vfvf::make(std::string tag_key, std::string msg_buf_name, int len, std::string name)
    {
      return gnuradio::get_initial_sptr
        (new descrambler_vfvf_impl(name, tag_key, msg_buf_name, len));
    }

    /*
     * The private constructor
     */
    descrambler_vfvf_impl::descrambler_vfvf_impl(std::string& name, std::string tag_key, std::string msg_buf_name, int len)
      : gr::sync_block(name,
              gr::io_signature::make(1, 1, sizeof(float) * len),
              gr::io_signature::make(1, 1, sizeof(float) * len)),
              d_len(len),
              d_seq_index(0),
              d_part(0)
    {
        d_tag_key = pmt::string_to_symbol(tag_key);

        d_msg_buf = pmt::mp(msg_buf_name);
        message_port_register_in(d_msg_buf);
        set_msg_handler(d_msg_buf, boost::bind(&descrambler_vfvf_impl::handle_msg, this, _1));
    }

    /*
     * Our virtual destructor.
     */
    descrambler_vfvf_impl::~descrambler_vfvf_impl()
    {
    }

    int
    descrambler_vfvf_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
    	const float *in = (const float *) input_items[0];
    	float *out = (float *) output_items[0];


        int scr_pos = 0;
        int max_parts = d_scr_seq_len/d_len;

        int seq_num = get_seq_num(0);
        int part = d_part;

        for(int i = 0; i < noutput_items; i++){
            int next = get_seq_num(i);
            if(seq_num !=  next){
                part = 0;
                seq_num = next;
            }
            scr_pos = part * d_len;
            float* seq = &d_scr_seq_vec[seq_num][scr_pos];
            volk_32f_x2_multiply_32f_u(out, in, seq, d_len);
            part = (part+1)%(max_parts);
            out += d_len;
            in += d_len;
        }
        d_part = part;

        // Tell runtime system how many output items we produced.
        return noutput_items;
    }
    
    int
    descrambler_vfvf_impl::get_seq_num(int idx)
    {
        int seq_num = 0;
        int pos = nitems_read(0)+idx;
        std::vector <gr::tag_t> v_b;
        get_tags_in_range(v_b, 0, pos, pos+1, d_tag_key);
        if(v_b.size() > 0){
            //std::string srcid = "srcid"; //pmt::pmt_symbol_to_string(v_b[0].srcid );
            long offset = v_b[0].offset;
            int value = int(pmt::to_long(v_b[0].value) );
            seq_num = (value - int(offset-pos) +d_num_seqs)%d_num_seqs;
            d_seq_index = seq_num;
            d_part = 0;
    //        printf("NEW\t");
        }
        else{
            seq_num = d_seq_index;
    //        printf("OLD\t");
        }
    //    printf("get_seq_num = %i\tidx = %i\tpos = %i\n", seq_num, idx, pos);
        return seq_num;
    }

    void
    descrambler_vfvf_impl::handle_msg(pmt::pmt_t msg)
    {
        pmt::print(msg);
        if(not pmt::is_vector(msg)){
            throw std::runtime_error("message doesn't have vector type! Can't use it!\n");
        }
        printf("%s received msg", name().c_str());
        std::vector<std::vector<float> > seqs;
        for(int i = 0; i < pmt::length(msg); i++){
            printf("vector num = %i of %lu\n", i, pmt::length(msg));
            pmt::pmt_t pmt_vec = pmt::vector_ref(msg, i);
            std::vector<float> vec;
            for(int e = 0; e < pmt::length(pmt_vec); e++){
                float elem = (float)pmt::to_double(pmt::vector_ref(pmt_vec, e));
                printf("ns = %i, e = %i, val = %f\n", i, e, elem);
                vec.push_back(/*pmt::f32vector_ref(pmt_vec, e)*/ elem);
            }
            seqs.push_back(vec);
        }
        set_descr_seqs(seqs);
    }

    void
    descrambler_vfvf_impl::set_descr_seqs(std::vector<std::vector<float> > seqs)
    {
        GR_LOG_INFO(d_debug_logger, "set_descr_seqs\n");
        d_scr_seq_len = seqs[0].size();
        d_num_seqs = seqs.size();

        std::vector<float*> aligned_seqs;
        for(int n = 0; n < seqs.size(); n++) {
            aligned_seqs.push_back(get_aligned_sequence(seqs[n]));
        }
        d_scr_seq_vec = aligned_seqs;
    }

    float*
    descrambler_vfvf_impl::get_aligned_sequence(std::vector<float> seq)
    {
        float* vec = (float*)fftwf_malloc(sizeof(float)*seq.size() );
        memcpy(vec, &seq[0], sizeof(float)*seq.size() );
        return vec;
    }

  } /* namespace lte */
} /* namespace gr */

