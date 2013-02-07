/* -*- c++ -*- */
/*
 * Copyright 2013 Communications Engineering Lab (CEL) / Karlsruhe Institute of Technology (KIT)
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
#include <lte_channel_estimator.h>


lte_channel_estimator_sptr
lte_make_channel_estimator (int N_rb_dl)
{
	return lte_channel_estimator_sptr (new lte_channel_estimator (N_rb_dl));
}


lte_channel_estimator::lte_channel_estimator (int N_rb_dl)
	: gr_sync_block ("channel_estimator",
		gr_make_io_signature ( 1, 1, sizeof(gr_complex)*12*N_rb_dl),
		gr_make_io_signature ( 3, 3, sizeof(gr_complex)*12*N_rb_dl)),
		d_N_rb_dl(N_rb_dl),
		d_cell_id(-1),
		d_work_call(0)
{
    message_port_register_in(pmt::mp("cell_id"));
    set_msg_handler(pmt::mp("cell_id"), boost::bind(&lte_channel_estimator::set_cell_id_msg, this, _1));

    d_key=pmt::pmt_string_to_symbol("symbol"); // specify key of tag.

    //Initial block setup
    set_min_output_buffer(8* sizeof(gr_complex)*12*N_rb_dl );
    set_output_multiple(7);

    // Initialize some basic vectors which are aligned for VOLK Operations
    //This arrays are used for estimation in frequency domain
    d_rx_rs     = (gr_complex*)fftwf_malloc(sizeof(gr_complex)* 2*d_N_rb_dl);
    d_div_rx_rs = (gr_complex*)fftwf_malloc(sizeof(gr_complex)* 2*d_N_rb_dl);
    d_div_mag   = (float*)fftwf_malloc(sizeof(float)*12*d_N_rb_dl);
    d_div_phase = (float*)fftwf_malloc(sizeof(float)*12*d_N_rb_dl);
    // aligned arrays for VOLK. Used to store them for the next slot.
    for(int i = 0; i < 2; i++){
        d_est_mag[i]   = (float*)fftwf_malloc(sizeof(float)*12*d_N_rb_dl);
        d_est_phase[i] = (float*)fftwf_malloc(sizeof(float)*12*d_N_rb_dl);
        //initialize these vectors with standard values
        for(int c = 0; c < 12*d_N_rb_dl; c++){
            d_est_mag[i][c] = 1.0f;
            d_est_phase[i][c]= 0.0f;
        }
    }

    for(int i = 0; i < 8 ; i++){
        d_est_slot_mag[i] = (float*)fftwf_malloc(sizeof(float)*12*d_N_rb_dl);
        d_est_slot_phase[i] = (float*)fftwf_malloc(sizeof(float)*12*d_N_rb_dl);
    }
    for(int i = 0; i < 3; i++){
        d_rx_rs_symbol_vectors[i] = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*12*d_N_rb_dl);
    }
    for(int i = 0; i < 8 ; i++){
        d_est_slot_p0[i] = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*12*d_N_rb_dl);
        d_est_slot_p1[i] = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*12*d_N_rb_dl);
    }
    d_diff_vector = (float*)fftwf_malloc(sizeof(float)*12*d_N_rb_dl);
    d_div_vector  = (float*)fftwf_malloc(sizeof(float)*12*d_N_rb_dl);
    d_phase_bound_vector = (float*)fftwf_malloc(sizeof(float)*12*d_N_rb_dl);
}


lte_channel_estimator::~lte_channel_estimator ()
{
    fftwf_free(d_rx_rs);
    fftwf_free(d_div_rx_rs);
    fftwf_free(d_div_mag);
    fftwf_free(d_div_phase);
    for(int i = 0; i < 2 ; i++){
        fftwf_free(d_est_mag[i]);
        fftwf_free(d_est_phase[i]);
    }
    for(int i = 0; i < 8 ; i++){
        fftwf_free(d_est_slot_mag[i]);
        fftwf_free(d_est_slot_phase[i]);
    }
    for(int i = 0; i < 3; i++){
        fftwf_free(d_rx_rs_symbol_vectors[i]);
    }
    for(int i = 0; i < 8 ; i++){
        fftwf_free(d_est_slot_p0[i]);
        fftwf_free(d_est_slot_p1[i]);
    }
    fftwf_free(d_diff_vector);
    fftwf_free(d_div_vector);
    fftwf_free(d_phase_bound_vector);

    if(d_cell_id >= 0){ // If RS symbol vectors were initialized --> delete them
        for(int i = 0; i < 40; i++){
            for(int p = 0; p < 2 ; p++){
                fftwf_free(d_rs_matrix[p][i]);
            }
        }
    }
}


int
lte_channel_estimator::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
	const gr_complex *in = (const gr_complex *) input_items[0];
	gr_complex *out0 = (gr_complex *) output_items[0];
	gr_complex *out1 = (gr_complex *) output_items[1];
	gr_complex *out2 = (gr_complex *) output_items[2];

    d_work_call++;
    //printf("%s\twork_call = %i\tnoutput_items = %i\n", name().c_str(), d_work_call, noutput_items);


    int slot_number = 5;
    std::vector <gr_tag_t> v_b;
    get_tags_in_range(v_b,0,nitems_read(0),nitems_read(0)+1, d_key);
    if(v_b.size() > 0){
        long tag_offset     = v_b[0].offset;
        std::string key      = pmt::pmt_symbol_to_string(v_b[0].key);
        int value           = int(pmt::pmt_to_long(v_b[0].value) );
        if(value%7 != 0 ){return value%7+7;}
        std::string srcid    = pmt::pmt_symbol_to_string(v_b[0].srcid);
        slot_number = value / 7;
        //printf("TAG!\tkey = %s\tvalue = %i\tslot_number = %i\toffset %ld\n", key.c_str(), value, slot_number, tag_offset);
	}

    //The following part of work does all the work!
    // extract ofdm symbols with RSs
    memcpy(d_rx_rs_symbol_vectors[0], in, sizeof(gr_complex)*12*d_N_rb_dl);
    memcpy(d_rx_rs_symbol_vectors[1], in+4*12*d_N_rb_dl, sizeof(gr_complex)*12*d_N_rb_dl);
    memcpy(d_rx_rs_symbol_vectors[2], in+7*12*d_N_rb_dl, sizeof(gr_complex)*12*d_N_rb_dl);
    //printf("%s\twork_call = %i\tDEBUG before\n", name().c_str(), d_work_call);
    //The next lines call all the calculation methods which do all the tasks.
    estimate_slot_symbols(d_est_slot_p0, d_rx_rs_symbol_vectors, slot_number, 0);
    estimate_slot_symbols(d_est_slot_p1, d_rx_rs_symbol_vectors, slot_number, 1);
    //printf("%s\twork_call = %i\tDEBUG after\n", name().c_str(), d_work_call);
    // produce output
    for(int i = 0; i < 7 ; i++){
        memcpy(out0, in, sizeof(gr_complex)*12*d_N_rb_dl);
        memcpy(out1, d_est_slot_p0[i], sizeof(gr_complex)*12*d_N_rb_dl);
        memcpy(out2, d_est_slot_p1[i], sizeof(gr_complex)*12*d_N_rb_dl);
        in += 12*d_N_rb_dl;
        out0 += 12*d_N_rb_dl;
        out1 += 12*d_N_rb_dl;
        out2 += 12*d_N_rb_dl;
    }
    //printf("%s\twork_call = %i\tDEBUG after\n", name().c_str(), d_work_call);
	// Tell runtime system how many output items we produced.
	return 7;
}

void
lte_channel_estimator::estimate_slot_symbols(gr_complex* est_slot[8],
            gr_complex* rx_rs_symbol_vectors[3],
            int ns,
            int p)
{
    //int count = 0;
    //printf("%s\testimate_slot\t%i\n", name().c_str(), count++);
    // Get already calculated vectors
    memcpy(d_est_slot_mag[0],   d_est_mag[p],   sizeof(float)*12*d_N_rb_dl );
    memcpy(d_est_slot_phase[0], d_est_phase[p], sizeof(float)*12*d_N_rb_dl );
    //printf("%s\testimate_slot\t%i\n", name().c_str(), count++);
    // Call estimator method for one ofdm symbol
    estimate_ofdm_symbol(d_est_slot_mag[4], d_est_slot_phase[4], rx_rs_symbol_vectors[1], ns  , 4, p);
    estimate_ofdm_symbol(d_est_slot_mag[7], d_est_slot_phase[7], rx_rs_symbol_vectors[2], ns+1, 0, p);
    //printf("%s\testimate_slot\t%i\n", name().c_str(), count++);
    // Take care that phase difference is within bounds.
    phase_bound_between_vectors(d_est_slot_phase[0], d_est_slot_phase[4]);
    phase_bound_between_vectors(d_est_slot_phase[4], d_est_slot_phase[7]);
    //printf("%s\testimate_slot\t%i\n", name().c_str(), count++);
    // This is supposed to be the optimized version of the VOLK interpolator!
    interpolate_vector(d_est_slot_mag  , d_est_slot_mag[0], d_est_slot_mag[4], 4);
    interpolate_vector(d_est_slot_mag+4, d_est_slot_mag[4], d_est_slot_mag[7], 3);
    interpolate_vector(d_est_slot_phase  , d_est_slot_phase[0], d_est_slot_phase[4], 4);
    interpolate_vector(d_est_slot_phase+4, d_est_slot_phase[4], d_est_slot_phase[7], 3);
    //printf("%s\testimate_slot\t%i\n", name().c_str(), count++);
/*
    for(int i = 0; i < 12*d_N_rb_dl ; i++){
        for(int c = 0; c < 7; c++){
            printf("%+2.4f\t", d_est_slot_phase[c][i]);
        }
        printf("\n");
    }
*/
    // Unfortunately this loop couldn't be optimized yet. Possible?
    for(int i=0; i < 12*d_N_rb_dl ; i++){
        for(int c = 0 ; c < 7 ; c++){
            est_slot[c][i] = mag_phase_to_complex(d_est_slot_mag[c][i], d_est_slot_phase[c][i]);
        }
    }
    // Save already calculated vectors for next slot!
    memcpy(d_est_mag[p], d_est_slot_mag[7],     sizeof(float)*12*d_N_rb_dl );
    memcpy(d_est_phase[p], d_est_slot_phase[7], sizeof(float)*12*d_N_rb_dl );
}

void inline
lte_channel_estimator::phase_bound_between_vectors(float* first, float* last)
{
    volk_32f_x2_subtract_32f_a(d_phase_bound_vector, last, first, 12*d_N_rb_dl);
    for(int i = 0; i < 12*d_N_rb_dl; i++){
        if( *(d_phase_bound_vector+i) >  M_PI){*(last+i) -= 2*M_PI;}
        if( *(d_phase_bound_vector+i) < -M_PI){*(last+i) += 2*M_PI;}
    }
}

void inline
lte_channel_estimator::phase_bound_vector(float* phase_vec)
{
    for(int i = 0; i < 12*d_N_rb_dl; i++){
        while(phase_vec[i] > M_PI){phase_vec[i] -= 2*M_PI;}
        while(phase_vec[i] <= -M_PI){phase_vec[i] += 2*M_PI;}
    }
}

// make sure phase difference between 2 values does not exceed 2*pi
void inline
lte_channel_estimator::phase_bound(float* phase_vec, int len)
{
    for(int i = 1; i < len; i++){
        while(phase_vec[i]-phase_vec[i-1] >  M_PI){phase_vec[i] -= 2*M_PI;}
        while(phase_vec[i]-phase_vec[i-1] < -M_PI){phase_vec[i] += 2*M_PI;}
    }
}

// Estimate channel for 1 OFDM symbol with RS symbols
// This is the "real" version of this function. The calculated values are seperated into magnitude and phase.
void
lte_channel_estimator::estimate_ofdm_symbol(float* mag_est_vec,
            float* phase_est_vec,
            gr_complex* symbol_vec,
            int ns,
            int l,
            int p)
{
    const int N_RB_MAX = 110; // Constant defined by LTE standard
    int offset = calc_offset(p, l, ns);
    int first_rs = offset;
    int last_rs = (12*d_N_rb_dl)-(6-offset);
    //extract RS symbols for further calculations!
    int mX = 0;
    int k = 0;
    for (int m = 0 ; m < 2*d_N_rb_dl ; m++ ){
        mX = m+N_RB_MAX-d_N_rb_dl;
        k  = 6*m + offset;
        d_rx_rs[m] = symbol_vec[k];
    }
    // Select the according RS vector
    gr_complex* rs_symbol_vec;
    switch(l){
        case 0: rs_symbol_vec = d_rs_matrix[0][2*ns  ]; break;
        case 4: rs_symbol_vec = d_rs_matrix[0][2*ns+1]; break;
    }
    //(x+jy)/(a+jb) = [(x+jy)*(a-jb)]/[a^2+b^2]
    //mag(a+jb) == 1 in this case and therefore a^2+b^2 == 1
    //Calculation is simplified to (x+jy)*(a-jb)
    volk_32fc_x2_multiply_conjugate_32fc_a(d_div_rx_rs, d_rx_rs, rs_symbol_vec, 2*d_N_rb_dl);
    volk_32fc_magnitude_32f_a(d_div_mag, d_div_rx_rs, 2*d_N_rb_dl);
    volk_32fc_s32f_atan2_32f_a(d_div_phase, d_div_rx_rs, 1, 2*d_N_rb_dl);
    phase_bound(d_div_phase, 2*d_N_rb_dl); // Phase shift between 2 values is within -PI to PI bounds
    //Next lines calculate correction values between RS symbols
    for(int i = 0; i < 2*d_N_rb_dl-1; i++){
        interpolate(mag_est_vec+6*i+offset,   d_div_mag[i],   d_div_mag[i+1], 6);
        interpolate(phase_est_vec+6*i+offset, d_div_phase[i], d_div_phase[i+1], 6);
    }
    //Calculate channel estimates before first RS in OFDM symbol
    float phase_diff = (d_div_phase[1]-d_div_phase[0])/6;
    float mag_diff   = (d_div_mag[1]-d_div_mag[0])/6;
    for(int i = 0; i < first_rs;i++){
        mag_est_vec[i] = d_div_mag[0]-mag_diff*(first_rs-i);
        phase_est_vec[i] = d_div_phase[0]-phase_diff*(first_rs-i);
    }
    //Calculate channel estimates after last RS in OFDM symbol
    phase_diff = (d_div_phase[2*d_N_rb_dl-1]-d_div_phase[2*d_N_rb_dl-2])/6;
    mag_diff   = (d_div_mag[2*d_N_rb_dl-1]-d_div_mag[2*d_N_rb_dl-2])/6;
    for(int i = 0; i < 12*d_N_rb_dl-last_rs; i++){
        mag_est_vec[last_rs+i]   = d_div_mag[2*d_N_rb_dl-1]+i*mag_diff;
        phase_est_vec[last_rs+i] = d_div_phase[2*d_N_rb_dl-1]+i*phase_diff;
    }
    phase_bound_vector(phase_est_vec); // Take care of phase vector to be in bounds between -pi and pi
}

//Calculate linear interpolated values
//only values between first_val, ... , last_val are calculated
//number of calculated values is steps-1
void inline
lte_channel_estimator::interpolate(float* interp_vals,
            float first_val,
            float last_val,
            int steps)
{
    float diff = (last_val-first_val)/steps;
    for(int i = 0; i < steps; i++){
        interp_vals[i] = first_val + diff*i;
    }
}

// Due to some other constraints this method does not behave like interpolate!
void inline
lte_channel_estimator::interpolate_vector(float** interp_vals,
            float* first_val,
            float* last_val,
            int steps)
{
    float mult_value = 1.0f/float(steps);
    //printf("%s\tinterpolate_vector\tmult_value = %2.4f\n", name().c_str(), mult_value);
    volk_32f_x2_subtract_32f_a(d_diff_vector, last_val, first_val, 12*d_N_rb_dl);
    // The following VOLK OP does have serious problems if called _a. DEBUG!
    volk_32f_s32f_multiply_32f_u(d_div_vector, d_diff_vector, mult_value, 12*d_N_rb_dl); // alignment problems?
    for(int i = 1; i < 4; i++){
        volk_32f_x2_add_32f_a(interp_vals[i], interp_vals[i-1], d_div_vector, 12*d_N_rb_dl);
    }
}

// A convenienve function
gr_complex inline
lte_channel_estimator::mag_phase_to_complex(float mag, float phase)
{
    return gr_complex(mag*cos(phase), mag*sin(phase));
}

// calculate offset of first RS symbol in an OFDM symbol
int inline
lte_channel_estimator::calc_offset(int p, int l, int ns)
{
    int v = 0; // Needed to find RS symbols
    if       (p==0 && l==0){v=0;}
    else if (p==0 && l!=0){v=3;}
    else if (p==1 && l==0){v=3;}
    else if (p==1 && l!=0){v=0;}
    else if (p==2){v=3*(ns%2);}
    else if (p==3){v=3*3*(ns%2);}
    return ((v+(d_cell_id%6) )%6);
}

// This is the LTE PN Sequence generator
void
lte_channel_estimator::pn_seq_generator(char* c, //Result will be written into this array. It must have length len
            int len, // Length of the input array
            int cinit) // Init value
{
	const int Nc=1600; //Constant is defined in standard
	char x2[3*len+Nc];
	for (int i = 0; i<31; i++){
        char val = cinit%2;
    	cinit = floor(cinit/2);
    	x2[i] = val;
	}
	char x1[3*len+Nc];
	// initialization for first 35 elements is needed. (Otherwise READ BEFORE INITIALIZATION ERROR)
	for (int i = 0 ; i < 35 ; i++){
		x1[i]=0;
	}
	x1[0]=1;
	for (int n=0; n <2*len+Nc-3;n++){
		x1[n+31]=(x1[n+3]+x1[n])%2;
		x2[n+31]=(x2[n+3]+x2[n+2]+x2[n+1]+x2[n])%2;
	}
	for (int n=0;n<len;n++){
		c[n]=(x1[n+Nc]+x2[n+Nc])%2;
	}
}

// This method returns the specified reference symbols for one symbol
void //gr_complex*
lte_channel_estimator::rs_generator(gr_complex* r, //This array must be 220 items long
            int ns, //This argument specifies the slot number within a frame
            int l, // This argument specifies the OFDM symbol within a slot
            int cell_id, // The cell_id is needed for PN sequence generation
            int Ncp) // Also needed for PN sequence generation
{
    const float SQRT2 = sqrt(2.0); //For easier handling
    const int N_RB_MAX = 110; // This constant is defined in Standard
    int cinit = 1024*(7*(ns+1)+l+1)*(2*cell_id+1)+2*cell_id+Ncp; //Calculate init value for PN Generator
    char c[4*N_RB_MAX]; //array for PN sequence values
    pn_seq_generator(c, 4*N_RB_MAX,cinit);
    for (int m = 0 ; m < 2*N_RB_MAX ; m++ ){
        r[m]=gr_complex( (1-2*c[2*m])/SQRT2 , (1-2*c[2*m+1])/SQRT2 );
    }
}

gr_complex*
lte_channel_estimator::rs_mapper(int N_rb_dl, int ns,int l,int cell_id,int Ncp,int p)
{
    const int Nrbdlmax = 110;

    gr_complex r[2*Nrbdlmax];
    rs_generator(r, ns, l, cell_id, Ncp);
    int vshift = cell_id%6;

    int v = 0;
    if       (p==0 && l==0){v=0;}
    else if (p==0 && l!=0){v=3;}
    else if (p==1 && l==0){v=3;}
    else if (p==1 && l!=0){v=0;}
    else if (p==2){v=3*(ns%2);}
    else if (p==3){v=3*3*(ns%2);}

    gr_complex *vec = new gr_complex[N_rb_dl*12];
    memset(vec,0,12*N_rb_dl*sizeof(gr_complex) );

    int mX = 0;
    int k = 0;
    for (int m = 0 ; m < 2*N_rb_dl ; m++ ){
        mX = m+Nrbdlmax-N_rb_dl;
        k  = 6*m + ((v+vshift)%6);
        vec[k] = r[mX];
    }

    return vec;
}

void
lte_channel_estimator::set_cell_id_msg(pmt::pmt_t msg)
{
    int cell_id = int(pmt::pmt_to_long(msg));
    //printf("********%s INPUT MESSAGE***************\n", name().c_str() );
    //printf("\t%i\n", cell_id);
    //printf("********%s INPUT MESSAGE***************\n", name().c_str() );
    set_cell_id(cell_id);
}

void
lte_channel_estimator::set_cell_id(int cell_id)
{
    printf("%s\tset_cell_id = %i\n", name().c_str(), cell_id);
    int Ncp = 1; //This Ncp == 1 if normal CP is used, else Ncp == 0
    const int N_RB_MAX = 110;
    for(int p = 0 ; p < 2; p++){
        for(int ns = 0 ; ns < 20 ; ns++){
            //printf("%s\tp = %i\tns= = %i\n", name().c_str(), p, ns);
            gr_complex r0[220];
            gr_complex r4[220];
            rs_generator( r0, ns, 0, cell_id, Ncp);
            rs_generator( r4, ns, 4, cell_id, Ncp);
            gr_complex* rs0 = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*12*d_N_rb_dl);
            gr_complex* rs4 = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*12*d_N_rb_dl);

            int mX = 0;
            for (int m = 0 ; m < 2*d_N_rb_dl ; m++ ){
                mX = m+N_RB_MAX-d_N_rb_dl;
                rs0[m]=r0[mX];
                rs4[m]=r4[mX];
            }
            d_rs_matrix[p][2*ns  ] = rs0;
            d_rs_matrix[p][2*ns+1] = rs4;
        }
    }
    d_cell_id = cell_id; // Set cell ID AFTER this method to avoid scheduling problems
}

