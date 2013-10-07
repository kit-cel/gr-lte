#!/usr/bin/env python
# -*- coding: utf-8 -*-
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

from gnuradio import gr, blocks, trellis


class bch_viterbi_vfvb(gr.hier_block2):
    """
    docstring for block bch_viterbi_vfvb
    """

    def __init__(self):
        gr.hier_block2.__init__(self,
                                "bch_viterbi_vfvb",
                                gr.io_signature(1, 1, gr.sizeof_float * 120), # Input signature
                                gr.io_signature(1, 1, gr.sizeof_char * 40)) # Output signature

        # Define blocks and connect them
        # Repeat input vector one time to get viterbi decoder state right (tail-biting stuff)
        self.rpt = blocks.repeat(gr.sizeof_float * 120, 2)
        # viterbi decoder requires stream as input
        self.vtos = blocks.vector_to_stream(1 * gr.sizeof_float, 120)


        # Correct FSM instantiation: k=num_input_bits, n=num_output_bits, Tuple[dim(k*n)] (decimal notation)
        self.fsm = trellis.fsm(1, 3, [91, 121, 117])

        # Values for viterbi decoder        
        K = 80  # steps for one coding block
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
        constellation = [1, 1, 1, 1, 1, -1, 1, -1, 1, 1, -1, -1, -1, 1, 1, -1, 1, -1, -1, -1, 1, -1, -1, -1]
        # print "len(constellation)/D = " + str(len(constellation)/D) + "\tfsm.O = " + str(self.fsm.O())
        # Viterbi_combined input: FSM, K, SO, SK, D, TABLE, TYPE
        # FSM    = Finite State Machine
        # K      = number of output symbols produced by the FSM
        # SO     = initial state of the FSM
        # SK     = final state of the FSM (unknown in this example)
        # D      = dimensionality
        # TABLE  = constellation of the input symbols
        self.vit = trellis.viterbi_combined_fb(self.fsm, K, SO, SK, D, constellation, 200)

        # connect all streams which are crated yet        
        self.connect(self,self.rpt,self.vtos,self.vit)
        
        # extract second half of each viterbi output block
        self.sts = blocks.stream_to_streams(1, 80)
        self.sts2 = blocks.streams_to_stream(1, 40)
        self.null = blocks.null_sink(1)
        self.connect(self.sts2, self.null)
       # dump the first half
        for i in range(40):
            self.connect( (self.sts, i), (self.sts2, i) )
        
        # vectorize the second half.
        self.sstv = blocks.streams_to_vector(1, 40)
        for i in range(40):
            self.connect( (self.sts, i+40), (self.sstv, i) )
            
        self.connect(self.vit, self.sts)
        self.connect(self.sstv, self)

