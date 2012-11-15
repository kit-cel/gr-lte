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

#ifndef INCLUDED_LTE_FREQ_ESTIMATE_C_H
#define INCLUDED_LTE_FREQ_ESTIMATE_C_H

#include <lte_api.h>
#include <gr_sync_block.h>
#include <gr_sig_source_c.h>
#include <fftw3.h>
#include <volk/volk.h>

class lte_freq_estimate_c;
typedef boost::shared_ptr<lte_freq_estimate_c> lte_freq_estimate_c_sptr;

LTE_API lte_freq_estimate_c_sptr lte_make_freq_estimate_c (boost::shared_ptr<gr_sig_source_c> &sig, int fftl);

/*!
 * \brief <+description+>
 *
 */
class LTE_API lte_freq_estimate_c : public gr_sync_block
{
	friend LTE_API lte_freq_estimate_c_sptr lte_make_freq_estimate_c (boost::shared_ptr<gr_sig_source_c> &sig, int fftl);

	lte_freq_estimate_c (boost::shared_ptr<gr_sig_source_c> &sig, int fftl);

 public:
	~lte_freq_estimate_c ();


	int work (int noutput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items);
	
	// This function does only exist for test purposes!	
	//std::vector<float> get_f_vec(){return f_vec;}
		
 private:
    // basic attributes
    boost::shared_ptr<gr_sig_source_c> d_sig;
    int d_fftl;
    int d_cpl;
    int d_cpl0;
    int d_slotl;
    int d_samp_rate;
    int d_offset;
    gr_complex* d_buffer;
    
    float d_f_av;
    int d_samp_num;
    int d_work_call;
    
    //methods for further calculations
    void calc_f_off_av();
    
    gr_complex corr(gr_complex *res, gr_complex *x, gr_complex *y, int len);
    
    // for test purposes
    //std::vector<float> f_vec;
		
};

#endif /* INCLUDED_LTE_FREQ_ESTIMATE_C_H */

