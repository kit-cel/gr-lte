/* -*- c++ -*- */
/*
 * Copyright 2013 <+YOU OR YOUR COMPANY+>.
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


#ifndef INCLUDED_LTE_CHANNEL_ESTIMATOR_VCVC_H
#define INCLUDED_LTE_CHANNEL_ESTIMATOR_VCVC_H

#include <lte_api.h>
#include <gr_sync_block.h>

#include <fftw3.h>
#include <volk/volk.h>
#include <cmath>
#include <cstdio>
#include <string>

class lte_channel_estimator_vcvc;

typedef boost::shared_ptr<lte_channel_estimator_vcvc> lte_channel_estimator_vcvc_sptr;

LTE_API lte_channel_estimator_vcvc_sptr lte_make_channel_estimator_vcvc (int subcarriers,
                                                                        int N_ofdm_symbols,
                                                                        std::string tag_key,
                                                                        std::string msg_buf_name,
                                                                        const std::vector<std::vector<int> > &pilot_carriers,
                                                                        const std::vector<std::vector<gr_complex> > &pilot_symbols);

/*!
 * \brief Channel Estimator for OFDM
 * \ingroup lte
 *
 */
class LTE_API lte_channel_estimator_vcvc : public gr_sync_block
{
 private:
	friend LTE_API lte_channel_estimator_vcvc_sptr lte_make_channel_estimator_vcvc (int subcarriers,
                                                                                 int N_ofdm_symbols,
                                                                                 std::string tag_key,
                                                                                 std::string msg_buf_name,
                                                                                 const std::vector<std::vector<int> > &pilot_carriers,
                                                                                 const std::vector<std::vector<gr_complex> > &pilot_symbols);

	lte_channel_estimator_vcvc(int subcarriers,
                            int N_ofdm_symbols,
                            std::string tag_key,
                            std::string msg_buf_name,
                            const std::vector<std::vector<int> > &pilot_carriers,
                            const std::vector<std::vector<gr_complex> > &pilot_symbols);

    static const gr_complex d_C_I;
    int d_subcarriers;
    int d_N_ofdm_symbols;
    int d_sym_num;
    int d_work_call;

    pmt::pmt_t d_key;

    std::vector<std::vector<int> > d_pilot_carriers;
    std::vector<gr_complex* > d_pilot_symbols;
    void initialize_volk_vectors();
    inline int get_max_pilot_number();
    inline int get_nsyms_in_frame();
    inline int get_next_sym_with_pilots(int sym_num);

    // Calculate phase and magnitude distortion for OFDM symbol with
    void estimate_ofdm_symbol(float* mag_est_vec, float* phase_est_vec,
                              gr_complex* symbol_vec,
                              std::vector<int> pilot_pos, gr_complex* pilot_sym);
    gr_complex* d_rx_rs;
    float* d_diff_mag;
    float* d_diff_phase;

    std::vector<gr_complex* > calculate_channel_estimates(const gr_complex* in_rx, int sym_num, int nitems);

    inline void calculate_mag_phase_diff(float* diff_mag, float* diff_phase,
                                         gr_complex* rx_rs, gr_complex* pilot_sym,
                                         int num_pilots);
    gr_complex* d_diff_rx_rs;

    inline void extract_pilots(gr_complex* b_vec, gr_complex* a_vec, std::vector<int> pilot_pos);

    void inline phase_bound_diff(float* phase_vec, int len);
    void inline phase_bound_abs(float* phase_vec, int len);

    void inline interpolate_ofdm_symbol(float* b_vec, float* a_vec, std::vector<int> pilot_pos);
    void inline interpolate(float* interp_vals, float first_val, float last_val, int steps);

    void inline vector_mag_phase_to_complex(gr_complex* c_vec, float* m_vec, float* p_vec, int len);
    gr_complex inline mag_phase_to_complex(float mag, float phase);




 public:
  ~lte_channel_estimator_vcvc();

	// Where all the action really happens
	int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);

    void set_pilot_map(const std::vector<std::vector<int> > &pilot_carriers,
                    const std::vector<std::vector<gr_complex> > &pilot_symbols);
};

#endif /* INCLUDED_LTE_CHANNEL_ESTIMATOR_VCVC_H */

