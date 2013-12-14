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

class qa_mib_unpack_vbm (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        # setup some dummy data before block instatiation
        N_rb_dl = 50
        phich_dur = 0
        phich_res = 1.0
        sfn =412
        mib = lte_test.pack_mib(N_rb_dl, phich_dur, phich_res, sfn)
        N_ant = range(1)
        N_ant[0] = 2
        self.src1 = blocks.vector_source_b(mib, False, 24)
        self.src2 = blocks.vector_source_b(N_ant,False,1)

        self.mib = lte.mib_unpack_vbm()
        
        self.tb.connect(self.src1,(self.mib,0))
        self.tb.connect(self.src2,(self.mib,1))
        
        # These few lines keep the qa from terminating without reason! --> DISABLED
        #self.msg = gr.message_debug()
        #self.tb.msg_connect(self.mib, "N_ant", self.msg, "print")
        #self.tb.msg_connect(self.mib, "N_rb_dl", self.msg, "print")
        #self.tb.msg_connect(self.mib, "phich_duration", self.msg, "print")
        #self.tb.msg_connect(self.mib, "phich_resources", self.msg, "print")
        #self.tb.msg_connect(self.mib, "SFN", self.msg, "print")

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        test_len = 1030
        N_ant = 2
        N_rb_dl = 50
        phich_dur = 0
        phich_res = 1.0
        input_data = []
        input_ant_data = []
        for sfn in range(test_len):
            input_data.extend(lte_test.pack_mib(N_rb_dl, phich_dur, phich_res, sfn))
            input_ant_data.append(N_ant)
        
        self.src1.set_data(input_data)
        self.src2.set_data(input_ant_data)
        # set up fg
        self.tb.run ()
        # check data
        #print self.mib.get_decoding_rate()


if __name__ == '__main__':
    gr_unittest.run(qa_mib_unpack_vbm, "qa_mib_unpack_vbm.xml")
    #gr_unittest.run("qa_mib_unpack_vbm.xml")