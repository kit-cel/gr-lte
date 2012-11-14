/* -*- c++ -*- */
/* 
 * Copyright 2012 <+YOU OR YOUR COMPANY+>.
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

#ifndef INCLUDED_LTE_PRE_DECODER_VCVC_H
#define INCLUDED_LTE_PRE_DECODER_VCVC_H

#include <lte_api.h>
#include <gr_sync_block.h>

class lte_pre_decoder_vcvc;
typedef boost::shared_ptr<lte_pre_decoder_vcvc> lte_pre_decoder_vcvc_sptr;

LTE_API lte_pre_decoder_vcvc_sptr lte_make_pre_decoder_vcvc (int N_ant,std::string style);

/*!
 * \brief <+description+>
 *
 */
class LTE_API lte_pre_decoder_vcvc : public gr_sync_block
{
	friend LTE_API lte_pre_decoder_vcvc_sptr lte_make_pre_decoder_vcvc (int N_ant,std::string style);

	lte_pre_decoder_vcvc (int N_ant, std::string style);

 public:
	~lte_pre_decoder_vcvc ();


	int work (int noutput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items);
		
 private:
    int d_N_ant;
    std::string d_style;
};

#endif /* INCLUDED_LTE_PRE_DECODER_VCVC_H */

