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

#ifndef INCLUDED_LTE_CRC_CALCULATOR_VBVB_H
#define INCLUDED_LTE_CRC_CALCULATOR_VBVB_H

#include <lte_api.h>
#include <gr_sync_block.h>

class lte_crc_calculator_vbvb;
typedef boost::shared_ptr<lte_crc_calculator_vbvb> lte_crc_calculator_vbvb_sptr;

LTE_API lte_crc_calculator_vbvb_sptr lte_make_crc_calculator_vbvb ();

/*!
 * \brief LTE CRC Calculation Block
 * Block takes input to calculate CRC checksums with a provided Boost CRC calculator.
 * Passes on data bits to MIB decoder along with an antenna count indicator.
 */
class LTE_API lte_crc_calculator_vbvb : public gr_sync_block
{
	friend LTE_API lte_crc_calculator_vbvb_sptr lte_make_crc_calculator_vbvb ();

	lte_crc_calculator_vbvb ();

 public:
	~lte_crc_calculator_vbvb ();


	int work (int noutput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items);
		
		
 private:
        
        int d_expo[32];
        static const int D_BLOCK_LEN = 24;
        static const int D_BITS = 16;
	    static const uint16_t D_CRC_POLY   = 0x1021;
	    static const uint16_t D_INIT_STATE = 0x0000;
	    static const uint16_t D_FINAL_XOR1 = 0x0000;
	    static const uint16_t D_FINAL_XOR2 = 0xFFFF;
	    static const uint16_t D_FINAL_XOR4 = 0x5555;
	    static const bool D_REFLECT_IN  = false;
	    static const bool D_REFLECT_REM = false;

        
};

#endif /* INCLUDED_LTE_CRC_CALCULATOR_VBVB_H */

