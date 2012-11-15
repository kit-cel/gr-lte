/* -*- c++ -*- */
/* 
 * Copyright 2012 Communications Engineering Lab (CEL) / Karlsruhe Institute of Technology (KIT)
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

#ifndef INCLUDED_LTE_LINEAR_OFDM_ESTIMATOR_VCVC_H
#define INCLUDED_LTE_LINEAR_OFDM_ESTIMATOR_VCVC_H

#include <lte_api.h>
#include <gr_sync_block.h>
#include <fftw3.h>
#include <volk/volk.h>

class lte_linear_OFDM_estimator_vcvc;
typedef boost::shared_ptr<lte_linear_OFDM_estimator_vcvc> lte_linear_OFDM_estimator_vcvc_sptr;

LTE_API lte_linear_OFDM_estimator_vcvc_sptr lte_make_linear_OFDM_estimator_vcvc (int N_rb_dl); //int cell_id,

/*!
 * \brief <+description+>
 *
 */
class LTE_API lte_linear_OFDM_estimator_vcvc : public gr_sync_block
{
	friend LTE_API lte_linear_OFDM_estimator_vcvc_sptr lte_make_linear_OFDM_estimator_vcvc (int N_rb_dl);//int cell_id,

	lte_linear_OFDM_estimator_vcvc (int N_rb_dl);//int cell_id, 

 public:
	~lte_linear_OFDM_estimator_vcvc ();

    typedef gr_complex* rs_mat[2][40];
    typedef gr_complex* ce_vec[7];
    
    void set_cell_id(int id);

	int work (int noutput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items);
		
 private:
    static const gr_complex d_C_I;
    int d_cell_id;
    int d_N_rb_dl;
    int d_sym_num;
    bool d_first_call;
    int d_work_call;
    gr_complex* d_ce_vec1;
    gr_complex* d_ce_vec2;
    rs_mat d_rs_matrix;
    pmt::pmt_t d_key;
	pmt::pmt_t d_tag_id;
	
	gr_complex* d_v4; //d_v4(gr_complex*)fftwf_malloc(sizeof(gr_complex)*len);
    gr_complex* d_v7; //d_v7
    gr_complex* d_ce11;
    gr_complex* d_ce12;
    gr_complex* d_ce41;
    gr_complex* d_ce42;
    gr_complex* d_ce71;
    gr_complex* d_ce72;
    gr_complex* d_divider1_4;
    gr_complex* d_divider1_3;
    gr_complex* d_diff1;
    gr_complex* d_diff2;
    ce_vec d_erg1;
    ce_vec d_erg2;
    
    void calc_eq_freq_domain(gr_complex* ce, gr_complex* svec, gr_complex* rvec);
    void pn_seq_generator(char* c, int len, int cinit);
    void rs_generator(gr_complex* r, int ns,int l,int cell_id,int Ncp);
    gr_complex* rs_mapper(int N_rb_dl,int ns,int l,int cell_id,int Ncp,int p);
};

#endif /* INCLUDED_LTE_LINEAR_OFDM_ESTIMATOR_VCVC_H */

