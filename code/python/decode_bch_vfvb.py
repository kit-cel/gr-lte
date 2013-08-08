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

class decode_bch_vfvb(gr.hier_block2):
    def __init__(self, ):

        gr.hier_block2.__init__(self, "decode_bch_vfvb",
				gr.io_signature(1,1, gr.sizeof_float*120),  # Input signature
				gr.io_signature2(2,2, gr.sizeof_char*24, gr.sizeof_char)) # Output signature
        
        # Define blocks        
        self.rate_unmatch = lte.rate_unmatch_vff()
        self.viterbi = lte.viterbi_vfvb()
        self.crc = lte.crc_calculator_vbvb()

        self.connect(self, self.rate_unmatch, self.viterbi, (self.crc, 0), (self, 0))
        self.connect((self.crc, 1), (self, 1))

