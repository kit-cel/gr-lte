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

#ifndef INCLUDED_LTE_mimo_pss_coarse_sync_IMPL_H
#define INCLUDED_LTE_mimo_pss_coarse_sync_IMPL_H

#include <lte/mimo_pss_coarse_sync.h>

namespace gr {
  namespace lte {

    class mimo_pss_coarse_sync_impl : public mimo_pss_coarse_sync
    {
     private:
        static const gr_complex d_C_I;
        static const float d_PI;
	
	static const int d_TIME_HYPO=9600;
	static const int d_CORRL=128;	

	int d_syncl;
        int d_N_id_2;
	int d_work_call;
	int d_posmax;
	float d_max;

        pmt::pmt_t d_port_coarse_pss;
        pmt::pmt_t d_port_N_id_2;

        gr_complex d_pss0_t[128];
        gr_complex d_pss1_t[128];
        gr_complex d_pss2_t[128];
        gr_complex d_pss012_t[128];
	float d_result[9600];

	gr_complex *d_a;	
    
        void zc(gr_complex *zc, int cell_id);	//generate Zadoff-Chu Sequenz
        void gen_pss_t(gr_complex *pss_t, int cell_id);
        void prepare_corr_vecs();

	float diff_corr(const gr_complex* x,const gr_complex* y);
	float diff_corr2(const gr_complex* x1, const gr_complex* x2, const gr_complex* y);
	int calc_N_id_2(const gr_complex* in1, const gr_complex* in2, const int &mpos);

     public:
      mimo_pss_coarse_sync_impl(int syncl);
      ~mimo_pss_coarse_sync_impl();    

      //void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      // Where all the action really happens
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_mimo_pss_coarse_sync_IMPL_H */

