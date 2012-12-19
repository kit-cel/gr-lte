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

class qa_qpsk_soft_demod_vcvf (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        
        mod=scipy.io.loadmat('/home/demel/exchange/matlab_pbch_frame_demap.mat')
        mat_u=range(240)
        mat_u=tuple(mod['pbch_frame_demap'].flatten())
        mat_d=range(240)
        for i in mat_d:
            mat_d[i]=(mat_u[i])
        intu=tuple(mat_d)
        #print type(intu)
        #print type(intu[8])
        print intu[0:3]

        self.src = gr.vector_source_c(intu,False,240)
        
        self.dmd = lte_swig.qpsk_soft_demod_vcvf()
        
        self.snk = gr.vector_sink_f(480);
        
        self.tb.connect(self.src,self.dmd,self.snk)

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        self.tb.run ()
        # check data
        res = self.snk.data()
        outf=open('/home/demel/exchange/matlab_pbch_demod_p.txt')
        outtu=range(480)
    	for i in range(480):
    		outtu[i]=float(outf.readline())
    	outtu=tuple(outtu)
    		
    	# print len(outtu)
        print "outtu = " + str(outtu[0:10])
        print "res   = " + str(res[0:10])
        
        for idx, val in enumerate(res):
            if (val*outtu[idx]) < 0:
                print "failure!!"

if __name__ == '__main__':
    gr_unittest.main ()
