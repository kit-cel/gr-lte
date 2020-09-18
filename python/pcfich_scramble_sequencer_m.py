#!/usr/bin/env python3
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

import numpy
from gnuradio import gr
import pmt
from . import utils


class pcfich_scramble_sequencer_m(gr.sync_block):
    """
    Block receives message with cell_id and generates message with descrambling sequences for PCFICH
    """

    def __init__(self, name="pcfich_scramble_sequencer_m"):
        gr.sync_block.__init__(self,
                               name=name,
                               in_sig=None,
                               out_sig=None)

        self.msg_buf_in = pmt.intern("cell_id")
        self.msg_buf_out = pmt.intern("descr")
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
        print(("received cell_id = " + str(cell_id)))

        seqs = pmt.make_vector(10, pmt.make_vector(32, pmt.from_double(0.0)))
        for ns in range(10):
            scr = utils.get_pcfich_scrambling_sequence(cell_id, ns)
            scr = utils.encode_nrz(scr)
            pmt_seq = pmt.make_vector(len(scr), pmt.from_double(0.0))
            for i in range(len(scr)):
                pmt.vector_set(pmt_seq, i, pmt.from_double(scr[i]))
            pmt.vector_set(seqs, ns, pmt_seq)
        self.message_port_pub(self.msg_buf_out, seqs)


    #def work(self, input_items, output_items):
    #    in0 = input_items[0]
    #    out = output_items[0]
    #    # <+signal processing here+>
    #    out[:] = in0
    #    return len(output_items[0])

