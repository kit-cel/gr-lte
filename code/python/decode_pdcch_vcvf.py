#!/usr/bin/env python
# 
# Copyright 2013 <+YOU OR YOUR COMPANY+>.
# 
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

from gnuradio import gr

class decode_pdcch_vcvf(gr.hier_block2):
    """
    Decode PDCCH
    This block sums up all blocks necessary for decoding of PDCCH.
    Also the blocks are parameterized accordingly.
    """
    def __init__(self, tag_key, sf_key, msg_cell_id_name, msg_N_ant_name, msg_cfi_name):
        gr.hier_block2.__init__(self,
            "decode_pdcch_vcvf",
            gr.io_signature(3, 3, gr.sizeof_gr_complex),  # Input signature
            gr.io_signature(1, 1, gr.sizeof_float)) # Output signature
            
        # Define blocks and connect them
        self.connect()
