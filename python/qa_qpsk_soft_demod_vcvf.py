#!/usr/bin/env python
# -*- coding: utf-8 -*-
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

from gnuradio import gr, gr_unittest, blocks
import lte_swig as lte
import lte_test

class qa_qpsk_soft_demod_vcvf (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

        vlen = 240
        self.src = blocks.vector_source_c([0]*vlen,False, vlen)
        self.demod = lte.qpsk_soft_demod_vcvf(vlen)
        self.snk = blocks.vector_sink_f(2*vlen)

        self.tb.connect(self.src, self.demod, self.snk)

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        cell_id = 124
        N_ant = 2
        tl = 1024
        data = []
        input_data = []
        for i in range(tl/4):
            mib = lte_test.pack_mib(50,0,1.0, i*4)
            bch = tuple(lte_test.encode_bch(mib, N_ant))
            p_scrambled = lte_test.pbch_scrambling(bch, cell_id)
            input_data.extend(lte_test.nrz_encoding(p_scrambled))
            mod = lte_test.qpsk_modulation(p_scrambled)
            data.extend(mod)

        self.src.set_data(data)

        # set up fg
        self.tb.run ()
        # check data
        res = self.snk.data()
        exp_res = tuple(input_data)#tuple([input_data[i]/math.sqrt(2) for i in range(len(input_data))])
        self.assertFloatTuplesAlmostEqual(res, exp_res, 5)


if __name__ == '__main__':
    gr_unittest.run(qa_qpsk_soft_demod_vcvf)
