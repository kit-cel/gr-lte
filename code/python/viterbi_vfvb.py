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

from gnuradio import gr, trellis
import lte

class viterbi_vfvb(gr.hier_block2):
    def __init__(self, ):
#    """
#    docstring
#	"""
        gr.hier_block2.__init__(self, "viterbi_vfvb",
                gr.io_signature(1,1, gr.sizeof_float*120), # sizeof (<+float+>)),  # Input signature
				gr.io_signature(1,1, gr.sizeof_char*40 )) #sizeof (<+float+>))) # Output signature

        #print "\nlte_viterbi_vfvb START"
        
        #tpp=gr.tag_propagation_policy()
        #################################
        # Define blocks
        # Repeat input vector one time to get viterbi decoder state right (tail-biting stuff)
        self.rpt  = gr.repeat(gr.sizeof_float*120,2)
        # viterbi decoder requires stream as input
        self.vtos = gr.vector_to_stream(1*gr.sizeof_float,120)
        
	
        # Correct FSM instantiation: k=num_input_bits, n=num_output_bits, Tuple[dim(k*n)] (decimal notation)
        self.fsm  = trellis.fsm(1,3,[91,121,117])

        # Values for viterbi decoder        
        K = 40  # steps for one coding block
        SO = 0  # initial state
        SK = -1 # final state (in this case unknown, therefore -1)
        D = 3   # dimensionality
        # D = 3 follows from the fact that 1 {0,1} input symbol of the encoder produces 3 {0,1} output symbols.
        # (packed into one byte {0,1,...,7} )
        # with NRZ coding follows: 
        # 0 -->  1
        # 1 --> -1
        # This leads to the following constellation input
        #               |  0  |  1   | 2    | 3     |  4   |  5    |  6    |  7     |
        constellation = [1,1,1,1,1,-1,1,-1,1,1,-1,-1,-1,1,1,-1,1,-1,-1,-1,1,-1,-1,-1]
        # print "len(constellation)/D = " + str(len(constellation)/D) + "\tfsm.O = " + str(self.fsm.O())
	    
	    # Viterbi_combined input: FSM, K, SO, SK, D, TABLE, TYPE
	    # FSM    = Finite State Machine
	    # K      = number of output symbols produced by the FSM
	    # SO     = initial state of the FSM
	    # SK     = final state of the FSM (unknown in this example)
	    # D      = dimensionality
	    # TABLE  = constellation of the input symbols
        self.vit  = trellis.viterbi_combined_fb(self.fsm,K,SO,SK,D,constellation,200)	

        # stream output of viterbi decoder to vector
        self.stov2 = gr.stream_to_vector(1*gr.sizeof_char,80)
        # second half of the viterbi output carries desired information. (tail-biting stuff)
        my_map=range(40)
        for i in my_map:
            my_map[i]=i+40
        self.map = lte.vector_resize_vbvb(my_map,80,40)

        self.connect(self,self.rpt,self.vtos,self.vit,self.stov2,self.map,self)
        # print "lte_viterbi_vfvb END\n"
        
        
