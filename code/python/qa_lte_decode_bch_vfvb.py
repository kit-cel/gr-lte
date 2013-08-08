#!/usr/bin/env python
# 
# Copyright 2013 Communications Engineering Lab (CEL) / Karlsruhe Institute of Technology (KIT)
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
import lte
import lte_test

class qa_decode_bch_vfvb (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
      
        self.src = gr.vector_source_f([0]*120, False, 120)
        self.bch = lte.decode_bch_vfvb()
        self.snk0 = gr.vector_sink_b(24)
        self.snk1 = gr.vector_sink_b(1)
        
        self.tb.connect(self.src, self.bch)
        self.tb.connect((self.bch, 0), self.snk0)
        self.tb.connect((self.bch, 1), self.snk1)
                

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        N_ant = 4    
        mib = lte_test.pack_mib(50,0,1.0, 511)
        bch = lte_test.encode_bch(mib, N_ant)
        bch = lte_test.nrz_encoding(bch)
        self.src.set_data(bch)
        # set up fg
        self.tb.run ()
        # check data
        res0 = self.snk0.data()
        res1 = self.snk1.data()
        print self.assertEqual(res0, tuple(mib))
        print self.assertEqual(res1, tuple([N_ant]))
        


if __name__ == '__main__':
    gr_unittest.main ()
