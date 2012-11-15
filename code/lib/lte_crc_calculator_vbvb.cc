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
    //printf("crc_calculator_vbvb\n");

	for (int i = 0 ;  i < 32 ; i++ ) {
	    d_expo[31-i]=std::pow(2,i);
	    //printf("%i\n",d_expo[31-i]);
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
	char *out = (char *) output_items[0];
	char *out2 = (char *) output_items[1];
	//printf("crc_calculator_vbvb::work START\n");
	
	// declare and initialize necessary arrays
	char block[D_BLOCK_LEN]; //contains input data
    //memset(block,0,D_BLOCK_LEN);
	memcpy(block,in,D_BLOCK_LEN);
	char rx_p[D_BITS];
	memcpy(rx_p,in+24,D_BITS);

    //calculate integer representation of RX checksum
    int rx_checksum = 0;   	
	for(int i = 0 ; i < D_BITS ; i++){
	    rx_checksum = rx_checksum + rx_p[15-i]*d_expo[31-i];
	}
	
	//calculate integer representation of input bits.
	int block_dec = 0;
	for(int i = 0 ; i < D_BLOCK_LEN ; i++){
	    block_dec = block_dec + block[23-i]*d_expo[31-i];
	}
	
	//print calculated and expected values
	//int expected = 6928384;
	//printf("block_dec = %i\nexpected  = %i\nrx_checksum = %i\n",block_dec,expected,rx_checksum);
	
	//packed blocks
	char p_block[3] = {0};
	for (int i = 0 ; i < 3; i++ ){
	    p_block[i] = char( (block_dec >> 8*(2-i)) & 0xFF );
	    
	}
	
	//declare, initialize and calculate checksums with different final_xor.
	boost::crc_optimal<D_BITS,D_CRC_POLY,D_INIT_STATE,D_FINAL_XOR1,D_REFLECT_IN,D_REFLECT_REM> crc1;
	crc1.process_block(p_block,&p_block[3]);
	
	boost::crc_optimal<D_BITS,D_CRC_POLY,D_INIT_STATE,D_FINAL_XOR2,D_REFLECT_IN,D_REFLECT_REM> crc2;
	crc2.process_block(p_block,&p_block[3]);
	
	boost::crc_optimal<D_BITS,D_CRC_POLY,D_INIT_STATE,D_FINAL_XOR4,D_REFLECT_IN,D_REFLECT_REM> crc4;
	crc4.process_block(p_block,&p_block[3]);
	
	uint16_t checksum = crc2.checksum();
	
	
	//printf("checksum1   = %i\n", int(crc1.checksum() ));
    //printf("checksum2   = %i\n", int(crc2.checksum() ));
    //printf("checksum4   = %i\n", int(crc4.checksum() ));
    
    char N_ant = 0;
    if (rx_checksum == crc1.checksum() ){N_ant = 1;}
    if (rx_checksum == crc2.checksum() ){N_ant = 2;}
	if (rx_checksum == crc4.checksum() ){N_ant = 4;}
	
	
	
	if (N_ant != 0){
/*
	    printf("%s\tN_ant = %i\n", name().c_str(), int(N_ant) );
	    printf("rx_checksum = %i\n", rx_checksum);
	    printf("checksum1   = %i\n", int(crc1.checksum() ));
        printf("checksum2   = %i\n", int(crc2.checksum() ));
        printf("checksum4   = %i\n", int(crc4.checksum() ));
        printf("block_dec   = %i\n", block_dec);
*/        
        if (block_dec == 0 ){
            N_ant = 0; // next block will contain better information
        }
	    
	    //printf("\n");
	}
	/*
	for (int i = 0 ; i < 16 ; i++){
	    *(out+(15-i)) = (checksum >> i) & 0x1;
	    //printf("%i  ", int(*(out+(15-i))) );
	}
    printf("\n");
    */
    
    // Copy MIB part to output
    memcpy(out,in,D_BLOCK_LEN);
    
    //pass number of antenna ports the next block
    *out2 = N_ant;
    
	//tag_propagation_policy_t tpp=tag_propagation_policy();
	//printf("%s\ttag_propagation_policy = %i\n" , name().c_str(), tpp );
    
/*    
    std::vector <gr_tag_t> v;
    get_tags_in_range(v,0,nitems_written(0),nitems_written(0)+1);
	if (v.size() > 0){
		uint64_t offset = v[0].offset;
		std::string key = pmt::pmt_symbol_to_string(v[0].key);
		long value = pmt::pmt_to_long(v[0].value);
		std::string srcid = pmt::pmt_symbol_to_string(v[0].srcid);
        printf("%s\t",name().c_str() );
		printf("v.size() = %i,\tnitems_written(0) = %i\tkey = %s\tsrcid = %s\tvalue = %ld\n", int(v.size()), int(nitems_written(0)), key.c_str(), srcid.c_str(), value);
	}
*/
    
    
    
	// Tell runtime system how many output items we produced.
	//printf("crc_calculator_vbvb::work END\n");
	return 1;
}


