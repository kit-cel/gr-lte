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
from .rs_map_generator_m import rs_map_generator_m
import pmt
import time

class qa_rs_map_generator_m (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

        # These are block parameters
        msg_buf_name_in = "cell_id"
        msg_buf_name_out = "pilots"
        N_rb_dl = 6
        ant_port = 0

        # These parameters are regarded as "data"
        cell_id = 124

        # Set up msg inputs and outputs
        msg = pmt.from_long(cell_id)
        self.strobe = blocks.message_strobe(msg, 100)
        self.dbg = blocks.message_debug()

        # UUT
        self.param = rs_map_generator_m(msg_buf_name_in, msg_buf_name_out, N_rb_dl, ant_port)

        # Set up connections
        self.tb.msg_connect(self.strobe, "strobe", self.param, msg_buf_name_in)
        self.tb.msg_connect(self.param, msg_buf_name_out, self.dbg, "store")

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        #self.tb.run ()
        print("start")
        # Don't run this code on a usual ctest.
        # For some reason msg port only blocks will not terminate.
        #self.tb.start()
        #time.sleep(1)
        #print "5sec mark"
        ##self.tb.pause()
        #print "pause"
        #self.tb.stop()
        #print "stop"
        ##self.tb.wait()
        ## check data
        #print "ended"
        #self.tb = None
        return


if __name__ == '__main__':
    gr_unittest.run(qa_rs_map_generator_m)
