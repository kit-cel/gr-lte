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
import numpy
import scipy.io

class qa_extract_occupied_tones_vcvc (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        
        N_rb_dl = 6
        fftl = 512
        
        mod=scipy.io.loadmat('/home/demel/exchange/matlab_mat_f.mat') 
        mat_u1=tuple(mod['mat_f'].flatten())
        mat_d=range(len(mat_u1))
        for idx, val in enumerate(mat_u1):
            mat_d[idx]=val
        intu1=tuple(mat_d)
        print len(intu1)/fftl
        
        self.src = gr.vector_source_c(intu1,False,fftl)
        self.ext = lte_swig.extract_occupied_tones_vcvc(N_rb_dl,fftl)
        self.snk = gr.vector_sink_c(12*N_rb_dl)
        
        self.tb.connect(self.src,self.ext,self.snk)

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        self.tb.run ()
        # check data
        mod=scipy.io.loadmat('/home/demel/exchange/matlab_frame.mat') 
        mat_u1=tuple(mod['frame_mat'].flatten())
        mat_d=range(len(mat_u1))
        for idx, val in enumerate(mat_u1):
            mat_d[idx]=val
        outtu=tuple(mat_d)
        
        res=self.snk.data()
        print len(res)/(12*6)
        
        for i in range(len(res)/72):
            try:
                self.assertComplexTuplesAlmostEqual(res[72*i:72*(i+1)],outtu[72*i:72*(i+1)],5)
                print str(i) + "\tres == outtu"
            except AssertionError:
                print str(i) + "\tres != outtu FAILED!!!"
                
        #for i in range(72):
        #    print str(res[i]) +"\t" + str(outtu[i])


if __name__ == '__main__':
    gr_unittest.main ()
