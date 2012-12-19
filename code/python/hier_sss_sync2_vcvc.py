# 
# Copyright 2012 Communications Engineering Lab (CEL) / Karlsruhe Institute of Technology (KIT)
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

class hier_sss_sync2_vcvc(gr.hier_block2):
    def __init__(self, daemon, fftl):
        """
        This is a hierarchical block to structure the SSS synchronization
        This version of the SSS Sync is meant to operate after the FFT
	    """
        gr.hier_block2.__init__(self, "hier_sss_sync2_vcvc",
				gr.io_signature(1, 1, (gr.sizeof_gr_complex)*fftl),  # Input signature
				gr.io_signature(1, 1, (gr.sizeof_gr_complex)*fftl)) # Output signature

        # Define blocks
        N_rb_dl = 6
        self.tag = lte.sss_tagging2_vcvc(fftl)
        self.ext = lte.extract_occupied_tones_vcvc(N_rb_dl, fftl)
        self.calc = lte.sss_calc2_vc(self.tag, daemon, fftl)
        
        self.connect(self, self.tag, self)
        self.connect(self, self.ext, self.calc)

