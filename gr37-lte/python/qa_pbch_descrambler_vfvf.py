#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2013 <+YOU OR YOUR COMPANY+>.
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
#import lte_swig as lte
import lte
import lte_test

class qa_pbch_descrambler_vfvf (gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()


        key = "descr_part"
        self.src = blocks.vector_source_f([0.0]*480, False, 480)
        self.descr = lte.pbch_descrambler_vfvf(key)
        self.snk = blocks.vector_sink_f(120)

        self.tb.connect(self.src, self.descr, self.snk)

    def tearDown(self):
        self.tb = None

    def test_001_t(self):
        cell_id = 124
        N_ant = 2
        mib = lte_test.pack_mib(50,0,1.0, 511)
        bch = tuple(lte_test.encode_bch(mib, N_ant))
        p_scrambled = lte_test.pbch_scrambling(bch, cell_id)
        p_scrambled = lte_test.nrz_encoding(p_scrambled)
        n_bch = tuple(lte_test.nrz_encoding(bch))


        self.src.set_data(p_scrambled)#[0:480])
        self.descr.set_cell_id(cell_id)
        dbgs = blocks.file_sink(480 * gr.sizeof_float, "/home/johannes/tests/descramble.dat")
        self.tb.connect(self.src, dbgs)
        # set up fg
        self.tb.run ()
        res = self.snk.data()

        count = 0
        for i in range(len(res)/len(n_bch)):
            part = res[len(n_bch)*i:(i+1)*len(n_bch)]
            try:
                self.assertEqual(part, n_bch,3)
                print str(i) + "\tSUCCESS"
            except:
                print str(i)
                count = count +1
        print "\nresult"
        print count
        print len(res)/len(n_bch)

    def test_002_data(self):
        pass


        #dbgs = blocks.file_sink(480 * gr.sizeof_float, "/home/johannes/tests/descramble.dat")
        #self.tb.connect(self.src, dbgs)
        ## set up fg
        #self.tb.run ()


if __name__ == '__main__':
    gr_unittest.run(qa_pbch_descrambler_vfvf)
