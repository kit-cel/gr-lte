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

class qa_remove_cp2_cvc (gr_unittest.TestCase):

    def setUp (self):
        print "qa_remove_cp2_cvc Start!"
        fftl = 2048
        cpl = 144*fftl/2048
        cpl0 = 160*fftl/2048
        slotl = 7*fftl+6*cpl+cpl0
        hf_start = 5*slotl+4711
        print "start = " + str(hf_start)
    
        self.tb = gr.top_block ()
        
        self.src = gr.noise_source_c(gr.GR_GAUSSIAN,1)
        self.head = gr.head( gr.sizeof_gr_complex, 30*slotl )
        self.tag = lte_swig.pss_tagging_cc(fftl)
        self.tag.set_half_frame_start(hf_start)
        self.tag.set_N_id_2(1)
        self.tag.lock()
        self.rcp = lte_swig.remove_cp2_cvc(fftl)
        self.snk = gr.vector_sink_c(fftl)
        
        self.tb.connect(self.src, self.head, self.tag, self.rcp, self.snk )
        

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        self.tb.run ()
        # check data
        print "qa_remove_cp2_cvc End!"

if __name__ == '__main__':
    gr_unittest.main ()
