#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2015 Communications Engineering Lab (CEL) / Karlsruhe Institute of Technology (KIT)
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


from gnuradio import gr, gr_unittest
from gnuradio import blocks
import lte_swig as lte
from . import lte_test.lte_phy as phy
from . import lte_test as t
import pmt
import numpy as np


class qa_mimo_remove_cp(gr_unittest.TestCase):
    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_t(self):
        cell_id = 124
        fftlen = 128
        N_rb_dl = 6
        N_ant = 1
        outkey = 'symbol'
        initial_offset = 1459
        frame = phy.generate_phy_frame(cell_id, N_rb_dl, N_ant)
        time_frame = phy.transform_to_time_domain(frame, fftlen)
        samples = phy.transform_to_stream_cp(time_frame, fftlen)
        samps = samples[0]
        samps = np.append(np.zeros(initial_offset, dtype=complex), samps)
        tag_list = self.get_tag_list(fftlen, initial_offset, 20, 10, 'slot', 'pss_tagger')

        src = blocks.vector_source_c(samps, tags=tag_list, repeat=False)
        ext = lte.mimo_remove_cp(fftlen, N_ant, outkey)
        snk = blocks.vector_sink_c(fftlen)

        self.tb.connect(src, ext, snk)
        self.tb.run()

        # check data
        n_syms = 139
        res = snk.data()[0:fftlen * n_syms]
        ref = time_frame.flatten()[0:fftlen * n_syms]
        self.assertComplexTuplesAlmostEqual(res, ref, 5)

    def get_tag_list(self, fft_len, initial_offset, num_tags, value_range, inkey, srcid):
        slotl = t.get_slot_length(fft_len)
        tl = t.get_tag_list(num_tags, value_range, inkey, srcid)
        for s in tl:
            s.offset *= slotl
            s.offset += initial_offset
        return tl


if __name__ == '__main__':
    gr_unittest.run(qa_mimo_remove_cp)
