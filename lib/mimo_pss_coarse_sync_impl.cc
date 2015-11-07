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
#include "mimo_pss_coarse_sync_impl.h"

#include <cstdio>
#include <cmath>
#include <volk/volk.h>
#include "lte/pss.h"

namespace gr {
  namespace lte {

    mimo_pss_coarse_sync::sptr
    mimo_pss_coarse_sync::make(int syncl, int rxant)
    {
      return gnuradio::get_initial_sptr(new mimo_pss_coarse_sync_impl(syncl, rxant));
    }

    /*
     * The private constructor
     */
    mimo_pss_coarse_sync_impl::mimo_pss_coarse_sync_impl(int syncl, int rxant) :
            gr::sync_block("mimo_pss_coarse_sync", gr::io_signature::make(1, 8, sizeof(gr_complex)),
                           gr::io_signature::make(0, 0, 0)), d_syncl(syncl), d_rxant(rxant),
            d_work_call(0), d_posmax(0), d_max(0)
    {

      //make sure that there are enough input items for
      //a 64 point correalation at position 4800 (index=4799)
      set_output_multiple(d_TIME_HYPO + d_CORRL - 1);

      const size_t alig = volk_get_alignment();
      d_a = (gr_complex*) volk_malloc(sizeof(gr_complex) * 4, alig);

      for(int rx = 0; rx < d_rxant; rx++){
        gr_complex* p = (gr_complex*) volk_malloc(
            sizeof(gr_complex) * (d_TIME_HYPO * syncl + d_CORRL - 1), alig);
        d_buffer.push_back(p);
      }

      // Declare all the message ports.
      d_port_N_id_2 = pmt::string_to_symbol("N_id_2");
      message_port_register_out(d_port_N_id_2);
      d_port_coarse_pos = pmt::string_to_symbol("coarse_pos");
      message_port_register_out(d_port_coarse_pos);
      d_port_control = pmt::string_to_symbol("control");
      message_port_register_out(d_port_control);

      //binary representation of 0.0 is 0
      memset(d_result, 0, sizeof(float) * d_TIME_HYPO);

      prepare_corr_vecs();
    }

    /*
     * Our virtual destructor.
     */
    mimo_pss_coarse_sync_impl::~mimo_pss_coarse_sync_impl()
    {
      volk_free(d_a);
    }

    int
    mimo_pss_coarse_sync_impl::work(int noutput_items, gr_vector_const_void_star &input_items,
                                    gr_vector_void_star &output_items)
    {

      //deactivate block after synclen ist reached
      if(d_work_call == d_syncl) return noutput_items;

      for(int rx = 0; rx < d_rxant; rx++){
        memcpy(d_buffer[rx] + d_work_call * d_TIME_HYPO, input_items[rx],
               sizeof(gr_complex) * (d_TIME_HYPO + d_CORRL - 1));
      }

      //printf("---BEGIN coarse timing---\n");

      for(int d = 0; d < d_TIME_HYPO; d++){
        for(int rx = 0; rx < d_rxant; rx++){
          const gr_complex* in = (gr_complex*) input_items[rx];
          d_result[d] += diff_corr(in + d, d_pss012_t, d_CORRL);
        }

        if(d_result[d] > d_max){
          d_max = d_result[d];
          d_posmax = d;
          printf("%s:new coarse timing max found: val=%f \t pos=%i\t syncloop=%i\n", name().c_str(),
                 d_max, d_posmax, d_work_call + 1);
        }
      }

      //printf("---END coarse timing---\n");

      d_work_call++;

      if(d_work_call != d_syncl) return d_TIME_HYPO;

      //obtain N_id_2 after coarse timinig sync
      d_N_id_2 = calc_N_id_2(d_buffer, d_posmax);

      //publish results
      message_port_pub(d_port_N_id_2, pmt::from_long((long) d_N_id_2));
      message_port_pub(d_port_coarse_pos, pmt::from_long((long) d_posmax));

      //stop coarse calculation
      message_port_pub(d_port_control, pmt::PMT_T);

      printf("\n%s:found N_id_2=%i\n", name().c_str(), d_N_id_2);
      printf("%s:coarse pss-pos=%i\n", name().c_str(), d_posmax);

      //Tell runtime system how many output items we produced
      return noutput_items;
    }

    int
    mimo_pss_coarse_sync_impl::calc_N_id_2(std::vector<gr_complex*> &buffer, int &mpos)
    {

      float max0 = 0;
      float max1 = 0;
      float max2 = 0;

      for(int i = 0; i < d_syncl; i++){
        for(int rx = 0; rx < d_rxant; rx++){
          gr_complex* pss = buffer[rx] + mpos + i * d_TIME_HYPO;
          max0 += diff_corr(pss, d_pss0_t, d_CORRL);
          max1 += diff_corr(pss, d_pss1_t, d_CORRL);
          max2 += diff_corr(pss, d_pss2_t, d_CORRL);
        }
      }

      printf("Nid2 correlation values: id0:%f\tid1:%f\tid2:%f\t\n", max0, max1, max2);

      int id = 0;
      float max = max0;

      if(max1 > max){
        id = 1;
        max = max1;
      }
      if(max2 > max){
        id = 2;
      }

      return id;
    }

    void
    mimo_pss_coarse_sync_impl::prepare_corr_vecs()
    {
      //init pss sequences in time domain (conj for correlations)
      pss::gen_conj_pss_t(d_pss0_t, 0, d_CORRL);
      pss::gen_conj_pss_t(d_pss1_t, 1, d_CORRL);
      pss::gen_conj_pss_t(d_pss2_t, 2, d_CORRL);

      //add pss for correlation
      for(int i = 0; i < d_CORRL; i++){
        d_pss012_t[i] = d_pss0_t[i] + d_pss1_t[i] + d_pss2_t[i];
      }

    }

//calculate differential correlation, 4parts, returns absolute value
    float
    mimo_pss_coarse_sync_impl::diff_corr(const gr_complex* x, const gr_complex* y, int len)
    {
      int len4 = len / 4;
      for(int i = 0; i < 4; i++){
        volk_32fc_x2_dot_prod_32fc(d_a + i, x + len4 * i, y + len4 * i, len4);
      }
      return abs(d_a[0] * conj(d_a[1]) + d_a[1] * conj(d_a[2]) + d_a[2] * conj(d_a[3]));
    }

  } /* namespace lte */
} /* namespace gr */

