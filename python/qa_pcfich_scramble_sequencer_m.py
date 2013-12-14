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
from pcfich_scramble_sequencer_m import pcfich_scramble_sequencer_m
import pmt


class qa_pcfich_scramble_sequencer_m(gr_unittest.TestCase):
    def setUp(self):
        self.tb = gr.top_block()

        self.param = pcfich_scramble_sequencer_m()

    def tearDown(self):
        self.tb = None

    def test_001_t(self):
        cell_id = 47
        self.param.handle_msg(pmt.from_long(cell_id))
        # set up fg
        self.tb.run()
        # check data


if __name__ == '__main__':
    gr_unittest.run(qa_pcfich_scramble_sequencer_m, "qa_pcfich_scramble_sequencer_m.xml")
