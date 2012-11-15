# 
# Copyright 2012 Johannes Demel
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

from gnuradio import gr, window
import lte_swig

class hier_sss_sync_cc(gr.hier_block2):
    def __init__(self, daemon, fftl):
        """
        docstring
        """
        gr.hier_block2.__init__(self, "hier_sss_sync_cc",
				gr.io_signature(1,1, gr.sizeof_gr_complex), # Input signature
				gr.io_signature(1,1, gr.sizeof_gr_complex)) # Output signature

        # input goes into 2 blocks: selector and tagging
        # 2 parallel streams: main (all data) right into tagging, sss symbol -> fft -> extract -> calc sss
        # required blocks:
        # selector
        # fft
        # resize (extract)
        # calc sss
        # tagging
        
        # SSS selector block
        self.sel = lte_swig.sss_selector_cvc(fftl)
        
        # in this block N_rb_dl is fixed. SSS is always mapped to those carriers.
        N_rb_dl = 6
        
        # fft block and matching extract block
        # input : vector with size: gr_complex * fftl
        # output: vector with size: gr_complex * 12 * N_rb_dl
        self.fft  = gr.fft_vcc(fftl,True,window.rectangular(fftl),False,1)
        self.ext  = lte_swig.extract_occupied_tones_vcvc(N_rb_dl,fftl)
        
        self.tag = lte_swig.sss_tagging_cc(fftl)
        # calc sink block, which sets some attributes of other blocks.
        self.calc = lte_swig.sss_calc_vc(self.tag, daemon, fftl)
        
        # Connect blocks
        self.connect(self, self.tag, self)
        self.connect(self, self.sel, self.fft, self.ext, self.calc )

               
        
        
        
    def get_cell_id(self):
        # The purpose of this function is to make the calculated cell ID easily available for other parts of the flowgraph.
        #print "sss.calc.get_cell_id()"
        return self.calc.get_cell_id()
        
        
        
        

