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
import numpy
import scipy.io
from pylab import *

class qa_sss_calc_vc (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        
        fftl = 512

        intu = [1.0] * 100 * fftl * 72
        self.src  = gr.vector_source_c(intu,False,72)
        self.tag = lte_swig.sss_tagging_cc(fftl)
        self.calc = lte_swig.sss_calc_vc(self.tag, fftl)
        
        self.tb.connect(self.src,self.calc)
        

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        self.tb.run ()
        # check data
        
        corr_vec = self.calc.get_corr_vec()
        
        #plot(corr_vec)
        #show()


if __name__ == '__main__':
    gr_unittest.main ()
