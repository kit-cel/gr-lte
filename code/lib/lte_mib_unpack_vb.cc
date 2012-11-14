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
		//gr_make_io_signature (1,1,sizeof(char)*1 )),
		d_SFN(-1),
        d_cell_id(-1),
        d_N_ant(-1),
        d_N_rb_dl(-1),
        d_phich_dur(-1),
        d_phich_res(-1.0),
        d_mib_count(0),
        d_work_calls(0),
        d_timestamp(0)
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
	//char *out = (char *) output_items[0];
	
	d_work_calls++;
	//tag_propagation_policy_t tpp=tag_propagation_policy();
    //printf("%s START\tnoutput_items = %i\td_work_calls = %i\ttpp = \n",name().c_str(),noutput_items,d_work_calls, tpp);
   
	
	

    // get number of antenna ports
    char N_ant = *in2;
    //printf("lte_mib_unpack.N_ant = %i\n",N_ant);
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
        sfn_lsb = (value%16)/4;
        
        //printf("%s\t",name().c_str() );
		//printf("v.size() = %i,\tnitems_written(0) = %i\tkey = %s\tsrcid = %s\tvalue = %ld\n",int(v.size()), int(nitems_read(0)), key.c_str(), srcid.c_str(), value);
	}

    
    
    // Get bytes representing MIB and get information about it.
    char mib[24];
    memcpy(mib,in1,24);
    
    int irb = 4*mib[0]+2*mib[1]+1*mib[2];
    //printf("irb  = %i\t%i %i %i\n",irb,int(mib[1]),int(mib[2]),int(mib[3]) );
    if       (irb == 0){d_N_rb_dl = 6 ;}
    else if (irb == 1){d_N_rb_dl = 15 ;}
    else if (irb == 2){d_N_rb_dl = 25 ;}
    else if (irb == 3){d_N_rb_dl = 50 ;}
    else if (irb == 4){d_N_rb_dl = 75 ;}
    else if (irb == 5){d_N_rb_dl = 100;}
    
    d_phich_dur = mib[3];

    int ipr = 2*mib[4]+1*mib[5];
    if       (ipr == 0){d_phich_res = 1.0/6.0;}
    else if (ipr == 1){d_phich_res = 1.0/2.0;}
    else if (ipr == 2){d_phich_res = 1.0;}
    else if (ipr == 3){d_phich_res = 2.0;}
    
    int sfn_msb = 0;
    for (int i = 0 ; i < 8 ; i++ ){
        sfn_msb = sfn_msb + mib[i+6]*std::pow(2,(9-i));
    }
    int sfn = sfn_msb+sfn_lsb;
    if(d_SFN != sfn){
        printf("Decoded %i\tN_ant = %i\tN_rb_dl = %i\tdiff = %i\n",sfn,d_N_ant,d_N_rb_dl,sfn-d_SFN);
        d_SFN_vec.push_back(sfn);
        d_mib_count++;
    /*    if(sfn < 30){
            time_t mytime = time(NULL);
            printf("%s\truntime = %ld\n", name().c_str(), mytime-d_timestamp );
            d_timestamp = mytime;
        }*/
    }
    d_SFN =sfn;
    
    

	// Tell runtime system how many output items we produced.
	return 1;
}

