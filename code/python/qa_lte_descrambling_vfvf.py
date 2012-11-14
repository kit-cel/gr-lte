#!/usr/bin/env python
# 
# Copyright 2012 Johannes Demel
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

class qa_descrambling_vfvf (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        
        print "qa_descrambling_vfvf START"
        
        cell_id = 124
        
        inf=open('/home/demel/exchange/matlab_pbch_demod_p.txt')
        intu=range(480)
    	for i in range(480):
    		intu[i]=float(inf.readline())
        
        self.src  = gr.vector_source_f( intu, True, 480)
       	self.ld   = lte_swig.descrambling_vfvf()
       	self.ld.set_cell_id(cell_id)
       	

        self.sink = gr.vector_sink_f(120)
        self.head = gr.head(480*4,10)

        self.tb.connect(self.src,self.head,self.ld,self.sink)
        

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        self.tb.run ()
        
        # check data
        res_all=self.sink.data()
        print len(res_all)
        res=res_all[0:1920]
        # Read in descrambled sequence from Matlab simulation.
        outf=open('/home/demel/exchange/matlab_pbch_descr_p.txt')
        outtu=range(1920)
    	for i in range(len(outtu)):
    		outtu[i]=float(outf.readline())
    		    	       
        print "assertTestFailure: " + str(self.assertFloatTuplesAlmostEqual(outtu,res,5))
        
        counter=0
        for i in range(len(res)):
        	if res[i]-outtu[i] > 1e-6:
        		# print str(i) + " unequal: " + str(res[i]-outtu[i])
        		counter=counter+1
        
        print "failed tests: " + str(counter)
        # print "self.sink.data() - LENGTH: " + str(len(self.sink.data()) )
                
        print "qa_descrambling_vfvf END"

if __name__ == '__main__':
    gr_unittest.main ()
