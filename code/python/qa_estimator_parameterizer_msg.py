#!/usr/bin/env python
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
from gruel import pmt
import time
from estimator_parameterizer_msg import estimator_parameterizer_msg

class qa_estimator_parameterizer_msg (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        
        # These are block parameters        
        msg_buf_name_in = "cell_id"
        msg_buf_name_out = "est_params"
        N_rb_dl = 6
        ant_port = 0
        
        # These parameters are regarded as "data"
        cell_id = 124
        
        # Set up msg inputs and outputs
        msg = pmt.pmt_from_long(cell_id)
        self.strobe = blocks.message_strobe(msg, 1000)
        self.dbg = blocks.message_debug()
        
        # UUT
        self.param = estimator_parameterizer_msg(msg_buf_name_in, msg_buf_name_out, N_rb_dl, ant_port)
        
        # Set up connections
        self.tb.msg_connect(self.strobe, "strobe", self.param, msg_buf_name_in)
        self.tb.msg_connect(self.param, msg_buf_name_out, self.dbg, "print")

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        #self.tb.run ()
        self.tb.start()
        time.sleep(5)
        self.tb.stop()        
        # check data
        print "ended"


if __name__ == '__main__':
    gr_unittest.run(qa_estimator_parameterizer_msg, "qa_estimator_parameterizer_msg.xml")
