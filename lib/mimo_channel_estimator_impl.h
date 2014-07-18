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


#ifndef INCLUDED_LTE_MIMO_CHANNEL_ESTIMATOR_IMPL_H
#define INCLUDED_LTE_MIMO_CHANNEL_ESTIMATOR_IMPL_H

#include <lte/mimo_channel_estimator.h>

namespace gr {
  namespace lte {

    class mimo_channel_estimator_impl : public mimo_channel_estimator
    {
     private:
		static const gr_complex d_C_I;
		int d_subcarriers;
		int d_n_frame_syms;
		int d_last_calced_sym;
		int d_rxant;
		pmt::pmt_t d_key;
		pmt::pmt_t d_msg_buf;

		inline void handle_msg(pmt::pmt_t msg);
		inline void msg_extract_poss(std::vector<std::vector<int> > &pilot_carriers, pmt::pmt_t poss);
		inline void msg_extract_vals(std::vector<std::vector<gr_complex> > &pilot_symbols, pmt::pmt_t vals);

		std::vector< std::vector<int> > d_pilot_carriers;
		std::vector< gr_complex* > d_pilot_symbols;

		// These methods do all the setup stuff.
		inline void init_pilot_symbol_arrays(const std::vector<std::vector<gr_complex> > &pilot_symbols, int n_frame_syms, int max_pilots);
		inline void initialize_volk_vectors(int max_pilots, int subcarriers, int n_frame_syms);
		inline void init_pilot_dependend_volk_vectors(int max_pilots);
		inline void init_subcarrier_dependend_volk_vectors(int subcarriers);
		inline void init_estimates_store_volk_vectors(int subcarriers, int n_frame_syms);

		std::vector< std::vector<gr_complex* > > d_estimates;
		std::vector< std::vector<float* > > d_mag_estimates;
		std::vector< std::vector<float* > > d_phase_estimates;

		inline int get_max_pilot_number(const std::vector< std::vector<int> > &pilot_carriers);
		inline int get_nsyms_in_frame(const std::vector< std::vector<int> > &pilot_carriers);

		// These methods are resposnible for control and data moves
		inline int get_sym_num_from_tags(std::vector <gr::tag_t> v_b);
		inline void copy_estimates_to_out_buf(gr_complex* out, int sym_num, int processed_items);

		// This method does all the estimation work
		int calculate_channel_estimates(const gr_complex* in_rx, int sym_num, int nitems);
		inline void calculate_ofdm_symbols_with_pilots(const gr_complex* in_rx, int first_sym, int processable_items, int rx);
		gr_complex* d_rx_vec;
		inline void calculate_interpolated_ofdm_symbols(int first_sym, int processable_items, int rx);
		inline void processed_items_to_complex(int first_sym, int processed_items, int rx);
		inline int get_processable_items(int sym_num, int nitems);
		inline int get_last_processable_sym(int first_sym, int nitems);

		// Calculate phase and magnitude distortion for OFDM symbol with
		void estimate_ofdm_symbol(float* mag_est_vec, float* phase_est_vec,
								  gr_complex* symbol_vec,
								  std::vector<int> pilot_pos, gr_complex* pilot_sym);
		gr_complex* d_rx_rs;
		float* d_diff_mag;
		float* d_diff_phase;

		inline void calculate_mag_phase_diff(float* diff_mag, float* diff_phase,
											 gr_complex* rx_rs, gr_complex* pilot_sym,
											 int num_pilots);
		gr_complex* d_diff_rx_rs;

		inline void extract_pilots(gr_complex* b_vec, gr_complex* a_vec, std::vector<int> pilot_pos);

		void inline phase_bound_diff(float* phase_vec, int len);
		void inline phase_bound_abs(float* phase_vec, int len);
		void inline phase_bound_between_pilot_vectors(int first_sym, int processable_items, int rx);
		void inline phase_bound_between_vectors(float* first, float* last);
		float* d_phase_bound_vector;

		void inline interpolate_between_vectors(std::vector<float*> &estimates, int previous_sym, int current_sym);
		float* d_diff_vector;
		float* d_div_vector;

		void inline interpolate_ofdm_symbol(float* b_vec, float* a_vec, std::vector<int> pilot_pos);
		void inline interpolate(float* interp_vals, float first_val, float last_val, int steps);

		void inline vector_mag_phase_to_complex(gr_complex* c_vec, float* m_vec, float* p_vec, int len);
		gr_complex inline mag_phase_to_complex(float mag, float phase);


     public:
      mimo_channel_estimator_impl(int rxant, int subcarriers,
			std::string tag_key,
			std::string msg_buf_name,
			const std::vector<std::vector<int> > &pilot_carriers,
			const std::vector<std::vector<gr_complex> > &pilot_symbols);
      ~mimo_channel_estimator_impl();

      // Where all the action really happens
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);

	  void set_pilot_map(const std::vector<std::vector<int> > &pilot_carriers,
							const std::vector<std::vector<gr_complex> > &pilot_symbols);

      std::vector<std::vector<int> > get_pilot_carriers(){ return d_pilot_carriers; }

    };

  } // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_MIMO_CHANNEL_ESTIMATOR_IMPL_H */

