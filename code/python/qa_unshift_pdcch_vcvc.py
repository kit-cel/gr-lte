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

from gnuradio import gr, gr_unittest
import lte

class qa_unshift_pdcch_vcvc (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        
        N_rb_dl = 50
        N_ant = 2
        cfi = 2
        N_g = 1.0
        len_tag_key = "packet_len"
        self.unshift = lte.unshift_pdcch_vcvc(N_rb_dl, N_ant, cfi, N_g, len_tag_key)

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        self.tb.run ()
        # check data


if __name__ == '__main__':
    gr_unittest.run(qa_unshift_pdcch_vcvc, "qa_unshift_pdcch_vcvc.xml")
