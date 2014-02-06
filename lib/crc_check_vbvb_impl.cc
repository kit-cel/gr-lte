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

#include <gnuradio/io_signature.h>
#include "crc_check_vbvb_impl.h"

#include <cstdio>

namespace gr {
  namespace lte {

    crc_check_vbvb::sptr
    crc_check_vbvb::make(int data_len, int final_xor, std::string name)
    {
      return gnuradio::get_initial_sptr
        (new crc_check_vbvb_impl(name, data_len, final_xor));
    }

    /*
     * The private constructor
     */
    crc_check_vbvb_impl::crc_check_vbvb_impl(std::string& name, int data_len, int final_xor)
      : gr::sync_block(name,
              gr::io_signature::make( 1, 1, sizeof(char)*(data_len+16)),
              gr::io_signature::make2( 2, 2, sizeof(char)*data_len, sizeof(char)*1 )),
              d_data_len(data_len),
              d_final_xor(final_xor)
    {
		//~ uint16_t final_xor1 = 0x0000;
	    //~ uint16_t final_xor2 = 0xFFFF;
	    //~ uint16_t final_xor4 = 0x5555;
	    //~ printf("xor1=%i\txor2=%i\txor4=%i\n", final_xor1, final_xor2, final_xor4);
	    //~ printf("given xor=%i\n", d_final_xor);   
	    //~ printf("%s\n", data_len%8==0 ? "valid data length":"invalid data length, must be a multiple of 8");
		for (int i = 0 ;  i < 16 ; i++ ) {
			d_exp[i]=std::pow(2,i);
		}
	}

    /*
     * Our virtual destructor.
     */
    crc_check_vbvb_impl::~crc_check_vbvb_impl()
    {
    }
    
    unsigned char
    crc_check_vbvb_impl::pack_byte(const char* unc)
    {
		unsigned char res = 0;
		for (int i = 0 ; i < 8 ; i++ ) {
			res += (unc[7-i]*d_exp[i]);
			//~ printf("%i ", (int)unc[i]);
		}
		//~ printf("0x%X\n", res);
		return res;
	}

    int
    crc_check_vbvb_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
        const char *in = (const char *) input_items[0];
        char *out0 = (char *) output_items[0];
		char *out1 = (char *) output_items[1];
		
		char bytes[d_data_len/8];
		for(int items = 0 ; items < noutput_items ; items++){

			for(int i = 0 ; i < d_data_len/8 ; i++){
				bytes[i] = pack_byte(in+8*i);
			}
			int rx_check = 0;
			for(int i = 0 ; i < 16 ; i++){
				rx_check += in[d_data_len+(15-i)] * d_exp[i];
			}
			lte_crc_16.reset();
			lte_crc_16.process_block(bytes, &bytes[d_data_len/8]);
			int checksum = lte_crc_16.checksum();
			checksum = checksum ^ d_final_xor; // bitwise XOR
			//~ printf("final_xor = %i\tchecksum result = %i\t expected = %i\n", d_final_xor, checksum, rx_check );
			
			memcpy(out0, in, d_data_len);
			if(checksum == rx_check){
				*out1 = 1;
			}
			else{
				*out1 = 0;
			}
			in += (d_data_len+16);
			out0 += d_data_len;
			out1++;
		}
        // Tell runtime system how many output items we produced.
        return noutput_items;
    }

  } /* namespace lte */
} /* namespace gr */

