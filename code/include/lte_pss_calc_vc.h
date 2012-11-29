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

#ifndef INCLUDED_LTE_PSS_CALC_VC_H
#define INCLUDED_LTE_PSS_CALC_VC_H

#include <lte_api.h>
#include <gr_sync_block.h>
#include <lte_pss_tagging_cc.h>
#include <lte_pss_selector_cvc.h>
#include <lte_correlation.h>


class lte_pss_calc_vc;
typedef boost::shared_ptr<lte_pss_calc_vc> lte_pss_calc_vc_sptr;

LTE_API lte_pss_calc_vc_sptr lte_make_pss_calc_vc (boost::shared_ptr<lte_pss_tagging_cc> &tag, boost::shared_ptr<lte_pss_selector_cvc> &sel, int fftl);

/*!
 * \brief PSS Calculator Block
 * Calculate cross-correlation and determine used zadoff-chu sequence.
 * Determine Cell ID number with detected Cell ID number.
 *
 */
class LTE_API lte_pss_calc_vc : public gr_sync_block
{
	friend LTE_API lte_pss_calc_vc_sptr lte_make_pss_calc_vc (boost::shared_ptr<lte_pss_tagging_cc> &tag, boost::shared_ptr<lte_pss_selector_cvc> &sel, int fftl);

	lte_pss_calc_vc (boost::shared_ptr<lte_pss_tagging_cc> &tag, boost::shared_ptr<lte_pss_selector_cvc> &sel, int fftl);

 public:
	~lte_pss_calc_vc ();

    std::vector<float> get_corr_vec(){return d_corr_vec;}

	int work (int noutput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items);
		
 private:
    static const gr_complex d_C_I;
    static const float d_PI;
    boost::shared_ptr<lte_pss_tagging_cc> d_tag;
    boost::shared_ptr<lte_pss_selector_cvc> d_sel;
    int d_fftl;
	int d_cpl;
	int d_cpl0;
	int d_slotl;
	int d_N_id_2;
	long d_half_frame_start;
	float d_corr_val;
	int d_lock_count;
	bool d_is_locked;
    gr_complex d_chu0[63];
	gr_complex d_chu1[63];
	gr_complex d_chu2[63];
	
	std::vector<float> d_corr_vec;
	
	void zc(gr_complex *zc, int cell_id); // used to generate Zadoff-Chu sequences
	bool find_pss_symbol(gr_complex *chuX); // prepares the calculation stuff etc.
	void max_pos(float &max, int &pos, gr_complex *x,gr_complex *y, int len); //finds maximum of one correlation
	
	// attributes for correlation
	lte_correlation *d_correlator;
	gr_complex *d_corr_in1;
	gr_complex *d_corr_in2;
	gr_complex *d_corr_out;	
};

#endif /* INCLUDED_LTE_PSS_CALC_VC_H */

