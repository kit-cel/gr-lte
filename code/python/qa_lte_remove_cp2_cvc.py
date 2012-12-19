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
#import lte_swig
import lte

class qa_remove_cp2_cvc (gr_unittest.TestCase):

    def setUp (self):
        print "qa_remove_cp2_cvc SetUp!"
        self.fftl = fftl = 2048
        cpl = 144*fftl/2048
        cpl0 = 160*fftl/2048
        self.slotl = slotl = 7*fftl+6*cpl+cpl0

        self.tb = gr.top_block ()
        
        #provide test data
        self.src = gr.noise_source_c(gr.GR_GAUSSIAN,1)
        self.head = gr.head( gr.sizeof_gr_complex, 10*slotl )
        self.tag = lte.pss_tagging_cc(fftl)
        # set up tagging block
        self.tag.set_half_frame_start(0)
        self.tag.set_N_id_2(1)
        self.tag.lock()
        
        # UUT
        self.rcp = lte.remove_cp2_cvc(fftl)
        
        # sinks
        self.snk = gr.vector_sink_c(fftl)
        self.tagsnc = gr.tag_debug(gr.sizeof_gr_complex*fftl, "remove_cp2_cvc")
        
        # connect blocks
        self.tb.connect(self.src, self.head, self.tag, self.rcp, self.snk )
        #self.tb.connect(self.rcp,self.tagsnc)
        

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        print "qa_remove_cp2_cvc TEST 001 Start!"
        #setup
        self.tag.set_half_frame_start(0)
        self.head.set_length(20*self.slotl)
        
        # set up fg
        self.tb.run ()
        
        # check data
        # test results
        data = self.snk.data()
        print len(data)/self.fftl
        print "qa_remove_cp2_cvc TEST 001 End!\n"
        
    def test_002_data(self):
        print "qa_remove_cp2_cvc TEST 002 Start!"
        #setup
        hf_start = 1*self.slotl+815
        print "start = " + str(hf_start)
        self.tag.set_half_frame_start(hf_start)
        self.head.set_length(20*self.slotl+495)
        
        
        # execute
        self.tb.run()

        # test results
        print "qa_remove_cp2_cvc TEST 002 End!"
        

if __name__ == '__main__':
    gr_unittest.main ()
