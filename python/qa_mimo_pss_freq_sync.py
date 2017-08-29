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
from gnuradio import gr, gr_unittest
from gnuradio import blocks, filter
import pmt
import lte_swig as lte
import lte_test.lte_phy as t
import numpy as np


class qa_mimo_pss_freq_sync(gr_unittest.TestCase):
    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_000_controller(self):
        samples = t.get_mod_frame(124, 6, 1, 128)
        src = blocks.vector_source_c(samples[0], repeat=False, vlen=1)
        ctrl = lte.mimo_pss_coarse_control(1)
        not_ctrl = lte.mimo_pss_coarse_control(1)
        snk = blocks.vector_sink_c(vlen=1)
        not_snk = blocks.vector_sink_c(vlen=1)

        not_ctrl._post(pmt.to_pmt('control'), pmt.PMT_T)
        self.tb.connect(src, ctrl, snk)
        self.tb.connect(src, not_ctrl, not_snk)
        self.tb.run()

        res = snk.data()
        self.assertComplexTuplesAlmostEqual(samples[0], res, 5)
        not_res = not_snk.data()
        self.assertEqual(not_res, ())

    def test_001_t(self):
        fftlen = 128
        cell_id = 124
        taps = filter.optfir.low_pass(1, fftlen*15e3, 472.5e3, 900e3, 0.2, 40)
        samples = t.get_mod_frame(cell_id, 6, 1, fftlen)
        samps = np.tile(samples[0], 5)

        src = blocks.vector_source_c(samps, repeat=False, vlen=1)
        fir = filter.fir_filter_ccc(fftlen/64, taps)
        sync = lte.mimo_pss_coarse_sync(4, 1)
        dbg0 = blocks.message_debug()
        dbg1 = blocks.message_debug()
        dbg2 = blocks.message_debug()

        self.tb.connect(src, fir, sync)
        self.tb.msg_connect((sync, 'control'), (dbg0, 'store'))
        self.tb.msg_connect((sync, 'N_id_2'), (dbg1, 'store'))
        self.tb.msg_connect((sync, 'coarse_pos'), (dbg2, 'store'))
        # set up fg
        self.tb.run()
        # check data
        self.assertEqual(pmt.to_long(dbg1.get_message(0)), int(cell_id % 3))

        ctrl_res = pmt.to_bool(dbg0.get_message(0))
        assert ctrl_res

        offset = 10 + 5 * 9 + 6 * fftlen
        print(offset)

        # print()
        pos = pmt.to_long(dbg2.get_message(0))
        print(pos)
        print(pos * 2 - (len(taps) // 2))
        #import matplotlib.pyplot as plt
        #plt.plot(np.abs(samps))
        #plt.show()


if __name__ == '__main__':
    gr_unittest.run(qa_mimo_pss_freq_sync)
