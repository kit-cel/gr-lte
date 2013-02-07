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

class qa_rate_unmatch_vff (gr_unittest.TestCase):

    def setUp (self):
        self.tb   = gr.top_block ()
        self.src  = gr.vector_source_f( [0]*120 , False, 120)
        self.lru  = lte_swig.rate_unmatch_vff()
        self.snk = gr.vector_sink_f(120)
        self.tb.connect(self.src, self.lru, self.snk)

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        print "lte_rate_unmatch TEST"
        mib = lte_test.pack_mib(50,0,1.0, 511)
        mib_crc = lte_test.crc_checksum(mib, 2)
        c_enc_sorted = lte_test.convolutional_encoder_sorted(mib_crc)
        
        
        in_data = c_enc_sorted
        input_data = lte_test.rate_match(in_data)
        my_data = []
        for i in range(40):
            my_data.append(in_data[i+0])
            my_data.append(in_data[i+40])
            my_data.append(in_data[i+80])    
        in_data = my_data
        exp_res = tuple([float(in_data[i]) for i in range(len(in_data))])
        
        self.src.set_data(input_data)
        self.tb.run ()
        res=self.snk.data()
        
        self.assertEqual(res, exp_res)




if __name__ == '__main__':
    gr_unittest.main ()
