#!/usr/bin/env python
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
#

from gnuradio import gr, gr_unittest
import lte_swig

class qa_freq_estimate_c (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        
        
        fftl = 512
        waveform = gr.GR_SIN_WAVE
        freq = 0.0
        ampl = 1.0
        offset = 0.0
        cpl = 144 * fftl / 2048
        cpl0 = 160 * fftl / 2048
        slotl = 7 * fftl + 6 * cpl + cpl0
        samp_rate = slotl / 0.0005

        self.sig = gr.sig_source_c(samp_rate,waveform,freq,ampl,offset)
        self.head = gr.head(gr.sizeof_gr_complex, 100000)
        
        print "amplitude\t" + str(self.sig.amplitude())
        print "frequency\t" + str(self.sig.frequency())
        print "name\t" + str(self.sig.name())
        print "offset\t" + str(self.sig.offset())
        print "samp_rate\t" + str(self.sig.sampling_freq())
        print "waveform\t" + str(self.sig.waveform())
        
        #print type(self.sig)
        
        self.est = lte_swig.freq_estimate_c(self.sig,fftl)
        
        self.tb.connect(self.sig,self.head,self.est)

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        print "\nqa_freq_estimate_c RUN!"
        self.tb.run ()
        
        print "amplitude\t" + str(self.sig.amplitude())
        print "frequency\t" + str(self.sig.frequency())
        print "name\t" + str(self.sig.name())
        print "offset\t" + str(self.sig.offset())
        print "samp_rate\t" + str(self.sig.sampling_freq())
        print "waveform\t" + str(self.sig.waveform())

        
        # check data


if __name__ == '__main__':
    gr_unittest.main ()
