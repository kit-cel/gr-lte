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
		d_unchanged_decodings(0),
        d_work_calls(0)
{
    d_port_N_ant = pmt::pmt_string_to_symbol("N_ant");
    d_port_N_rb_dl = pmt::pmt_string_to_symbol("N_rb_dl");
    d_port_phich_duration = pmt::pmt_string_to_symbol("phich_duration");
    d_port_phich_resources = pmt::pmt_string_to_symbol("phich_resources");
    d_port_SFN = pmt::pmt_string_to_symbol("SFN");

    message_port_register_out(d_port_N_ant);
    message_port_register_out(d_port_N_rb_dl);
    message_port_register_out(d_port_phich_duration);
    message_port_register_out(d_port_phich_resources);
    message_port_register_out(d_port_SFN);

    d_state_info.N_ant = 0;
    d_state_info.N_rb_dl = 0;
    d_state_info.phich_duration = -1;
    d_state_info.phich_resources = -1.0f;
}


lte_mib_unpack_vb::~lte_mib_unpack_vb ()
{
    printf("%s\tdecoding_rate = %1.3f\n",name().c_str(), get_decoding_rate() );
}


int
lte_mib_unpack_vb::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
	const char *in1 = (const char *) input_items[0];
	const char *in2 = (const char *) input_items[1];

    char N_ant = *in2;
    switch (N_ant){
        case 0: return 1; break;
        default: d_state_info.N_ant = N_ant;
    }

    char mib[24];
    memcpy(mib,in1,24);
    decode_mib(mib);

	return 1;
}

inline void
lte_mib_unpack_vb::decode_mib(char* mib)
{
    bool unchanged = decode_state_mib(mib);
    if(!unchanged && d_SFN > -1){
        return;
    }

    int sfn = decode_sfn(mib+6);
    int diff = ((sfn+1024)-d_SFN)%1024;
    d_SFN = sfn;

    if(diff > 0){
        printf("SFN = %i\tdiff = %i\t", d_SFN, diff );
        d_state_info.print_values();
        send_sfn();
    }
}

inline bool
lte_mib_unpack_vb::decode_state_mib(char* mib)
{
    bool unchanged = true;
    int N_rb_dl = decode_N_rb_dl(mib);
    int phich_dur = mib[3];
    float phich_res = decode_phich_resources(mib+4);

    if(N_rb_dl != d_state_info.N_rb_dl){
        printf("MIB state changed!\t N_rb_dl = %i\n", N_rb_dl);
        unchanged = false;
    }
    if(phich_dur != d_state_info.phich_duration){
        printf("MIB state changed!\t phich_duration = %i\n", phich_dur);
        unchanged = false;
    }
    if(phich_res != d_state_info.phich_resources){
        printf("MIB state changed!\t phich_resources = %1.2f\n", phich_res);
        unchanged = false;
    }

    if(!unchanged && d_unchanged_decodings < 10){
        d_state_info.N_rb_dl = N_rb_dl;
        d_state_info.phich_duration = phich_dur;
        d_state_info.phich_resources = phich_res;
        d_unchanged_decodings = 0;
        send_state_mib();
    }
    else{
        d_unchanged_decodings++;
    }
    return unchanged;
}

inline int
lte_mib_unpack_vb::decode_N_rb_dl(char* mib)
{
    int irb = 4*mib[0]+2*mib[1]+1*mib[2];
    int N_rb_dl = 0;
    switch (irb){
        case 0: N_rb_dl = 6   ; break;
        case 1: N_rb_dl = 15  ; break;
        case 2: N_rb_dl = 25  ; break;
        case 3: N_rb_dl = 50  ; break;
        case 4: N_rb_dl = 75  ; break;
        case 5: N_rb_dl = 100 ; break;
        default: N_rb_dl = 0;
    }
    return N_rb_dl;
}

inline float
lte_mib_unpack_vb::decode_phich_resources(char* mib)
{
    int ipr = 2*mib[0]+1*mib[1];
    float phich_res = 0.0f;
    switch (ipr){
        case 0: phich_res = 1.0f/6.0f; break;
        case 1: phich_res = 1.0f/2.0f; break;
        case 2: phich_res = 1.0f;     break;
        case 3: phich_res = 2.0f;     break;
        default : phich_res = -1.0f;
    }
    return phich_res;
}

inline int
lte_mib_unpack_vb::decode_sfn(char* mib)
{
    int sfn_lsb = extract_sfn_lsb_from_tag();
    int sfn_msb = 0;
    for (int i = 0 ; i < 8 ; i++ ){
        sfn_msb = sfn_msb + mib[i]*std::pow(2,(9-i));
    }
    int sfn = sfn_msb + sfn_lsb;

    if(d_SFN < 0){d_SFN = sfn;}
    if(d_SFN != sfn){
        int diff = ((sfn+1024)-d_SFN)%1024;
        for(int i = 1; i < diff ; i++ ){
            d_SFN_vec.push_back(-1);
        }
        d_SFN_vec.push_back(sfn);
    }

    return sfn;
}

inline int
lte_mib_unpack_vb::extract_sfn_lsb_from_tag()
{
    int sfn_lsb = 0;
    std::vector <gr_tag_t> v;
    get_tags_in_range(v,0,nitems_read(0),nitems_read(0)+1);
    if (v.size() > 0){
        long value = pmt::pmt_to_long(v[0].value);
        sfn_lsb = (value%16)/4; // 32 consecutive vectors (16 with soft-combining, then 16 without)
	}
	return sfn_lsb;
}

float
lte_mib_unpack_vb::get_decoding_rate()
{
    int count_success = 0;
    for(int i = 0; i < d_SFN_vec.size(); i++){
        if(d_SFN_vec[i] > -1){
            count_success++;
        }
    }
    float success_rate = float(count_success) / float(d_SFN_vec.size() );
    return success_rate;
}


void
lte_mib_unpack_vb::send_state_mib()
{
    pmt::pmt_t msg_ant = pmt::pmt_cons(d_port_N_ant, pmt::pmt_from_long(long(d_state_info.N_ant) ) );
    pmt::pmt_t msg_rb = pmt::pmt_cons(d_port_N_rb_dl, pmt::pmt_from_long(long(d_state_info.N_rb_dl) ) );
    pmt::pmt_t msg_dur = pmt::pmt_cons(d_port_phich_duration, pmt::pmt_from_long(long(d_state_info.phich_duration) ) );
    pmt::pmt_t msg_res = pmt::pmt_cons(d_port_phich_resources, pmt::pmt_from_long(long(d_state_info.phich_resources) ) );

    message_port_pub( d_port_N_ant, msg_ant );
    message_port_pub( d_port_N_rb_dl, msg_rb );
    message_port_pub( d_port_phich_duration, msg_dur );
    message_port_pub( d_port_phich_resources, msg_res );
}

inline void
lte_mib_unpack_vb::send_sfn()
{
    pmt::pmt_t msg = pmt::pmt_cons(d_port_SFN, pmt::pmt_from_long(long(d_SFN) ) );
    message_port_pub( d_port_SFN, msg );
}








