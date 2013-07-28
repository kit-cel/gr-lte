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

class decode_pbch_vcvf(gr.hier_block2):
    """
    docstring for block decode_pbch_vcvf
    """
    def __init__(self):
        gr.hier_block2.__init__(self,
            "decode_pbch_vcvf",
            gr.io_signature(3, 3, gr.sizeof_gr_complex*240),  # Input signature
            gr.io_signature(1, 1, gr.sizeof_float*120)) # Output signature
        self.message_port_register_hier_out("cell_id")
        # Define blocks and connect them
        style = "tx_diversity"
        vlen = 240
        self.pre_decoder_1ant = lte.pre_decoder_vcvc(1, vlen, style)
        self.pre_decoder_2ant = lte.pre_decoder_vcvc(2, vlen, style)
        self.demapper_1ant = lte.layer_demapper_vcvc(1, vlen, style)
        self.demapper_2ant = lte.layer_demapper_vcvc(2, vlen, style)
        self.interleave = gr.interleave(vlen*gr.sizeof_gr_complex)
        self.demod = lte.qpsk_soft_demod_vcvf(vlen)
        self.descr = lte.descrambling_vfvf()
        
        self.msg_connect(self, "cell_id", self.descr, "cell_id")

        self.connect( (self, 0) , (self.pre_decoder_1ant, 0) )
        self.connect( (self, 1) , (self.pre_decoder_1ant, 1) )
        self.connect( (self, 0) , (self.pre_decoder_2ant, 0) )
        self.connect( (self, 1) , (self.pre_decoder_2ant, 1) )
        self.connect( (self, 2) , (self.pre_decoder_2ant, 2) )
        
        self.connect( self.pre_decoder_1ant, self.demapper_1ant, (self.interleave, 0) )
        self.connect( self.pre_decoder_2ant, self.demapper_2ant, (self.interleave, 1) )
        
        
        self.connect(self.interleave, self.descr, self)
