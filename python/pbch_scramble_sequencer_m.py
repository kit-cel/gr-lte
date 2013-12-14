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

import numpy
from gnuradio import gr
import pmt
import lte

class pbch_scramble_sequencer_m(gr.sync_block):
    """
    Take cell_id, generate scrambling sequence and out it out
    """
    def __init__(self):
        gr.sync_block.__init__(self,
            name="pbch_scramble_sequencer_m",
            in_sig=None,
            out_sig=None)

        self.msg_buf_in = pmt.intern("cell_id")
        self.msg_buf_out = pmt.intern("vector")
        self.cell_id = -1
        
        self.message_port_register_in(self.msg_buf_in)
        self.set_msg_handler(self.msg_buf_in, self.handle_msg)
        self.message_port_register_out(self.msg_buf_out)

    def handle_msg(self, msg):
        cell_id = pmt.to_long(msg)
        if self.cell_id == cell_id:
            return
        else:
            self.cell_id = cell_id
        print "received cell_id = " + str(cell_id)

        seqs = []

        seq = lte.encode_nrz(lte.generate_pn_sequence(1920, cell_id))

        pmt_list = pmt.list1(pmt.from_double(seq[0]))
        for i in range(len(seq) - 1):
            pmt_list = pmt.list_add(pmt_list, pmt.from_double(seq[i+1]))
        self.message_port_pub(self.msg_buf_out, pmt_list)



    def work(self, input_items, output_items):
        in0 = input_items[0]
        out = output_items[0]
        # <+signal processing here+>
        out[:] = in0
        return len(output_items[0])

