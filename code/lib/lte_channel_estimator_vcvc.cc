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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_io_signature.h>
#include "lte_channel_estimator_vcvc.h"

lte_channel_estimator_vcvc_sptr
lte_make_channel_estimator_vcvc (int subcarriers,
                               int N_ofdm_symbols,
                               std::string tag_key,
                               std::string msg_buf_name,
                               const std::vector<std::vector<int> > &pilot_carriers,
                               const std::vector<std::vector<gr_complex> > &pilot_symbols)
{
	return gnuradio::get_initial_sptr (new lte_channel_estimator_vcvc(subcarriers, N_ofdm_symbols, tag_key, msg_buf_name, pilot_carriers, pilot_symbols));
}


/*
 * The private constructor
 */
lte_channel_estimator_vcvc::lte_channel_estimator_vcvc (int subcarriers,
                                                    int N_ofdm_symbols,
                                                    std::string tag_key,
                                                    std::string msg_buf_name,
                                                    const std::vector<std::vector<int> > &pilot_carriers,
                                                    const std::vector<std::vector<gr_complex> > &pilot_symbols)
  : gr_sync_block ("channel_estimator_vcvc",
		   gr_make_io_signature(1, 1, sizeof(gr_complex) * subcarriers ),
		   gr_make_io_signature(1, 1, sizeof(gr_complex) * subcarriers )),
		   d_subcarriers(subcarriers),
		   d_N_ofdm_symbols(N_ofdm_symbols)
{
    d_key=pmt::pmt_string_to_symbol(tag_key); // specify key of tag.

    message_port_register_in(pmt::mp(msg_buf_name));
    //set_msg_handler(pmt::mp(tag_key), boost::bind(&lte_channel_estimator::set_cell_id_msg, this, _1));

    set_pilot_map(pilot_carriers, pilot_symbols);

    initialize_volk_vectors();

}


/*
 * Our virtual destructor.
 */
lte_channel_estimator_vcvc::~lte_channel_estimator_vcvc()
{
	for(int i = 0; i < d_pilot_symbols.size(); i++){
        fftwf_free(d_pilot_symbols[i]);
	}
	fftwf_free(d_rx_rs);
}


int
lte_channel_estimator_vcvc::work(int noutput_items,
		  gr_vector_const_void_star &input_items,
		  gr_vector_void_star &output_items)
{
	const gr_complex *in = (const gr_complex *) input_items[0];
	gr_complex *out = (gr_complex *) output_items[0];

    int slot_number = 5;
    std::vector <gr_tag_t> v_b;
    get_tags_in_range(v_b, 0, nitems_read(0), nitems_read(0)+noutput_items, d_key);
    for(int i = 0; i < v_b.size() ; i++){
        long tag_offset     = v_b[i].offset;
        int value           = int(pmt::pmt_to_long(v_b[i].value) );
        //printf("%s\t%i\t%ld\n", name().c_str(), value, tag_offset);
    }


    printf("work call\n");
    float* mag_est_vec = (float*)fftwf_malloc( sizeof(float)*d_subcarriers );
    float* phase_est_vec = (float*)fftwf_malloc( sizeof(float)*d_subcarriers );
    gr_complex* rx_vec = (gr_complex*)fftwf_malloc( sizeof(gr_complex) * d_subcarriers);
    memcpy(rx_vec,in, sizeof(gr_complex) * d_subcarriers);
    estimate_ofdm_symbol(mag_est_vec, phase_est_vec, rx_vec, d_pilot_carriers[0], d_pilot_symbols[0]);

    for(int i = 0; i < d_subcarriers; i++){
        printf("value = %+1.2f %+1.2fj\tmag = %+1.2f\t\tphase = %+1.2f\n", rx_vec[i].real(), rx_vec[i].imag(), mag_est_vec[i], phase_est_vec[i]);
    }

	// Tell runtime system how many output items we produced.
	return noutput_items;
}

// Estimate channel for 1 OFDM symbol with RS symbols
// The calculated values are seperated into magnitude and phase.
void
lte_channel_estimator_vcvc::estimate_ofdm_symbol(float* mag_est_vec, float* phase_est_vec,
            gr_complex* symbol_vec,
            std::vector<int> pilot_pos, gr_complex* pilot_sym)
{
    extract_pilots(d_rx_rs, symbol_vec, pilot_pos);
    int num_pilots = pilot_pos.size();
    calculate_mag_phase_diff(d_diff_mag, d_diff_phase, d_rx_rs, pilot_sym, num_pilots);
    phase_bound_diff(d_diff_phase, num_pilots);

    interpolate_ofdm_symbol(mag_est_vec, d_diff_mag, pilot_pos);
    interpolate_ofdm_symbol(phase_est_vec, d_diff_phase, pilot_pos);
    phase_bound_abs(phase_est_vec, d_subcarriers);
}

inline void
lte_channel_estimator_vcvc::extract_pilots(gr_complex* b_vec, gr_complex* a_vec, std::vector<int> pilot_pos)
{
    for(int i = 0; i < pilot_pos.size(); i++){
        b_vec[i] = a_vec[pilot_pos[i] ];
    }
}
inline void
lte_channel_estimator_vcvc::calculate_mag_phase_diff(float* diff_mag, float* diff_phase,
                                                     gr_complex* rx_rs, gr_complex* pilot_sym,
                                                     int num_pilots)
{
    /*
     * (x+jy)/(a+jb) = [(x+jy)*(a-jb)]/[a^2+b^2]
     * mag(a+jb) == 1 in this case and therefore a^2+b^2 == 1
     * Calculation is simplified to (x+jy)*(a-jb)
     */
    volk_32fc_x2_multiply_conjugate_32fc_a(d_diff_rx_rs, rx_rs, pilot_sym, num_pilots);
    volk_32fc_magnitude_32f_a(diff_mag, d_diff_rx_rs, num_pilots);
    volk_32fc_s32f_atan2_32f_a(diff_phase, d_diff_rx_rs, 1, num_pilots);
}

// make sure phase difference between 2 values is within [-PI, PI)
void inline
lte_channel_estimator_vcvc::phase_bound_diff(float* phase_vec, int len)
{
    for(int i = 1; i < len; i++){
        while(phase_vec[i]-phase_vec[i-1] >  M_PI){phase_vec[i] -= 2*M_PI;}
        while(phase_vec[i]-phase_vec[i-1] < -M_PI){phase_vec[i] += 2*M_PI;}
    }
}

void inline
lte_channel_estimator_vcvc::phase_bound_abs(float* phase_vec, int len)
{
    for(int i = 0; i < len; i++){
        while(phase_vec[i] > M_PI){phase_vec[i] -= 2*M_PI;}
        while(phase_vec[i] <= -M_PI){phase_vec[i] += 2*M_PI;}
    }
}

void inline
lte_channel_estimator_vcvc::interpolate_ofdm_symbol(float* b_vec, float* a_vec, std::vector<int> pilot_pos)
{
    for(int i = 0; i <= pilot_pos.front() ; i++){
        b_vec[i] = a_vec[0];
        //printf("pos = %i\tpilot_pos = %i\n", i, pilot_pos.front() );
    }
    int num_pilots = pilot_pos.size();

    for(int i = 0; i < num_pilots-1; i++){
        interpolate(&b_vec[pilot_pos[i] ],   a_vec[i],   a_vec[i+1], pilot_pos[i+1]-pilot_pos[i] );
        //printf("pos = %i\tpilot_pos = %i\n", i, pilot_pos[i] );
    }

    for(int i = pilot_pos.back(); i < d_subcarriers; i++){
        b_vec[i] = a_vec[num_pilots-1];
        //printf("pos = %i\tpilot_pos = %i\n", i, pilot_pos.back() );
    }
}

//Calculate linear interpolated values
//only values [first_val, last_val) are written to the output array
void inline
lte_channel_estimator_vcvc::interpolate(float* interp_vals,
            float first_val,
            float last_val,
            int steps)
{
    float diff = (last_val-first_val)/steps;
    for(int i = 0; i < steps; i++){
        interp_vals[i] = first_val + diff*i;
    }
}


void
lte_channel_estimator_vcvc::set_pilot_map(const std::vector<std::vector<int> > &pilot_carriers,
                                        const std::vector<std::vector<gr_complex> > &pilot_symbols)
{
    d_pilot_carriers = pilot_carriers;
    d_pilot_symbols.clear();
    for( int i = 0; i < pilot_symbols.size(); i++){
        // aligned arrays for each symbol.
        d_pilot_symbols.push_back( (gr_complex*)fftwf_malloc(sizeof(gr_complex) * pilot_symbols[i].size()) );
        memcpy(d_pilot_symbols[i], &pilot_symbols[i][0], sizeof(gr_complex) * pilot_symbols[i].size() );

        //Test output, just to see something on the console
 /*       for(int c = 0; c < pilot_symbols[i].size(); c++){
            printf("symbol = %i\tpos = %i\tvalue = %+1.2f %+1.2fj\n", i, d_pilot_carriers[i][c], d_pilot_symbols[i][c].real(), d_pilot_symbols[i][c].imag());

        }*/
    }
    initialize_volk_vectors();
}

void
lte_channel_estimator_vcvc::initialize_volk_vectors()
{
    int max_pilots = get_max_pilot_number();
    d_rx_rs = (gr_complex*)fftwf_malloc(sizeof(gr_complex) * max_pilots);
    d_diff_rx_rs = (gr_complex*)fftwf_malloc(sizeof(gr_complex) * max_pilots);
    d_diff_mag = (float*)fftwf_malloc(sizeof(float) * max_pilots);
    d_diff_phase = (float*)fftwf_malloc(sizeof(float) * max_pilots);

}

int
lte_channel_estimator_vcvc::get_max_pilot_number()
{
    int max_size = 0;
    for(int i = 0; i < d_pilot_carriers.size(); i++){
        max_size = std::max(max_size, int(d_pilot_carriers[i].size()) );
    }
    return max_size;
}





