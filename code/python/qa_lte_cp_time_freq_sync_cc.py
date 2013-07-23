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
import lte as lte_swig
import scipy.io
import time
import os
from pylab import *

class qa_cp_time_freq_sync_cc (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        
        fftl = 512
        
        
        self.intu = intu = [1.0] * 100 * fftl        
        self.src = gr.vector_source_c(intu,False,1)
        
        self.sync = lte_swig.cp_time_freq_sync_cc(fftl)
        
        self.head = gr.head(gr.sizeof_gr_complex,500000)
        
        self.snk = gr.vector_sink_c(1)
        
        self.tb.connect(self.src, self.head, self.sync, self.snk)
        

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        self.tb.run ()
        # check data
        
        res = self.snk.data()
        self.assertComplexTuplesAlmostEqual(res,self.intu[0:500000])


if __name__ == '__main__':
    gr_unittest.main ()
