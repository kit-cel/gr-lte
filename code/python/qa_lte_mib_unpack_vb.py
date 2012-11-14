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

class qa_mib_unpack_vb (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        
        # Read in binary Matlab data from viterbi decoder output.
        vit=scipy.io.loadmat('/home/demel/exchange/matlab_vit.mat')
        mat_u=range(40)
        mat_u=tuple(vit['vit'].flatten())
        mat_vit=range(40)
        for i in mat_vit:
            mat_vit[i]=int(mat_u[i])
        mat_vit=tuple(mat_vit)
        self.in_data=mat_vit
        intu=mat_vit[0:24]
        print intu
        self.src1 = gr.vector_source_b(intu, False, 24)
        N_ant = range(1)
        N_ant[0] = 2
        self.src2 = gr.vector_source_b(N_ant,False,1)

        self.mib = lte_swig.mib_unpack_vb()
        
        #self.snk = gr.null_sink(1)
        
        self.tb.connect(self.src1,(self.mib,0))
        self.tb.connect(self.src2,(self.mib,1))
        #self.tb.connect(self.mib,self.snk)
        
    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        self.tb.run ()
        # check data
        print "SFN       = " + str(self.mib.get_SFN())
        print "cell_id   = " + str(self.mib.get_cell_id())
        print "N_ant     = " + str(self.mib.get_N_ant())
        print "N_rb_dl   = " + str(self.mib.get_N_rb_dl())
        print "phich_dur = " + str(self.mib.get_phich_dur())
        print "phich_res = " + str(self.mib.get_phich_res())

if __name__ == '__main__':
    gr_unittest.main ()
