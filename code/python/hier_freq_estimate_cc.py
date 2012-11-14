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

from gnuradio import gr
import lte_swig

class hier_freq_estimate_cc(gr.hier_block2):
    def __init__(self, fftl):
        """
        docstring
	    """
        gr.hier_block2.__init__(self, "hier_freq_estimate_cc",
				gr.io_signature(1,1, gr.sizeof_gr_complex),  # Input signature
				gr.io_signature(1,1, gr.sizeof_gr_complex)) # Output signature

        # Define blocks
        waveform = gr.GR_COS_WAVE
        wave_freq = 0.0
        ampl = 1.0
        offset = 0.0
        cpl = 144 * fftl / 2048
        cpl0 = 160 * fftl / 2048
        slotl = 7 * fftl + 6 * cpl + cpl0
        samp_rate = slotl / 0.0005
        #print fftl
        #print cpl
        #print cpl0
        #print slotl
        #print samp_rate
        self.sig   = gr.sig_source_c(samp_rate, waveform,wave_freq,ampl,offset)
        self.multi = gr.multiply_cc(1)
        self.est   = lte_swig.freq_estimate_c(self.sig,fftl)
        
        self.connect(self,(self.multi,0),self)
        self.connect(self.sig,(self.multi,1) )
        self.connect(self.multi,self.est ) 

