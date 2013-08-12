#!/usr/bin/env python
# 
# Copyright 2013 Communications Engineering Lab (CEL) / Karlsruhe Institute of Technology (KIT)
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
import lte

class decode_pcfich_vcm(gr.hier_block2):
    """
    This is a hierarchical block which includes all parameterized blocks to decode the PCFICH
    """
    def __init__(self, N_rb_dl, key, out_key, msg_cell_id_name, msg_N_ant_name, msg_cfi_name):
        gr.hier_block2.__init__(self,
            "decode_pcfich_vcm",
            gr.io_signature(3, 3, gr.sizeof_gr_complex * 12 * N_rb_dl),  # Input signature
            gr.io_signature(0, 0, 0)) # Output signature
        
        # register message ports        
        self.message_port_register_hier_out(msg_cell_id_name)
        self.message_port_register_hier_out(msg_N_ant_name)
        self.message_port_register_hier_in(msg_cfi_name)
        # define some variables
        cvlen = 16
        N_ant = 2 # dummy resetable during runtime
        style = "tx_diversity" # PCFICH only uses this value (no spatial multiplexing etc.)
        
        # instatiate blocks with there parameters
        self.demux = lte.pcfich_demux_vcvc(N_rb_dl, key, out_key, msg_cell_id_name)
        self.predecoder = lte.pre_decoder_vcvc(N_ant, cvlen, style)
        self.demapper = lte.layer_demapper_vcvc(N_ant, cvlen, style)
        self.qpsk = lte.qpsk_soft_demod_vcvf(cvlen)
        self.descrambler = lte.pcfich_descrambler_vfvf(out_key, msg_cell_id_name)
        self.unpack = lte.cfi_unpack_vf(out_key, msg_cfi_name)

        # Connect defined blocks
        self.connect((self, 0), (self.demux, 0), (self.predecoder, 0))
        self.connect((self, 1), (self.demux, 1), (self.predecoder, 1))
        self.connect((self, 2), (self.demux, 2), (self.predecoder, 2))
        self.connect(self.predecoder, self.demapper, self.qpsk, self.descrambler, self.unpack)
        
        # Don't forget the message ports
        self.msg_connect(self, msg_N_ant_name, self.predecoder, msg_N_ant_name)
        self.msg_connect(self, msg_N_ant_name, self.demapper, msg_N_ant_name)
        self.msg_connect(self, msg_cell_id_name, self.demux, msg_cell_id_name)
        self.msg_connect(self, msg_cell_id_name, self.descrambler, msg_cell_id_name)
        self.msg_connect(self.unpack, msg_cfi_name, self, msg_cfi_name)
        
    def set_cell_id(self, cell_id):
        # use this method for hier block access
        self.demux.set_cell_id(cell_id)
        self.descrambler.set_cell_id(cell_id)
        
    def set_N_ant(self, N_ant):
        # use this method for hier block access
        self.predecoder.set_N_ant(N_ant)
        self.demapper.set_N_ant(N_ant)        
        
        
        
        
        