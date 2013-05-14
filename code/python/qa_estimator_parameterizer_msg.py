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
from estimator_parameterizer_msg import estimator_parameterizer_msg

class qa_estimator_parameterizer_msg (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        
        msg_buf_name = "cell_id"
        N_rb_dl = 6
        
        cell_id = 124
        data = (cell_id,)
        self.src = blocks.vector_source_i(data, False, 1)
        #self.msg_sink = blocks.message_sink(gr.sizeof_int, True)
        #self.tb.connect(self.src, self.msg_sink, )
        msg = gr.message(cell_id)
        self.msgqin = gr.msg_queue()
        self.msgqin.insert_tail(msg)
        
        
        self.param = estimator_parameterizer_msg(msg_buf_name, N_rb_dl)
        
        self.tb.msg_connect(self.msgqin, self.param, "cell_id")

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        self.tb.run ()
        # check data


if __name__ == '__main__':
    gr_unittest.run(qa_estimator_parameterizer_msg, "qa_estimator_parameterizer_msg.xml")
