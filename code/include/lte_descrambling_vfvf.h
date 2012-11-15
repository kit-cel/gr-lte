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

#ifndef INCLUDED_LTE_DESCRAMBLING_VFVF_H
#define INCLUDED_LTE_DESCRAMBLING_VFVF_H

#include <lte_api.h>
#include <gr_sync_interpolator.h>

class lte_descrambling_vfvf;
typedef boost::shared_ptr<lte_descrambling_vfvf> lte_descrambling_vfvf_sptr;

LTE_API lte_descrambling_vfvf_sptr lte_make_descrambling_vfvf ();// int cell_id

/*!
 * \brief <+description+>
 *
 */
class LTE_API lte_descrambling_vfvf : public gr_sync_interpolator
{
	friend LTE_API lte_descrambling_vfvf_sptr lte_make_descrambling_vfvf ();//int cell_id

	lte_descrambling_vfvf (); //int cell_id

 public:
	~lte_descrambling_vfvf ();
	
	void set_cell_id(int id);

	static char* pn_seq_generator(int len, int cell_id);
	int work (int noutput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items);
private:
	int d_cell_id;
	char* d_pn_seq;
	int d_pn_seq_len;
	
	pmt::pmt_t d_key;
	pmt::pmt_t d_tag_id;
	int d_work_call;
};

#endif /* INCLUDED_LTE_DESCRAMBLING_VFVF_H */

