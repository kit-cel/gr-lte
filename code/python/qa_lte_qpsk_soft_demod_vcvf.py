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
import lte_test

class qa_qpsk_soft_demod_vcvf (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        
        vlen = 240
        
        self.src = gr.vector_source_c([0]*vlen,False, vlen)
        self.demod = lte_swig.qpsk_soft_demod_vcvf(vlen)
        self.snk = gr.vector_sink_f(2*vlen)
        
        self.tb.connect(self.src, self.demod, self.snk)

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        print "demodulation test"
        mib = lte_test.pack_mib(50,0,1.0, 511)   
        bch = lte_test.encode_bch(mib, 2)
        p_scrambled = lte_test.pbch_scrambling(bch, 124)
        input_data = p_scrambled#[0:480]
        qpsk_mod = lte_test.qpsk_modulation(input_data)
        input_data = lte_test.nrz_encoding(input_data)
        
        self.src.set_data(qpsk_mod)
        # set up fg
        self.tb.run ()
        # check data
        res = self.snk.data()
        exp_res = tuple(input_data)#tuple([input_data[i]/math.sqrt(2) for i in range(len(input_data))])
        print self.assertFloatTuplesAlmostEqual(res, exp_res, 5)



if __name__ == '__main__':
    gr_unittest.main ()
