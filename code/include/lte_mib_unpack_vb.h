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

#ifndef INCLUDED_LTE_MIB_UNPACK_VB_H
#define INCLUDED_LTE_MIB_UNPACK_VB_H

#include <lte_api.h>
#include <gr_sync_block.h>
#include <ctime>

class lte_mib_unpack_vb;
typedef boost::shared_ptr<lte_mib_unpack_vb> lte_mib_unpack_vb_sptr;

LTE_API lte_mib_unpack_vb_sptr lte_make_mib_unpack_vb ();

/*!
 * MIB unpack decodes all MIB parameters and makes them available. 
 * This block takes 2 inputs. 
 * A vector with the 24 Bits of the MIB.
 * An input with the decoded N_ant.
 * 
 * This block has public members to get access to the decoded MIB parameters.
 */
class LTE_API lte_mib_unpack_vb : public gr_sync_block
{
	friend LTE_API lte_mib_unpack_vb_sptr lte_make_mib_unpack_vb ();

	lte_mib_unpack_vb ();

 public:
	~lte_mib_unpack_vb ();

    

	int work (int noutput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items);
		
	int   get_SFN       (){return d_SFN;}
	std::vector<int> get_SFN_vec(){return d_SFN_vec;}
	int   get_cell_id   (){return d_cell_id;}	
    int   get_N_ant     (){return d_N_ant;}
	int   get_N_rb_dl   (){return d_N_rb_dl;}
	int   get_phich_dur (){return d_phich_dur;}
	float get_phich_res (){return d_phich_res;}
	int   get_mib_count (){return d_mib_count;}
	
 private:
    int d_SFN;
    int d_cell_id;
    int d_N_ant;
    int d_N_rb_dl;
    int d_phich_dur;
    float d_phich_res;
    int d_mib_count;
    std::vector<int> d_SFN_vec;
    
    int d_work_calls;
};

#endif /* INCLUDED_LTE_MIB_UNPACK_VB_H */

