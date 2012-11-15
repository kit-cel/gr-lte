#!/usr/bin/env python
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
#

from gnuradio import gr, gr_unittest
import lte_swig
import numpy
import scipy.io
from pylab import *

class qa_sss_calc_vc (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        
        mod=scipy.io.loadmat('/home/demel/exchange/matlab_sss.mat') 
        mat_u1=tuple(mod['sss'].flatten())
        mat_d=range(len(mat_u1))
        for idx, val in enumerate(mat_u1):
            mat_d[idx]=val
        intu=tuple(mat_d)
        
        fftl = 512
        
        self.daemon = lte_swig.cell_id_daemon()
        
        # This is not yet a complete test! It's just for the purpose of testing the decoding and correlating.
        # Tag handling is not tested.
        self.src  = gr.vector_source_c(intu,False,72)
        
        self.tag = lte_swig.sss_tagging_cc(fftl)
        # calc sink block, which sets some attributes of other blocks.
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
