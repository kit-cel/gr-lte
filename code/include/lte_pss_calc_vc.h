/* -*- c++ -*- */
/* 
 * Copyright 2012 <+YOU OR YOUR COMPANY+>.
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
//#include <boost/numeric/ublas/vector.hpp>
//#include <boost/numeric/ublas/vector_proxy.hpp>
//#include <boost/numeric/ublas/matrix.hpp>
//#include <boost/numeric/ublas/io.hpp>
//#define NDEBUG



class lte_pss_calc_vc;
typedef boost::shared_ptr<lte_pss_calc_vc> lte_pss_calc_vc_sptr;
//typedef boost::numeric::ublas::vector<gr_complex> ucvector;

LTE_API lte_pss_calc_vc_sptr lte_make_pss_calc_vc (boost::shared_ptr<lte_pss_tagging_cc> &tag, boost::shared_ptr<lte_pss_selector_cvc> &sel, int fftl);

/*!
 * \brief <+description+>
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
	gr_complex d_chu0[62];
	gr_complex d_chu1[62];
	gr_complex d_chu2[62];
	//ucvector d_uchu0;
    //ucvector d_uchu1;
    //ucvector d_uchu2;
	
	std::vector<float> d_corr_vec;
	
	
	
	void zc(gr_complex *zc, int cell_id);
	bool find_pss_symbol(gr_complex *chuX);
	
	// Functions to calculate correlations
	gr_complex corr(gr_complex *x, gr_complex *y, int len);
	gr_complex corr(gr_complex *res, gr_complex *x, gr_complex *y, int len);
	//void xcorr(std::vector<gr_complex> &v, gr_complex *x,gr_complex *y, int len);
	void cxcorr(std::vector<gr_complex> &v, gr_complex *x,gr_complex *y, int len);
	//void cxcorr(gr_complex *v, gr_complex *x,gr_complex *y, int len);
	
	void max_pos(float &max, int &pos, gr_complex *x,gr_complex *y, int len);
	
	//uBlas version of correlation functions (about 6 times slower)
	//gr_complex corr(ucvector &x,ucvector &y);
	//void xcorr(std::vector<gr_complex> &v, ucvector &x,ucvector &y);
	
};

#endif /* INCLUDED_LTE_PSS_CALC_VC_H */

