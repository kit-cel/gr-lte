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


class qa_extract_subcarriers_vcvc(gr_unittest.TestCase):
    def setUp(self):
        self.tb = gr.top_block()

        self.N_rb_dl = N_rb_dl = 6
        self.fftl = fftl = 512

        intu1 = range(fftl)
        self.src = blocks.vector_source_c(intu1, False, fftl)
        self.ext = lte.extract_subcarriers_vcvc(N_rb_dl, fftl)
        self.snk = blocks.vector_sink_c(12 * N_rb_dl)

        self.tb.connect(self.src, self.ext, self.snk)

    def tearDown(self):
        self.tb = None

    def test_001_t(self):
        # set up fg
        frame_len = 140
        data = [1.0] * self.N_rb_dl * 12 * frame_len
        in_data = []
        n_zeros = self.fftl - (self.N_rb_dl * 12) - 1

        for i in range(frame_len):
            sym = i * 12 * self.N_rb_dl
            vec = [0, ]
            vec.extend(data[sym:sym + (6 * self.N_rb_dl)])
            vec.extend([0] * n_zeros)
            vec.extend(data[sym + 6 * self.N_rb_dl:sym + 12 * self.N_rb_dl])
            in_data.extend(vec)

        self.src.set_data(in_data)

        # run fg
        self.tb.run()
        # check data
        res = self.snk.data()
        self.assertFloatTuplesAlmostEqual(res, data)


if __name__ == '__main__':
    gr_unittest.run(qa_extract_subcarriers_vcvc, "qa_extract_subcarriers_vcvc.xml")
