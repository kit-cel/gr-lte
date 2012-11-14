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
import lte

class pss_sync_hier_cc(gr.hier_block2):
    def __init__(self, fftl):
        """
        This hierarchical block has a complex input and complex output stream. The actual data remains unchanged.
        The N_id_2 or cell ID number is calculated.
        Based on the position of the PSS the frame structure is calculated on a half frame basis and tagged to the output stream together with N_id_2
        A tag with the slot number within a half frame is tagged at the beginning of every slot. (key = slot)
        A tag with the N_id_2 is tagged at the beginning of every half frame. (Corresponds to slot 0) (key = N_id_2)
        The input data must be tagged with information for symbols. (key = symbol)
        the tag propagation policy is set to TPP_DONT.
	    """
        gr.hier_block2.__init__(self, "pss_sync_hier_cc",
				gr.io_signature(1,1, gr.sizeof_gr_complex),  # Input signature
				gr.io_signature(1,1, gr.sizeof_gr_complex)) # Output signature

        
        # This is a fixed value due to the fact that the PSS is always located at the 62 center carriers.
        N_rb_dl = 6;
        
        
        
        self.tag = lte.pss_tagging_cc(fftl)
        
        self.sel  = lte.pss_selector_cvc(fftl)
        self.fft  = gr.fft_vcc(fftl,True,window.rectangular(fftl),False,1)
        self.ext  = lte.extract_occupied_tones_vcvc(N_rb_dl,fftl)
        self.calc = lte.pss_calc_vc(self.tag,self.sel,fftl)
        
        # Define blocks
        self.connect(self, self.tag, self)
        self.connect(self, self.sel, self.fft, self.ext, self.calc)

