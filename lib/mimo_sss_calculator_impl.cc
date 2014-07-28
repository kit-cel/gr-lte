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
#include "mimo_sss_calculator_impl.h"
#include <volk/volk.h>
#include <cstdio>

#include <boost/multi_array.hpp>

namespace gr {
  namespace lte {

    mimo_sss_calculator::sptr
    mimo_sss_calculator::make(int rxant)
    {
      return gnuradio::get_initial_sptr
        (new mimo_sss_calculator_impl(rxant));
    }

    /*
     * The private constructor
     */
    mimo_sss_calculator_impl::mimo_sss_calculator_impl(int rxant)
      : gr::sync_block("mimo_sss_calculator",
              gr::io_signature::make(1, 1, sizeof(gr_complex)*72*rxant),
              gr::io_signature::make(0, 0, 0)),
                d_rxant(rxant),
                d_cell_id(-1),
                d_max_val_new(0.0),
                d_max_val_old(0.0),
                d_N_id_2(-1),
                d_offset(0),
                d_sss_pos(0),
                d_frame_start(0),
                d_is_locked(false),
                d_unchanged_id(0)
    {
        d_key_offset = pmt::string_to_symbol("offset");
//        d_key_offset = pmt::string_to_symbol("offset_marker");
        // Get needed message ports!
        d_port_cell_id = pmt::string_to_symbol("cell_id");
        message_port_register_out(d_port_cell_id);
        d_port_frame_start = pmt::string_to_symbol("frame_start");
        message_port_register_out(d_port_frame_start);

        message_port_register_in(pmt::mp("sector_id"));
		set_msg_handler(pmt::mp("sector_id"), boost::bind(&mimo_sss_calculator_impl::msg_set_sector_id, this, _1));

        //initialize d_cX
        char cX_x[31] = {0};
        cX_x[4] = 1;
        for(int i = 0; i < 26 ; i++ ){
            cX_x[i+5] = (cX_x[i+3] + cX_x[i])%2;
        }
        //Do NRZ coding (necessary here?)
        for(int i = 0 ; i < 31 ; i++){
            d_cX[i] = 1-2*cX_x[i];
        }

        //initialize d_sref
        int m0_ref=0;
        char sX_x[31]={0};
        sX_x[4]=1;
        for(int i = 0; i < 26 ; i++){
            sX_x[i+5] = (sX_x[i+2] + sX_x[i])%2;
        }
        gr_complex sX[31]={0};
        for(int i = 0 ; i < 31 ; i++){
            sX[i] = 1-2*sX_x[i];
        }
        int m0 = 0;
        for(int i = 0 ; i < 31 ; i++){
            d_sref[i   ]=sX[ (i+m0)%31 ];
            d_sref[i+31]=sX[ (i+m0)%31 ];
        }

        //initialize d_zX
        char zX_x[31] = {0};
        zX_x[4] = 1;
        for (int i = 0 ; i < 26 ; i++){
            zX_x[i+5] = (zX_x[i+4] + zX_x[i+2] + zX_x[i+1] + zX_x[i+0])%2;
        }
        for (int i = 0; i < 31 ; i++){
            d_zX[i] = 1 - (2*zX_x[i]);
        }

        //initialize m0/m1 pair lookup table
        for(int i = 0 ; i < 168 ; i++){
            int N = i;
            int q_prime = floor(N/30);
            int q = floor( ( N + (q_prime*(q_prime+1)/2) )/30 );
            int m_prime = N + q * (q+1)/2;
            int m0g = m_prime%31;
            int m1g = (m0g + int(floor(m_prime/31))+1 )%31;
            d_v_m0[i] = m0g;
            d_v_m1[i] = m1g;
        }

    }


    void
    mimo_sss_calculator_impl::msg_set_sector_id(pmt::pmt_t msg){
        d_N_id_2 = int(pmt::to_long(msg));
    }

    /*
     * Our virtual destructor.
     */
    mimo_sss_calculator_impl::~mimo_sss_calculator_impl()
    {
    }

    int
    mimo_sss_calculator_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
        gr_complex* in = (gr_complex*) input_items[0];

        if(d_is_locked){
            return noutput_items;
        }

//        std::vector <gr::tag_t> v_id;
//        get_tags_in_range(v_id, 0, nitems_read(0), nitems_read(0)+1, d_key_offset);
//        if (v_id.size() > 0){
//            d_ = int(pmt::to_long(v_id[0].value));
//        }
        if(d_N_id_2 < 0){return 1;}

        // extract the 2 half sss symbols which are interleaved differently by their position within a frame.
        gr_complex** even = new gr_complex*[d_rxant];
        gr_complex** odd  = new gr_complex*[d_rxant];

        for(int rx=0; rx<d_rxant; rx++)
        {
            even[rx] = new gr_complex[31];
            odd[rx] = new gr_complex[31];
            for(int i = 0; i < 31 ; i++){
                  even[rx][i] = in[5 + 2*i + 0 + 72*rx];
                  odd[rx][i]  = in[5 + 2*i + 1 + 72*rx];
            }
        }

        sss_info info = get_sss_info(even, odd, d_N_id_2, d_rxant);
        if(info.N_id_1 < 0){return 1;}

        if(d_max_val_new > d_max_val_old*0.8){
            long offset = 0;
            std::vector <gr::tag_t> v_off;
            get_tags_in_range(v_off,0, nitems_read(0), nitems_read(0)+1, d_key_offset);
            if (v_off.size() > 0){
                offset = pmt::to_long(v_off[0].value);
            }

            d_sss_pos = info.pos;

            if(d_sss_pos == 5){
                offset += 70;
            }
            d_frame_start = (offset-5+140) % 140;

            d_cell_id = 3 * info.N_id_1 + d_N_id_2;

            d_unchanged_id++;
            if(d_unchanged_id > 2){
//                printf("\n%s locked to frame_start = %ld\tabs_pos = %ld\tcell_id = %i\n\n", name().c_str(), d_frame_start, offset, d_cell_id );,3
                printf("\n%s locked to frame_start = %li\tcell_id = %i\n\n", name().c_str(), d_frame_start, d_cell_id );
                publish_frame_start(d_frame_start);
                publish_cell_id(d_cell_id);
                d_is_locked = true;
            }
        }
        else{
            if(d_sss_pos == 0){d_sss_pos = 5;}
            else{d_sss_pos = 0;}
        }

        d_max_val_old = d_max_val_new;


        //delete used arrays
        for(int rx=0; rx<d_rxant; rx++){
            delete[] even[rx];
            delete[] odd[rx];
        }
        delete[] even;
        delete[] odd;

        // Tell runtime system how many output items we produced.
        return 1;
    }

    sss_info
    mimo_sss_calculator_impl::get_sss_info(gr_complex** &even, gr_complex** &odd, int N_id_2, int rxant)
    {
        sss_info info;
        // next 2 sequences depend on N_id_2
        gr_complex c0[31] = {0};
        gr_complex c1[31] = {0};
        for(int i = 0; i < 31 ; i++){
            c0[i] = d_cX[ (i+d_N_id_2  )%31 ];
            c1[i] = d_cX[ (i+d_N_id_2+3)%31 ];
        }

        gr_complex** s0m0 = new gr_complex* [rxant];
        for(int rx=0; rx<rxant; rx++){
            s0m0[rx] = new gr_complex[31];
            for (int i = 0 ; i < 31 ; i++){
                s0m0[rx][i]=even[rx][i]/gr_complex(c0[i]);
            }
        }

        int m0 = calc_m(s0m0, rxant);

        char z1m0[31] = {0};
        for (int i = 0 ; i < 31 ; i++) {
            z1m0[i] = d_zX[ ( i+(m0%8) )%31 ];
        }

        gr_complex** s1m1 = new gr_complex*[rxant];
        for(int rx=0; rx<rxant; rx++){
            s1m1[rx] = new gr_complex[31];
            for (int i = 0 ; i < 31 ; i++){
                s1m1[rx][i] = odd[rx][i] / (c1[i] * gr_complex(z1m0[i]) );
            }
        }
        int m1 = calc_m(s1m1, rxant);
        //printf("m1 = %i\n",m1);

        info.pos = 0;
        if (m0 > m1){
            std::swap(m0, m1);
            info.pos = 5;
        }
        info.N_id_1 = get_N_id_1(m0, m1);

        for(int rx=0; rx<rxant; rx++){
            delete[] s0m0[rx];
            delete[] s1m1[rx];
        }

        delete[] s0m0;
        delete[] s1m1;
        return info;
    }

    int
    mimo_sss_calculator_impl::get_N_id_1(int m0, int m1)
    {
        int N_id_1 = -1;
        for(int i = 0 ; i < 168 ; i++ ){
            if(d_v_m0[i] == m0 && d_v_m1[i] == m1){
                N_id_1 = i;
                break;
            }
        }
        return N_id_1;
    }

    int
    mimo_sss_calculator_impl::calc_m(gr_complex **s0m0, int rxant)
    {
        int mX = -1;
        int N = 62;


        gr_complex** s0 = new gr_complex*[rxant];
        for(int i=0; i<rxant; i++){
            s0[i] = new gr_complex[62];
            memcpy(s0[i], s0m0[i], sizeof(gr_complex) * 31);
        }

        gr_complex sr[62] = {0};
        memcpy(sr, d_sref, sizeof(gr_complex) * 62);

        std::vector<float> x_vec;
        xcorr(x_vec, s0, sr, N, rxant);

        float max = 0;
        int pos = -1;
        for (int i = 0 ; i < 2*N-1 ; i++){
            float mag = x_vec[i];
            if (max < mag){
                max = mag;
                pos = i;
            }
        }

        mX = abs(pos-31-31);

        d_max_val_new = (d_max_val_new + max)/2;
        for(int rx=0; rx<rxant; rx++){
           delete s0[rx];
        }
        delete[] s0;

        return mX;
    }

    // simple correlation between 2 arrays. returns complex value.
    gr_complex
    mimo_sss_calculator_impl::corr(gr_complex *x,gr_complex *y, int len)
    {
        gr_complex val = 0;
        for(int i = 0 ; i < len ; i++){
            val += (x[i]*conj(y[i]) );
        }
        return val;
    }

    // be careful! input arrays must have the same size!
    void
    mimo_sss_calculator_impl::xcorr(std::vector<float> &v, gr_complex **x,gr_complex *y, int len, int rxant)
    {
        int N = len;
        float val;

        for (int i = 0 ; i < 2 * N - 1 ; i++){
            val=0;
            if(i < N){
                for(int rx=0; rx<rxant; rx++)
                    val += abs(corr(x[rx]+(N-1-i),y,i+1));
                v.push_back(val);
            }
            else{
                for(int rx=0; rx<rxant; rx++)
                    val += abs(corr(x[rx],y+(i-N),2*N-1-i));
                v.push_back(val);
            }
        }
    }

    void
    mimo_sss_calculator_impl::publish_cell_id(int cell_id)
    {
        printf("%s\t\tpublish_cell_id %i\n", name().c_str(), cell_id );
        pmt::pmt_t msg = pmt::from_long((long)cell_id) ;
        message_port_pub( d_port_cell_id, msg );
    }

    void
    mimo_sss_calculator_impl::publish_frame_start(long frame_start)
    {
//        printf("%s\t\tpublish_frame_start %ld\n", name().c_str(), frame_start );
        pmt::pmt_t msg = pmt::from_long(frame_start) ;
        message_port_pub( d_port_frame_start, msg );
    }


  } /* namespace lte */
} /* namespace gr */

