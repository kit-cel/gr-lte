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
#include <lte_pbch_demux_vcvc.h>
#include <cstdio>


lte_pbch_demux_vcvc_sptr
lte_make_pbch_demux_vcvc (int N_rb_dl) //int cell_id,
{
	return lte_pbch_demux_vcvc_sptr (new lte_pbch_demux_vcvc (N_rb_dl)); //cell_id,
}


lte_pbch_demux_vcvc::lte_pbch_demux_vcvc (int N_rb_dl) //int cell_id,
	: gr_block ("pbch_demux_vcvc",
		gr_make_io_signature (3,3, sizeof (gr_complex)*12*N_rb_dl),
		gr_make_io_signature (3,3, sizeof (gr_complex)*240)),
		d_cell_id(-1),
		d_N_rb_dl(N_rb_dl),
		d_sym_num(0)
{
    message_port_register_in(pmt::mp("cell_id"));
    set_msg_handler(pmt::mp("cell_id"), boost::bind(&lte_pbch_demux_vcvc::set_cell_id_msg, this, _1));


    int pbch_symb_size = 4*12*N_rb_dl;
    d_pbch_symbs = new gr_complex[pbch_symb_size];
	d_pbch_ce1_symbs = new gr_complex[pbch_symb_size];
	d_pbch_ce2_symbs = new gr_complex[pbch_symb_size];
}


lte_pbch_demux_vcvc::~lte_pbch_demux_vcvc ()
{
    delete[] d_pbch_symbs;
    delete[] d_pbch_ce1_symbs;
    delete[] d_pbch_ce2_symbs;
}


int
lte_pbch_demux_vcvc::general_work (int noutput_items,
			       gr_vector_int &ninput_items,
			       gr_vector_const_void_star &input_items,
			       gr_vector_void_star &output_items)
{
    const gr_complex *in1 = (const gr_complex *) input_items[0];
    const gr_complex *in2 = (const gr_complex *) input_items[1];
    const gr_complex *in3 = (const gr_complex *) input_items[2];
    gr_complex *out1 = (gr_complex *) output_items[0];
    gr_complex *out2 = (gr_complex *) output_items[1];
    gr_complex *out3 = (gr_complex *) output_items[2];



	// get smallest number of input items
    int ninitems=ninput_items[0];
    for(int i = 1 ; i < 3 ; i++){
        if(ninitems > ninput_items[i]){
            ninitems = ninput_items[i];
        }
    }
    if (ninitems > noutput_items){
        //printf("ninitems = %i\t>\t%i = noutput_items\n",ninitems,noutput_items);
        ninitems = noutput_items;
    }
    // No data is processed as long as the cell_id is not available
    if(d_cell_id < 0){
        consume_each(ninitems);
        return noutput_items;
    }



  	// set noutput_items to zero. if output is produced, noutput_items is incremented.
	noutput_items = 0;

    // get function copy of class attributes
    int cell_id_mod3 = d_cell_id%3;
    int N_rb_dl = d_N_rb_dl;
    int sym_num = d_sym_num; // specifies symbol number within a frame.
  	int pbch_symb_size = 4*12*N_rb_dl;

    //Read vector
    std::vector <gr_tag_t> v;
    get_tags_in_range(v,0,nitems_read(0),nitems_read(0)+ninitems);
    if(v.size() > 0){
	    sym_num = int(pmt::pmt_to_long(v[0].value) );
	}

/*
    //printf("%s is supposed to read stream tags here!! v.size() = %i\n",name().c_str(), int(v.size()) );
    for(int i = 0 ; i < v.size() ; i++ ){
        uint64_t offset = v[i].offset;
        std::string key = pmt::pmt_symbol_to_string(v[i].key);
        long value = pmt::pmt_to_long(v[i].value);
        std::string srcid = pmt::pmt_symbol_to_string(v[i].srcid);

        //printf("%s\t",name().c_str() );
		//printf("v.size() = %i,\tnitems_read(0) = %i\tkey = %s\tvalue = %ld\n",int(v.size()), int(nitems_read(0)), key.c_str(), value);
	}
*/

    //This loop searches for the REs with the PBCH and copies them to the output stream.
    for (int i = 0 ; i < ninitems ; i++ ) {
        //printf("%i\tinput buffers\t1: %i\t2: %i\t3: %i\tnoutput_items = %i\tsym_num = %i\n",i,ninput_items[0],ninput_items[1],ninput_items[2],noutput_items,sym_num);

        if(sym_num==7){
            if (ninitems-i < 4){
                //printf("%s call to work not enough input items left\n", name().c_str() );
                ninitems=ninitems-(i+1);
                break;
            }

            int idx = 0;
            for (int c = 0 ; c < 12*N_rb_dl ; c++ ) {
                if ( cell_id_mod3 != c%3 ){
                    out1[idx]    = in1[c];
                    out1[idx+48] = in1[c+72];
                    out2[idx]    = in2[c];
                    out2[idx+48] = in2[c+72];
                    out3[idx]    = in3[c];
                    out3[idx+48] = in3[c+72];
                    idx++;
                }
            }
            memcpy(out1+96, in1+144, 144*sizeof(gr_complex) );
            memcpy(out2+96, in2+144, 144*sizeof(gr_complex) );
            memcpy(out3+96, in3+144, 144*sizeof(gr_complex) );

            //printf("%s has produced output!\n", name().c_str() );
            noutput_items++;
            out1+=240;
            out2+=240;
            out3+=240;
        }



        // update work values for next symbol
        if (sym_num < 139){
            sym_num++;
        }
        else{
            sym_num = 0;
        }
        in1+=12*N_rb_dl;
        in2+=12*N_rb_dl;
        in3+=12*N_rb_dl;
    }





    // Tell runtime system how many input items we consumed on
    // each input stream.
    consume_each (ninitems);

    // set d_sym_num to new value
    d_sym_num = sym_num;

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

void
lte_pbch_demux_vcvc::set_cell_id_msg(pmt::pmt_t msg)
{
    int cell_id = int(pmt::pmt_to_long(msg));
    //printf("********%s INPUT MESSAGE***************\n", name().c_str() );
    //printf("\t%i\n", cell_id);
    //printf("********%s INPUT MESSAGE***************\n", name().c_str() );
    set_cell_id(cell_id);
}

void
lte_pbch_demux_vcvc::set_cell_id(int id)
{
    d_cell_id = id;
    printf("%s\t\tset_cell_id = %i\n", name().c_str(), d_cell_id );
}






