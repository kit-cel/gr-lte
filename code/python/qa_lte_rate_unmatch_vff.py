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

class qa_rate_unmatch_vff (gr_unittest.TestCase):

    def setUp (self):
        print "qa_lte_rate_unmatch_vff START"   
    	inf=open('/home/demel/exchange/matlab_pbch_descr.txt')
    	intu=range(120)
    	for i in range(120):
    		intu[i]=float(inf.readline())
    		#print line
    		#intu[i]=float(line)
    	invec=intu	# invec is a sample vector interleaved from Matlab
   	
        self.tb   = gr.top_block ()
        self.src  = gr.vector_source_f( invec, True, 40)
        self.vtos = gr.vector_to_stream(1*gr.sizeof_float,40)        
        self.stov = gr.stream_to_vector(1*gr.sizeof_float,1*120)
        self.head = gr.head(120*4,1)
        self.lru  = lte_swig.rate_unmatch_vff() # 124
        self.sink = gr.vector_sink_f(120)
        

        self.tb.connect(self.src, self.vtos, self.stov, self.head, self.lru, self.sink)

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        self.tb.run ()
        res=self.sink.data()

    
        # set up fg
        outtu=range(120)
        outf=open('/home/demel/exchange/matlab_d.txt')
    	for i in range(120):
    		outtu[i]=float(outf.readline())
 
        
        
        
#        for i in range(120):
#        	if self.assertFloatTuplesAlmostEqual(res,outtu,1):
#        		print "is equal"
#        	else:
#        		print "unequal! " + str(res[i]) + "\t" + str(outtu[i])
        #for i in range(len(res)):
        #	print '{0:.2f}'.format(res[i])
        print "rate_unmatch assertTEST: " + str(self.assertFloatTuplesAlmostEqual(res,outtu,5))
        #print res[5]
	# print self.sink.data()
	#print "Test if input is almost equal to output"
	#for i in range(120):
	#	if res[i]-outtu[i] > 1e-7:
	#		print str(i) + str(res[i]-outtu[i]) + "\tis greater than 0.000001"
#		else:
#			print str(i) + "\talmost equal"
	
	print "qa_lte_rate_unmatch_vff END"
        # check data


if __name__ == '__main__':
    gr_unittest.main ()
