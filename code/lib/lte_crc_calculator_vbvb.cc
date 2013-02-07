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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_io_signature.h>
#include <lte_crc_calculator_vbvb.h>
#include <cstdio>
#include <boost/crc.hpp>


lte_crc_calculator_vbvb_sptr
lte_make_crc_calculator_vbvb ()
{
	return lte_crc_calculator_vbvb_sptr (new lte_crc_calculator_vbvb ());
}


lte_crc_calculator_vbvb::lte_crc_calculator_vbvb ()
	: gr_sync_block ("crc_calculator_vbvb",
		gr_make_io_signature (1,1, sizeof(char)*40),
		gr_make_io_signature2 (2,2, sizeof(char)*24,sizeof(char)*1))
{
    // useful to calculate integer represention (pack bits)
	for (int i = 0 ;  i < 32 ; i++ ) {
	    d_expo[31-i]=std::pow(2,i);
	}
}


lte_crc_calculator_vbvb::~lte_crc_calculator_vbvb ()
{
}


int
lte_crc_calculator_vbvb::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
	const char *in = (const char *) input_items[0];
	char *out1  = (char *) output_items[0];
	char *out2 = (char *) output_items[1];

	// declare and initialize necessary arrays
	char block[D_BLOCK_LEN]; //contains input data
	memcpy(block,in,D_BLOCK_LEN);
	char rx_p[D_BITS];
	memcpy(rx_p,in+24,D_BITS);

    int rx_checksum = calculate_int_representation(rx_p, D_BITS);
	int block_dec = calculate_int_representation(block, D_BLOCK_LEN);

    int N_ant = calculate_crc_N_ant(block_dec, rx_checksum);

    // OUTPUT
    memcpy(out1, in, D_BLOCK_LEN);
    *out2 = N_ant;
	// Tell runtime system how many output items we produced.
	return 1;
}

inline int
lte_crc_calculator_vbvb::calculate_int_representation(char* bytes, int len)
{
    int value = 0;
	for(int i = 0 ; i < len ; i++){
	    value = value + bytes[(len-1)-i]*d_expo[31-i];
	}
	return value;
}

inline int
lte_crc_calculator_vbvb::calculate_crc_N_ant(int block_dec, int rx_checksum)
{
	//packed blocks
	char p_block[3] = {0};
	for (int i = 0 ; i < 3; i++ ){
	    p_block[i] = char( (block_dec >> 8*(2-i)) & 0xFF );
	}

	// Case: 1 antenna port
	boost::crc_optimal<D_BITS,D_CRC_POLY,D_INIT_STATE,D_FINAL_XOR1,D_REFLECT_IN,D_REFLECT_REM> crc1;
	crc1.process_block(p_block,&p_block[3]);

	// Case: 2 antenna ports
	boost::crc_optimal<D_BITS,D_CRC_POLY,D_INIT_STATE,D_FINAL_XOR2,D_REFLECT_IN,D_REFLECT_REM> crc2;
	crc2.process_block(p_block,&p_block[3]);

	// Case: 4 antenna ports
	boost::crc_optimal<D_BITS,D_CRC_POLY,D_INIT_STATE,D_FINAL_XOR4,D_REFLECT_IN,D_REFLECT_REM> crc4;
	crc4.process_block(p_block,&p_block[3]);

    // Decode N_ant
    char N_ant = 0;
    if (rx_checksum == crc1.checksum() ){N_ant = 1;}
    if (rx_checksum == crc2.checksum() ){N_ant = 2;}
	if (rx_checksum == crc4.checksum() ){N_ant = 4;}

	if (N_ant != 0){
        if (block_dec == 0 ){
            N_ant = 0; // next block will contain better information
        }
	}
    return N_ant;
}

