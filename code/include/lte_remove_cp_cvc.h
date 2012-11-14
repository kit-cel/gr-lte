/* -*- c++ -*- */
/* 
 * Copyright 2012 Johannes Demel
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

#ifndef INCLUDED_LTE_REMOVE_CP_CVC_H
#define INCLUDED_LTE_REMOVE_CP_CVC_H

#include <lte_api.h>
#include <gr_block.h>

class lte_remove_cp_cvc;
typedef boost::shared_ptr<lte_remove_cp_cvc> lte_remove_cp_cvc_sptr;

LTE_API lte_remove_cp_cvc_sptr lte_make_remove_cp_cvc (int fftl);

/*!
 * \brief <+description+>
 *
 */
class LTE_API lte_remove_cp_cvc : public gr_block
{
	friend LTE_API lte_remove_cp_cvc_sptr lte_make_remove_cp_cvc (int fftl);

	lte_remove_cp_cvc (int fftl);

 public:
	~lte_remove_cp_cvc ();

    void forecast (int noutput_items, gr_vector_int &ninput_items_required);
    int general_work (int noutput_items,
		    gr_vector_int &ninput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);
		    
 private:
    int d_fftl;
    int d_cpl;
    int d_cpl0;
    int d_slotl;
    int d_symb;
    int d_sym_num;
    pmt::pmt_t d_key;
	pmt::pmt_t d_tag_id;
	int d_work_call;
	bool d_found_frame_start;
	long d_frame_start;
    
};

#endif /* INCLUDED_LTE_REMOVE_CP_CVC_H */

