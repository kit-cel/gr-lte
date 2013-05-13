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
                                 std::string tag_key,
                                 std::string msg_buf_name,
                                 const std::vector<std::vector<int> > &pilot_carriers,
                                 const std::vector<std::vector<gr_complex> > &pilot_symbols)
{
    return gnuradio::get_initial_sptr (new lte_channel_estimator_vcvc(subcarriers, tag_key, msg_buf_name, pilot_carriers, pilot_symbols));
}


/*
 * The private constructor
 */
lte_channel_estimator_vcvc::lte_channel_estimator_vcvc (int subcarriers,
        std::string tag_key,
        std::string msg_buf_name,
        const std::vector<std::vector<int> > &pilot_carriers,
        const std::vector<std::vector<gr_complex> > &pilot_symbols)
    : gr_sync_block ("channel_estimator_vcvc",
                     gr_make_io_signature(1, 1, sizeof(gr_complex) * subcarriers ),
                     gr_make_io_signature(1, 1, sizeof(gr_complex) * subcarriers )),
    d_subcarriers(subcarriers),
    d_last_calced_sym(0),
    d_work_call(0)
{
    d_key=pmt::pmt_string_to_symbol(tag_key); // specify key of tag.

    message_port_register_in(pmt::mp(msg_buf_name));
    //set_msg_handler(pmt::mp(tag_key), boost::bind(&lte_channel_estimator::set_cell_id_msg, this, _1));

    set_pilot_map(pilot_carriers, pilot_symbols);
}


/*
 * Our virtual destructor.
 */
lte_channel_estimator_vcvc::~lte_channel_estimator_vcvc()
{
    fftwf_free(d_rx_rs);
    /*
         for(int i = 0; i < d_pilot_symbols.size(); i++) {
              printf("free pilot_syms %i\n", i);
              fftwf_free(d_pilot_symbols[i]);
         }
    */
}


int
lte_channel_estimator_vcvc::work(int noutput_items,
                                 gr_vector_const_void_star &input_items,
                                 gr_vector_void_star &output_items)
{
    const gr_complex *in = (const gr_complex *) input_items[0];
    gr_complex *out = (gr_complex *) output_items[0];
    //d_work_call++;

    std::vector <gr_tag_t> v_b;
    get_tags_in_range(v_b, 0, nitems_read(0), nitems_read(0)+noutput_items, d_key);
    int first_sym = get_sym_num_from_tags(v_b);

    int processed_items = calculate_channel_estimates(in, first_sym, noutput_items);
    copy_estimates_to_out_buf(out, first_sym, processed_items);

    // Tell runtime system how many output items we produced.
    return processed_items;
}

inline int
lte_channel_estimator_vcvc::get_sym_num_from_tags(std::vector <gr_tag_t> v_b)
{
    int sym_num = -1;
    for(int i = 0; i < v_b.size() ; i++) {
        long tag_offset     = v_b[i].offset;
        int value           = int(pmt::pmt_to_long(v_b[i].value) );
        sym_num = (value - ( tag_offset - nitems_read(0) ) + d_n_frame_syms )%d_n_frame_syms;
    }
    if(sym_num < 0){
        sym_num = d_last_calced_sym+1;
    }
    return sym_num;
}

inline void
lte_channel_estimator_vcvc::copy_estimates_to_out_buf(gr_complex* out, int sym_num, int processed_items)
{
    for(int i = sym_num; i-sym_num < processed_items; i++) {
        memcpy(out, d_estimates[i%d_n_frame_syms], sizeof(gr_complex) * d_subcarriers);
        out += d_subcarriers;
//        printf("\nsym_num = %i\n", i);
//        for(int n = 0; n < d_subcarriers; n++){
//            //printf("%i\t%+1.2f %+1.2fj\t", n, rx_vec[n].real(), rx_vec[n].imag() );
//            //printf("mag = %+1.2f\tphase = %+1.2f\t", d_mag_estimates[i][n], d_phase_estimates[i][n] );
//            //printf("%+1.2f %+1.2fj\n", d_estimates[i][n].real(), d_estimates[i][n].imag() );
//        }
    }


}

int
lte_channel_estimator_vcvc::calculate_channel_estimates(const gr_complex* in_rx, int first_sym, int nitems)
{
    int last_sym = get_last_processable_sym(first_sym, nitems);
    int processable_items = get_processable_items(first_sym, nitems);
    int last_calced_sym = d_last_calced_sym;
    //printf("first_sym = %i\tlast_sym = %i\tprocessable_items = %i\tlast_calced = %i\n", first_sym, last_sym, processable_items, last_calced_sym);
    calculate_ofdm_symbols_with_pilots(in_rx, first_sym, processable_items);
    calculate_interpolated_ofdm_symbols(last_calced_sym, processable_items);
    processed_items_to_complex(first_sym, processable_items);

    return processable_items;
}

inline void
lte_channel_estimator_vcvc::calculate_ofdm_symbols_with_pilots(const gr_complex* in_rx, int first_sym, int processable_items)
{
    int sym = first_sym;
    for(int i = first_sym; i-first_sym <= processable_items; i++) {
        sym = i%d_n_frame_syms;
        if(d_pilot_carriers[sym].size() > 0) {
            //printf("calc_ofdm_sym = %i\n", sym);
            memcpy(d_rx_vec, in_rx+(i-first_sym)*d_subcarriers, sizeof(gr_complex)*d_subcarriers );
            estimate_ofdm_symbol(d_mag_estimates[sym], d_phase_estimates[sym], d_rx_vec, d_pilot_carriers[sym], d_pilot_symbols[sym]);
            d_last_calced_sym = sym;
        }
    }
}

inline void
lte_channel_estimator_vcvc::calculate_interpolated_ofdm_symbols(int first_sym, int processable_items)
{
    int current_sym = first_sym;
    int next_sym = first_sym;
    phase_bound_between_pilot_vectors(first_sym, processable_items);

    for(int i = first_sym; i-first_sym < processable_items; i++) {
        current_sym = i%d_n_frame_syms;
        if(d_pilot_carriers[current_sym].size() > 0) {
            for(int n = i+1 ; n-first_sym <= processable_items; n++ ) {
                next_sym = n%d_n_frame_syms;
                if(d_pilot_carriers[next_sym].size() > 0) {
                    interpolate_between_vectors(d_mag_estimates, current_sym, next_sym);
                    interpolate_between_vectors(d_phase_estimates, current_sym, next_sym);
                    //printf("interpolate between %i -- %i\n", current_sym, next_sym);
                    break;
                }
            }
        }
    }
}

inline void
lte_channel_estimator_vcvc::processed_items_to_complex(int first_sym, int processed_items)
{
    int sym = first_sym;
    for(int i = first_sym; i-first_sym <=  processed_items; i++) {
        sym = i%d_n_frame_syms;
        //printf("mag_phase_to_complex\tsym = %i\n", sym);
        vector_mag_phase_to_complex(d_estimates[sym], d_mag_estimates[sym], d_phase_estimates[sym], d_subcarriers);
    }
}

inline int
lte_channel_estimator_vcvc::get_processable_items(int sym_num, int nitems)
{
    int processable_items = 0;
    for(int i = sym_num; i-sym_num < nitems ; i++) {
        if(d_pilot_carriers[i%d_n_frame_syms].size() > 0) {
            //printf("get_proc_items\tsym_num = %i\n", i);
            processable_items = i-sym_num+1;
        }
    }
    return processable_items;
}

inline int
lte_channel_estimator_vcvc::get_last_processable_sym(int first_sym, int nitems)
{
    int last_sym = first_sym;
    int n_frame_syms = d_pilot_carriers.size();
    for(int i = first_sym; i-first_sym < nitems; i++) {
        if(d_pilot_carriers[i%n_frame_syms].size() > 0) {
            //printf("get_last_proc_items\tsym_num = %i\n", i);
            last_sym = i%n_frame_syms;
        }
    }
    return last_sym;
}

void inline
lte_channel_estimator_vcvc::interpolate_between_vectors(std::vector<float*> &estimates, int previous_sym, int current_sym)
{
    //printf("interpolate between %i\t%i\n", previous_sym, current_sym);
    int steps = (current_sym+d_n_frame_syms-previous_sym)%d_n_frame_syms;
    float mult_value = 1.0f/float(steps);
    volk_32f_x2_subtract_32f_a(d_diff_vector, estimates[current_sym], estimates[previous_sym], d_subcarriers);
    // The following VOLK OP does have serious problems if called _a. DEBUG!
    volk_32f_s32f_multiply_32f_u(d_div_vector, d_diff_vector, mult_value, d_subcarriers); // alignment problems?
    int sym = previous_sym;
    int prev_sym = previous_sym;
    for(int i = previous_sym+1; i < previous_sym+steps; i++) {
        sym = i%d_n_frame_syms;
        //printf("interpolate symbol = %i\n", i);
        volk_32f_x2_add_32f_a(estimates[sym], estimates[prev_sym], d_div_vector, d_subcarriers);
        prev_sym = sym;
    }
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
    for(int i = 0; i < pilot_pos.size(); i++) {
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
    for(int i = 1; i < len; i++) {
        while(phase_vec[i]-phase_vec[i-1] >  M_PI) {
            phase_vec[i] -= 2*M_PI;
        }
        while(phase_vec[i]-phase_vec[i-1] < -M_PI) {
            phase_vec[i] += 2*M_PI;
        }
    }
}

void inline
lte_channel_estimator_vcvc::phase_bound_abs(float* phase_vec, int len)
{
    for(int i = 0; i < len; i++) {
        while(phase_vec[i] > M_PI) {
            phase_vec[i] -= 2*M_PI;
        }
        while(phase_vec[i] <= -M_PI) {
            phase_vec[i] += 2*M_PI;
        }
    }
}

void inline
lte_channel_estimator_vcvc::phase_bound_between_pilot_vectors(int first_sym, int processable_items)
{
    int current_sym = first_sym;
    int next_sym = first_sym;
    for(int i = first_sym; i-first_sym < processable_items; i++) {
        current_sym = i%d_n_frame_syms;
        if(d_pilot_carriers[current_sym].size() > 0) {
            for(int c = current_sym; c-current_sym <= processable_items; c++){
                next_sym = c%d_n_frame_syms;
                if(d_pilot_carriers[next_sym].size() > 0){
                    phase_bound_between_vectors(d_phase_estimates[current_sym], d_phase_estimates[next_sym]);
                    break;
                }
            }
        }
    }
}

void inline
lte_channel_estimator_vcvc::phase_bound_between_vectors(float* first, float* last)
{
    volk_32f_x2_subtract_32f_a(d_phase_bound_vector, last, first, d_subcarriers);
    for(int i = 0; i < d_subcarriers; i++) {
        if( *(d_phase_bound_vector+i) >  M_PI) {
            *(last+i) -= 2*M_PI;
        }
        if( *(d_phase_bound_vector+i) < -M_PI) {
            *(last+i) += 2*M_PI;
        }
    }
}

void inline
lte_channel_estimator_vcvc::interpolate_ofdm_symbol(float* b_vec, float* a_vec, std::vector<int> pilot_pos)
{
    for(int i = 0; i <= pilot_pos.front() ; i++) {
        b_vec[i] = a_vec[0];
        //printf("pos = %i\tpilot_pos = %i\n", i, pilot_pos.front() );
    }
    int num_pilots = pilot_pos.size();

    for(int i = 0; i < num_pilots-1; i++) {
        interpolate(&b_vec[pilot_pos[i] ],   a_vec[i],   a_vec[i+1], pilot_pos[i+1]-pilot_pos[i] );
        //printf("pos = %i\tpilot_pos = %i\n", i, pilot_pos[i] );
    }

    for(int i = pilot_pos.back(); i < d_subcarriers; i++) {
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
    for(int i = 0; i < steps; i++) {
        interp_vals[i] = first_val + diff*i;
    }
}

inline void
lte_channel_estimator_vcvc::vector_mag_phase_to_complex(gr_complex* c_vec, float* m_vec, float* p_vec, int len)
{
    for(int i = 0; i < len; i++) {
        c_vec[i] = mag_phase_to_complex(m_vec[i], p_vec[i]);
    }
}

gr_complex inline
lte_channel_estimator_vcvc::mag_phase_to_complex(float mag, float phase)
{
    return gr_complex(mag*cos(phase), mag*sin(phase));
}

void
lte_channel_estimator_vcvc::set_pilot_map(const std::vector<std::vector<int> > &pilot_carriers,
        const std::vector<std::vector<gr_complex> > &pilot_symbols)
{
    //printf("%s\tset_pilot_map BEGIN\n", name().c_str() );
    int n_frame_syms = get_nsyms_in_frame(pilot_carriers);
    int max_pilots = get_max_pilot_number(pilot_carriers);

    init_pilot_symbol_arrays(pilot_symbols, n_frame_syms, max_pilots);
    initialize_volk_vectors(max_pilots, d_subcarriers, n_frame_syms);

    d_n_frame_syms = n_frame_syms;
    d_pilot_carriers = pilot_carriers;
    //printf("set_pilot_map END\n");
}

inline int
lte_channel_estimator_vcvc::get_nsyms_in_frame(const std::vector<std::vector<int> > &pilot_carriers)
{
    return pilot_carriers.size();
}

inline int
lte_channel_estimator_vcvc::get_max_pilot_number(const std::vector<std::vector<int> > &pilot_carriers)
{
    int max_size = 0;
    for(int i = 0; i < pilot_carriers.size(); i++) {
        max_size = std::max(max_size, int(pilot_carriers[i].size()) );
    }
    return max_size;
}

inline void
lte_channel_estimator_vcvc::init_pilot_symbol_arrays(const std::vector<std::vector<gr_complex> > &pilot_symbols, int n_frame_syms, int max_pilots)
{
    d_pilot_symbols.clear();
    d_pilot_symbols.reserve(n_frame_syms);

    for( int i = 0; i < n_frame_syms; i++) {
        // aligned arrays for each symbol.
        //printf("set_pilot_sym %i\n", i);

        d_pilot_symbols.push_back( (gr_complex*)fftwf_malloc(sizeof(gr_complex) * max_pilots) );
        memcpy(d_pilot_symbols[i], &pilot_symbols[i][0], sizeof(gr_complex) * pilot_symbols[i].size() );
    }
}

inline void
lte_channel_estimator_vcvc::initialize_volk_vectors(int max_pilots, int subcarriers, int n_frame_syms)
{
    //printf("%s\tinitialize VOLK vectors BEGIN\n", name().c_str() );

    init_pilot_dependend_volk_vectors(max_pilots);
    init_subcarrier_dependend_volk_vectors(subcarriers);
    init_estimates_store_volk_vectors(subcarriers, n_frame_syms);
}

inline void
lte_channel_estimator_vcvc::init_pilot_dependend_volk_vectors(int max_pilots)
{
    d_rx_rs = (gr_complex*)fftwf_malloc(sizeof(gr_complex) * max_pilots);
    d_diff_rx_rs = (gr_complex*)fftwf_malloc(sizeof(gr_complex) * max_pilots);
    d_diff_mag = (float*)fftwf_malloc(sizeof(float) * max_pilots);
    d_diff_phase = (float*)fftwf_malloc(sizeof(float) * max_pilots);
}

inline void
lte_channel_estimator_vcvc::init_subcarrier_dependend_volk_vectors(int subcarriers)
{
    d_diff_vector = (float*)fftwf_malloc(sizeof(float) * subcarriers);
    d_div_vector = (float*)fftwf_malloc(sizeof(float) * subcarriers);
    d_rx_vec = (gr_complex*)fftwf_malloc(sizeof(gr_complex) * subcarriers);
    d_phase_bound_vector = (float*)fftwf_malloc(sizeof(float) * subcarriers);
}

inline void
lte_channel_estimator_vcvc::init_estimates_store_volk_vectors(int subcarriers, int n_frame_syms)
{
    d_estimates.clear();
    d_mag_estimates.clear();
    d_phase_estimates.clear();
    //printf("capacity\test = %i\tmag = %i\tphase = %i\n", int(d_estimates.capacity()), int(d_mag_estimates.capacity()), int(d_phase_estimates.capacity()) );
    d_estimates.reserve(n_frame_syms);
    d_mag_estimates.reserve(n_frame_syms);
    d_phase_estimates.reserve(n_frame_syms);

    for(int i = 0; i < n_frame_syms; i++) {
        d_estimates.push_back( (gr_complex*)fftwf_malloc(sizeof(gr_complex) * subcarriers) );
        d_mag_estimates.push_back( (float*)fftwf_malloc(sizeof(float) * subcarriers) );
        d_phase_estimates.push_back( (float*)fftwf_malloc(sizeof(float) * subcarriers) );
    }
}






