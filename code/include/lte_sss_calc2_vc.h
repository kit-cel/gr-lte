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

#ifndef INCLUDED_LTE_SSS_CALC2_VC_H
#define INCLUDED_LTE_SSS_CALC2_VC_H

#include <lte_api.h>
#include <gr_sync_block.h>
#include <lte_sss_tagging2_vcvc.h>
#include <lte_cell_id_daemon.h>

class lte_sss_calc2_vc;
typedef boost::shared_ptr<lte_sss_calc2_vc> lte_sss_calc2_vc_sptr;
typedef boost::shared_ptr<lte_cell_id_daemon> daemon_sptr;
typedef boost::shared_ptr<lte_sss_tagging2_vcvc> tag2_sptr;

LTE_API lte_sss_calc2_vc_sptr lte_make_sss_calc2_vc (tag2_sptr &tag, daemon_sptr &daemon, int fftl);

/*!
 * \brief This is the SSS Calculation block for OFDM-symbol based operations
 *
 */
class LTE_API lte_sss_calc2_vc : public gr_sync_block
{
	friend LTE_API lte_sss_calc2_vc_sptr lte_make_sss_calc2_vc (tag2_sptr &tag, daemon_sptr &daemon, int fftl);

	lte_sss_calc2_vc (tag2_sptr &tag, daemon_sptr &daemon, int fftl);

 public:
	~lte_sss_calc2_vc ();

	int get_cell_id(){return d_cell_id;}
	long get_frame_start(){return d_frame_start;}


	int work (int noutput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items);

 private:
    int d_N_id_2;
    int d_cell_id;
    int d_fftl;
    int d_slotl;
    char d_cX[31];
    gr_complex d_sref[62];
    char d_zX[31];
    int d_v_m0[168];
    int d_v_m1[168];
    float d_max_val_new;
    float d_max_val_old;
    int d_sss_pos;
    tag2_sptr d_tag;
    daemon_sptr d_daemon;
    long d_frame_start;
    bool d_is_locked;
    int d_unchanged_id;

    long d_offset;
    long d_value;

    //method definitions
    void initialize ();
    int calc_m(gr_complex *s0m0);
    gr_complex corr(gr_complex *x,gr_complex *y, int len);
    void xcorr(std::vector<gr_complex> &v, gr_complex *x,gr_complex *y, int len);

    std::vector<float> corr_vec;

};

#endif /* INCLUDED_LTE_SSS_CALC2_VC_H */

