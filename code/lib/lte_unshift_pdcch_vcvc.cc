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
#include "lte_unshift_pdcch_vcvc.h"

lte_unshift_pdcch_vcvc_sptr
lte_make_unshift_pdcch_vcvc (int N_rb_dl, int N_ant, int cfi, float N_g, const std::string& len_tag_key)
{
	return gnuradio::get_initial_sptr (new lte_unshift_pdcch_vcvc(N_rb_dl, N_ant, cfi, N_g, len_tag_key));
}


/*
 * The private constructor
 */
lte_unshift_pdcch_vcvc::lte_unshift_pdcch_vcvc (int N_rb_dl, int N_ant, int cfi, float N_g, const std::string& len_tag_key)
  : gr_tagged_stream_block ("unshift_pdcch_vcvc",
		   gr_make_io_signature(1, 1, sizeof(gr_complex) * 36),
		   gr_make_io_signature(1, 1, sizeof(gr_complex) * 36),
		   len_tag_key),
		   d_len_tag_key(len_tag_key)
{
	set_N_rb_dl(N_rb_dl);
	set_N_ant(N_ant);
	set_cfi(cfi);
	set_N_g(N_g);
}


/*
 * Our virtual destructor.
 */
lte_unshift_pdcch_vcvc::~lte_unshift_pdcch_vcvc()
{
	// Put in <+destructor stuff+> here
}


int
lte_unshift_pdcch_vcvc::work(int noutput_items,
        gr_vector_int &ninput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
{
	const gr_complex *in = (const gr_complex *) input_items[0];
	gr_complex *out = (gr_complex *) output_items[0];

	// Do <+signal processing+>

	// Tell runtime system how many output items we produced.
	return noutput_items;
}

void
lte_unshift_pdcch_vcvc::set_N_rb_dl(int N_rb_dl)
{
    d_N_rb_dl = N_rb_dl;
}

void
lte_unshift_pdcch_vcvc::set_N_ant(int N_ant)
{
    d_N_ant = N_ant;
}

void
lte_unshift_pdcch_vcvc::set_cfi(int cfi)
{
    d_cfi = cfi;
}

void
lte_unshift_pdcch_vcvc::set_N_g(float N_g)
{
    d_N_g = N_g;
}

//~ int
//~ lte_unshift_pdcch_vcvc::get_n_regs()
//~ {
    //~ int M_pcfich_bits = 32; // get_pcfich_length_bits() == 32 ALWAYS!!
    //~ int M_phich_bits = get_phich_length_bits(N_g, N_rb_dl)
    //~ int M_used = int(M_pcfich_bits/2) + M_phich_bits
    //~ n_syms = cfi
    //~ if N_rb_dl < 10:
        //~ n_syms = n_syms + 1
    //~ total_re = n_syms * 12 * N_rb_dl
    //~ rs_re = 0
    //~ if N_ant < 4:
        //~ rs_re = 4 * N_rb_dl
    //~ elif N_ant == 4:
        //~ rs_re = 4 * N_rb_dl
        //~ if n_syms > 1:
            //~ rs_re = rs_re + 4 * N_rb_dl
    //~ n_avail = total_re - (rs_re + M_used)
    //~ return n_avail / 4
//~ }


//~ int
//~ lte_unshift_pdcch_vcvc::get_phich_length_bits(float N_g, int N_rb_dl)
//~ {
    //~ std::string cp_len = "normal";
    //~ return get_phich_scr_length(cp_len) * get_n_phich_groups(N_g, N_rb_dl); // only normal CP length at this point.
//~ }

//~ int
//~ lte_unshift_pdcch_vcvc::get_n_phich_groups(float N_g, int N_rb_dl)
//~ {
    //~ return int(math::ceil(N_g * (N_rb_dl/8)));
//~ }
//~ 
//~ int
//~ lte_unshift_pdcch_vcvc::get_phich_scr_length(std::string cp_len)
//~ {
    //~ return 3 * get_N_phich_sf(cp_len);
//~ }
//~ 
//~ int
//~ lte_unshift_pdcch_vcvc::get_N_phich_sf(std::string cp_len)
//~ {
    //~ switch(cp_len){
        //~ case "normal": return 4;
        //~ case "extended": return 2;
        //~ default: return 0; // invalid input!!!
    //~ }
//~ }


