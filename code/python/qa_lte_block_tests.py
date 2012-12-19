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

from gnuradio import gr, gr_unittest, trellis, window, blks2
import lte_swig
import lte
import numpy
import scipy.io
import cmath

class qa_block_tests (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        
        print "qa_block_tests"
        
        fftl = 512
        I = 768
        D = 1000
        
        #mod=scipy.io.loadmat('/home/demel/exchange/matlab_mat_t.mat') 
        #mat_u1=tuple(mod['mat_t'].flatten())
        #mat_d=range(len(mat_u1))
        #for idx, val in enumerate(mat_u1):
        #    mat_d[idx]=val
        #intu1=tuple(mat_d)
        #print len(intu1)/fftl        
        
        # source and conversion from float to char
        #self.src  = gr.vector_source_c(intu1,False,fftl)
        
        
        #self.fft = gr.fft_vcc(fftl,True,window.rectangular(fftl),False,1)
        
        
        #self.snk = gr.vector_sink_c(fftl)Messung_LTE_2012-05-23_12:49:57_xae
        self.src = gr.file_source(gr.sizeof_gr_complex,'/home/demel/Dokumente/Messungen/Messung_LTE_2012-05-23_12:49:57_xae.dat', False)
        self.resample = blks2.rational_resampler_ccc(I, D)
        self.snk = gr.file_sink(gr.sizeof_gr_complex, '/home/demel/Dokumente/LTE_Resample_test.dat')
        
        self.tb.connect(self.src,self.resample,self.snk)
        
        
        
        

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        self.tb.run ()
        # check data
        
        '''
        res = self.snk.data()        
        
        mod=scipy.io.loadmat('/home/demel/exchange/matlab_mat_f.mat') 
        mat_u1=tuple(mod['mat_f'].flatten())
        mat_d=range(len(mat_u1))
        for idx, val in enumerate(mat_u1):
            mat_d[idx]=val
        outtu=tuple(mat_d)
        #print len(outtu)/512
        print "len(res)   = " + str(len(res)) + "\nlen(outtu) = " + str(len(outtu))
        
        count=0
        for i in range (len(outtu)/512):
            try:
                self.assertComplexTuplesAlmostEqual(outtu[512*i:512*(i+1)],res[512*i:512*(i+1)],5)
                print str(i) + "\tres == outtu"
            except AssertionError:
                print str(i) + "\tres != outtu FAILED!!!"
                count = count+1
        #self.assertComplexTuplesAlmostEqual(outtu,res,5)
        
        #for i in range(512):
        #    pd = cmath.phase(res[i])-cmath.phase(outtu[i])
        #    print str(i) + "\t" + str(cmath.phase(res[i])) + "\t" + str(cmath.phase(outtu[i])) + "\t" + str(pd)
        #    print str(i) + "\t" + str(abs(res[i])) + "\t" + str(abs(outtu[i])) + "\t" + str(cmath.phase(res[i])) + "\t" + str(cmath.phase(outtu[i]))
        '''
        
        


if __name__ == '__main__':
    gr_unittest.main ()
