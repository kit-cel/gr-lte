#!/usr/bin/env python
# 
# Copyright 2012 <+YOU OR YOUR COMPANY+>.
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
#

from gnuradio import gr, gr_unittest
import lte_swig

class qa_zadoff_chu_seq_generator_vc (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        
        mylen = 63
        self.zc = lte_swig.zadoff_chu_seq_generator_vc(mylen,29)
        self.head = gr.head(gr.sizeof_gr_complex*mylen,1)
        self.snk = gr.vector_sink_c(63)
        
        self.tb.connect(self.zc, self.head, self.snk )

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        self.tb.run ()
        # check data
        res = self.snk.data()
        
        for i in range(len(res)):
            print str(i) + "\t" + str(res[i])
       


if __name__ == '__main__':
    gr_unittest.main ()
