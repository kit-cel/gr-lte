#!/usr/bin/env python
# 
# Copyright 2012 Communications Engineering Lab (CEL) / Karlsruhe Institute of Technology (KIT)
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

class qa_vector_resize_vbvb (gr_unittest.TestCase):

    def setUp (self):
        self.tb   = gr.top_block ()
        
        my_map = range(46)
        for i in range(40):
            my_map[i+6] = int(i)
            
        my_map[0]=int(34)
        my_map[1]=int(35)
        my_map[2]=int(36)
        my_map[3]=int(37)
        my_map[4]=int(38)
        my_map[5]=int(39)
        
        my_data = range(40)
        for i in my_data:
            my_data[i]=int(i)
        
        self.src  = gr.vector_source_b(my_data,False,40)
        self.head = gr.head(40,1)
        
        self.vrs  = lte_swig.vector_resize_vbvb(my_map,40,46)
        
        self.snk  = gr.vector_sink_b(46)
        
        self.tb.connect(self.src,self.head,self.vrs,self.snk)

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        self.tb.run ()
        # check data
        #print self.snk.data()


if __name__ == '__main__':
    gr_unittest.main ()
