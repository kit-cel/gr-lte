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


class qa_sss_calculator_vcm(gr_unittest.TestCase):
    def setUp(self):
        self.tb = gr.top_block()

        self.fftl = 512
        self.key_id = "N_id_2"
        self.key_offset = "frame_offset"

        intu = [1.0] * 72
        self.src = blocks.vector_source_c(intu, False, 72)
        self.calc = lte.sss_calculator_vcm(self.fftl, self.key_id, self.key_offset)
        self.dbg = blocks.message_debug()

        self.tb.connect(self.src, self.calc)



    def tearDown(self):
        self.tb = None

    def test_001_t(self):
        # set up fg
        syms = 4
        cell_id = 124
        sss = lte_test.get_sss(cell_id)
        vec = [0] * 5
        vec.extend(sss[0])
        vec.extend([0] * 10)
        vec.extend(sss[1])
        vec.extend([0] * 5)
        data = []
        tags = []
        for n in range(syms):
            data.extend(vec)
            tags.append(lte_test.generate_tag(self.key_id, "src", cell_id%3, n))
            tags.append(lte_test.generate_tag(self.key_offset, "src", 4711+n, n))

        self.src.set_data(data, tags)
        self.tb.run()
        # check data
        calc_id = self.calc.get_cell_id()
        print calc_id
        assert(calc_id == cell_id) # check if id is calculated correctly
        print self.calc.get_frame_start()


if __name__ == '__main__':
    gr_unittest.run(qa_sss_calculator_vcm)
