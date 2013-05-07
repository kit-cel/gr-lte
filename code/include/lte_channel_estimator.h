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

#ifndef INCLUDED_LTE_CHANNEL_ESTIMATOR_H
#define INCLUDED_LTE_CHANNEL_ESTIMATOR_H

#include <lte_api.h>
#include <gr_sync_block.h>

#include <fftw3.h>
#include <volk/volk.h>
#include <cmath>
#include <cstdio>

class lte_channel_estimator;
typedef boost::shared_ptr<lte_channel_estimator> lte_channel_estimator_sptr;
typedef gr_complex* rs_matrix[2][40];

LTE_API lte_channel_estimator_sptr lte_make_channel_estimator (int N_rb_dl);

/*!
 * \brief Calculate channel estimation
 * Takes in the samples for one slot and calculates the channel estimates for antenna ports 1 and 2
 */
class LTE_API lte_channel_estimator : public gr_sync_block
{
	friend LTE_API lte_channel_estimator_sptr lte_make_channel_estimator (int N_rb_dl);

	lte_channel_estimator (int N_rb_dl);



 public:
	~lte_channel_estimator ();

    void set_cell_id(int cell_id);
    void set_cell_id_msg(pmt::pmt_t msg);

    std::vector<int> get_pn_sequence(int len, int cinit);
    std::vector<gr_complex> get_rs_sequence(int ns,int l,int cell_id,int Ncp);
    std::vector<std::vector<gr_complex> > get_frame_rs_symbols(int N_rb_dl, int cell_id, int Ncp);
    std::vector<std::vector<int> > get_frame_rs_positions(int p);

	int work (int noutput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items);

 private:
    static const gr_complex d_C_I;
    int d_cell_id;
    int d_N_rb_dl;
    int d_sym_num;
    int d_work_call;
    pmt::pmt_t d_key;
	pmt::pmt_t d_tag;
	rs_matrix d_rs_matrix; // Hold arrays with RS symbols for each OFDM symbol

    // hold already calculated values for next slot
	float* d_est_mag[2];
	float* d_est_phase[2];

    //class member arrays for work function
    gr_complex* d_rx_rs_symbol_vectors[3];
    gr_complex* d_est_slot_p0[8];
    gr_complex* d_est_slot_p1[8];

	// methods used during channel estimation
    void estimate_ofdm_symbol(float* mag_est_vec, float* phase_est_vec, gr_complex* symbol_vec, int ns, int l, int p);
    // and there class member array pointers.
    gr_complex* d_rx_rs;
    gr_complex* d_div_rx_rs;
    float* d_div_mag;
    float* d_div_phase;

    // method to estimate symbols in 1 slot.
    void estimate_slot_symbols(gr_complex* est_slot[8], gr_complex* rx_rs_symbol_vectors[3], int ns, int p);
    // and there class member array pointers
    float* d_est_slot_mag[8];
    float* d_est_slot_phase[8];

    // Convenience functions
    int inline calc_offset(int p, int l, int ns);
    void inline phase_bound(float* phase_vec, int len);
    void inline phase_bound_vector(float* phase_vec);
    void inline phase_bound_between_vectors(float* first, float* last);
    float* d_phase_bound_vector;
    void inline interpolate(float* interp_vals, float first_val, float last_val, int steps);
    void inline interpolate_vector(float** interp_vals, float* first_val, float* last_val, int steps);
    float* d_diff_vector; //used in interpolate_vector
    float* d_div_vector;
    gr_complex inline mag_phase_to_complex(float mag, float phase);

    //functions used for setup
    void pn_seq_generator(char* c, int len, int cinit);
    void rs_generator(gr_complex* r, int ns,int l,int cell_id,int Ncp);
    void generate_rs_frame(rs_matrix &mat, int cell_id, int Ncp);
    inline void get_used_rs_symbols(gr_complex* rs, int N_rb_dl, int ns, int l, int cell_id, int Ncp);
    gr_complex* rs_mapper(int N_rb_dl,int ns,int l,int cell_id,int Ncp,int p);



};

#endif /* INCLUDED_LTE_CHANNEL_ESTIMATOR_H */

