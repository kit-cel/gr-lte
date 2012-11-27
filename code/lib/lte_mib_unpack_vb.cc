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
#include <lte_mib_unpack_vb.h>
#include <cstdio>


lte_mib_unpack_vb_sptr
lte_make_mib_unpack_vb ()
{
	return lte_mib_unpack_vb_sptr (new lte_mib_unpack_vb ());
}


lte_mib_unpack_vb::lte_mib_unpack_vb ()
	: gr_sync_block ("mib_unpack_vb",
		gr_make_io_signature2 (2,2, sizeof (char)*24 , sizeof(char)*1 ),
		gr_make_io_signature (0,0,0)),
		d_SFN(-1),
        d_cell_id(-1),
        d_N_ant(-1),
        d_N_rb_dl(-1),
        d_phich_dur(-1),
        d_phich_res(-1.0),
        d_mib_count(0),
        d_work_calls(0)
{   
}


lte_mib_unpack_vb::~lte_mib_unpack_vb ()
{
}


int
lte_mib_unpack_vb::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
	const char *in1 = (const char *) input_items[0];
	const char *in2 = (const char *) input_items[1];
	
	d_work_calls++;	

    // get number of antenna ports
    char N_ant = *in2;
    if (N_ant == 0){
        return 1; // if N_ant = 0 CRC check failed --> MIB is unusable --> dismiss it.
    }
    else{
        d_N_ant = N_ant;
        //d_mib_count++;
    }
    // This part of work is only executed when N_ant != 0. Otherwise CRC check failed --> MIB data invalid anyways
    
    // get stream tags
    int sfn_lsb = 0;
    std::vector <gr_tag_t> v;
    get_tags_in_range(v,0,nitems_read(0),nitems_read(0)+1);
    if (v.size() > 0){
        //printf("lte_mib_unpack READ TAGS!!!\n");
        uint64_t offset = v[0].offset;
        std::string key = pmt::pmt_symbol_to_string(v[0].key);
        long value = pmt::pmt_to_long(v[0].value);
        std::string srcid = pmt::pmt_symbol_to_string(v[0].srcid);
        sfn_lsb = (value%16)/4; // 32 consecutive vectors (16 with soft-combining, then 16 without)
	}
   
    // Get bytes representing MIB and get information about it.
    char mib[24];
    memcpy(mib,in1,24);
    
    //Decode system bandwidth (number of resource blocks)
    int irb = 4*mib[0]+2*mib[1]+1*mib[2];
    switch (irb){
        case 0: d_N_rb_dl = 6   ; break;
        case 1: d_N_rb_dl = 15  ; break;
        case 2: d_N_rb_dl = 25  ; break;
        case 3: d_N_rb_dl = 50  ; break;
        case 4: d_N_rb_dl = 75  ; break;
        case 5: d_N_rb_dl = 100 ; break;
    }
    
    //Decode PHICH duration
    d_phich_dur = mib[3];
    
    //Decode PHICH resources
    int ipr = 2*mib[4]+1*mib[5];
    switch (ipr){
        case 0: d_phich_res = 1.0/6.0; break;
        case 1: d_phich_res = 1.0/2.0; break;
        case 2: d_phich_res = 1.0;     break;
        case 3: d_phich_res = 2.0;     break;
    }

    //Decode SFN MSBs    
    int sfn_msb = 0;
    for (int i = 0 ; i < 8 ; i++ ){
        sfn_msb = sfn_msb + mib[i+6]*std::pow(2,(9-i));
    }
    //Calculate SFN with MSB and LSB part
    int sfn = sfn_msb+sfn_lsb;
    
    //Output if new SFN is decoded
    if(d_SFN != sfn){
        printf("SFN = %i\tdiff = %i\t",sfn, sfn-d_SFN);
        printf("(N_ant=%i N_rb_dl=%i PHICH: dur=%i res=%1.2f)\n",d_N_ant,d_N_rb_dl,d_phich_dur,d_phich_res);
        d_SFN_vec.push_back(sfn);
        d_mib_count++;
    }
    //Now set attribute SFN
    d_SFN =sfn;
    
	// Tell runtime system how many output items we produced.
	return 1;
}

