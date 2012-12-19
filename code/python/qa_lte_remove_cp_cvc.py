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
import time

class qa_remove_cp_cvc (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        
        offset = 43223
        fftl = 512
        
        mytime=time.clock()
        mod=scipy.io.loadmat('/home/demel/exchange/matlab_test_freq_estimate.mat') 
        mat_u1=tuple(mod['test'].flatten())
        mat_d=range(len(mat_u1))
        for idx, val in enumerate(mat_u1):
            mat_d[idx]=val
        intu=tuple(mat_d)
        
        print "read in data time = " + str(time.clock() - mytime)

        '''
        mod=scipy.io.loadmat('/home/demel/exchange/matlab_frame_samples.mat') 
        mat_u1=tuple(mod['frame_samples'].flatten())
        mat_d=range(len(mat_u1))
        for idx, val in enumerate(mat_u1):
            mat_d[idx]=val
        intu=tuple(mat_d)
        '''
        
        print len(intu)
        
        self.src  = gr.vector_source_c(intu,False,1)
        self.tag = lte_swig.tag_symbol_cc(offset,fftl)
        #self.head = gr.head(gr.sizeof_gr_complex,len(intu)*2)
        self.rcp  = lte_swig.remove_cp_cvc(fftl)
        self.snk  = gr.vector_sink_c(fftl)
        
        self.tb.connect(self.src,self.tag,self.rcp,self.snk)
        
        print "setup flowgraph time = " + str(time.clock() - mytime)

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        
        myruntime = time.clock()
        self.tb.run ()
        print "flowgraph runtime = " + str(time.clock() - myruntime)
        # check data
        fftl=512
        res = self.snk.data()
        print len(res)
        
        mod=scipy.io.loadmat('/home/demel/exchange/matlab_mat_t.mat') 
        mat_u1=tuple(mod['mat_t'].flatten())
        mat_d=range(len(mat_u1))
        for idx, val in enumerate(mat_u1):
            mat_d[idx]=val
        outtu=tuple(mat_d)
        print len(outtu)
        
        count = 0
        succ = 0
        for i in range(len(res)/fftl):
            try:
                self.assertComplexTuplesAlmostEqual(res[fftl*i:fftl*(i+1)],outtu[fftl*i:fftl*(i+1)],5)
                succ = succ+1
                #print str(i) + "\tres == outtu"
            except AssertionError:
                #print str(i) + "\tres != outtu FAILED!!!"
                count = count +1
        print "failed vectors     = " + str(count)
        print "successful vectors = " + str(succ)
        
        #for i in range(20):
        #    print str(i) + "\t" + str(res[i]) + "\t" + str(outtu[i])


if __name__ == '__main__':
    gr_unittest.main ()
