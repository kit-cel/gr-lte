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


#ifndef INCLUDED_LTE_UNSHIFT_PDCCH_VCVC_H
#define INCLUDED_LTE_UNSHIFT_PDCCH_VCVC_H

#include <lte_api.h>
#include <gr_tagged_stream_block.h>

class lte_unshift_pdcch_vcvc;

typedef boost::shared_ptr<lte_unshift_pdcch_vcvc> lte_unshift_pdcch_vcvc_sptr;

LTE_API lte_unshift_pdcch_vcvc_sptr lte_make_unshift_pdcch_vcvc (int N_rb_dl, int N_ant, int cfi, float N_g, const std::string& len_tag_key);

/*!
 * \brief <+description+>
 * \ingroup lte
 *
 */
class LTE_API lte_unshift_pdcch_vcvc : public gr_tagged_stream_block
{
 private:
	friend LTE_API lte_unshift_pdcch_vcvc_sptr lte_make_unshift_pdcch_vcvc (int N_rb_dl, int N_ant, int cfi, float N_g, const std::string& len_tag_key);

	lte_unshift_pdcch_vcvc(int N_rb_dl, int N_ant, int cfi, float N_g, const std::string& len_tag_key);

	// parameters
	int d_N_rb_dl;
	int d_N_ant;
	int d_cfi;
	float d_N_g;
	const std::string& d_len_tag_key;

	int get_n_regs();

 public:
  ~lte_unshift_pdcch_vcvc();

	// Where all the action really happens
	int work (int noutput_items,
        gr_vector_int &ninput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items);

    void set_N_rb_dl(int N_rb_dl);
    void set_N_ant(int N_ant);
    void set_cfi(int cfi);
    void set_N_g(float N_g);
};

#endif /* INCLUDED_LTE_UNSHIFT_PDCCH_VCVC_H */

