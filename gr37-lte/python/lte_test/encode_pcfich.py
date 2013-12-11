#!/usr/bin/env python
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


import math
from lte_core import *
import numpy as np

cfi_codewords = { 1:[0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1], 2:[1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0], 3:[1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1],4:[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]}
def get_cfi_sequence(cfi):
    # check validity
    # According to ETSI 136211 the CFI sequence is represented by b(0),...,b(31)
    if cfi < 1 or cfi > 4:
        print "hey stupid! " + str(cfi) + " is an invalid value!"
        return cfi_codewords[4]
    return cfi_codewords[cfi]
        
def scramble_cfi_sequence(cfi_seq, cell_id, ns):
    # Initialize cinit with by the formula given in ETSI 136211
    # scrambled sequence is represented by b \tilda
    cinit = get_pcfich_cinit(ns, cell_id)
#    print "cinit python = " + str(cinit)
    return scramble_sequence(cfi_seq, cinit)
    
def get_pcfich_cinit(ns, cell_id):
    return int( (math.floor(ns/2)+1) * (2*cell_id+1) * (2**9) + cell_id )
    
def get_pcfich_scrambling_sequence(cell_id, ns):
    cinit = get_pcfich_cinit(ns, cell_id)
    return pn_generator(32, cinit)
    
def get_pcfich_length_bits():
    return 32

def encode_pcfich(cfi, cell_id, ns, N_ant):
    style = "tx_diversity"
    cfi_seq = get_cfi_sequence(cfi)
    scr_cfi_seq = scramble_cfi_sequence(cfi_seq, cell_id, ns)
    mod_cfi_seq = qpsk_modulation(scr_cfi_seq)
    lay_cfi_seq = layer_mapping(mod_cfi_seq, N_ant, style)
    pre_cfi_seq = pre_coding(lay_cfi_seq, N_ant, style)
    if N_ant == 1:
        pre_cfi_seq = [pre_cfi_seq]
    return pre_cfi_seq
    
if __name__ == "__main__":
    cfi = 2    
    N_rb_dl = 50
    cell_id = 124
    ns = 4
    N_ant = 2
    style = "tx_diversity"
    

    for i in range(5):
        cfi_seq = get_cfi_sequence(i)

    cfi_seq = get_cfi_sequence(cfi)
    for i in range(10):
        ns = 2*i # For tests, CFI at beginning of subframe
        scr_cfi_seq = scramble_cfi_sequence(cfi_seq, cell_id, ns)
        #print qpsk_modulation(scr_cfi_seq)
    scr_cfi_seq = scramble_cfi_sequence(cfi_seq, cell_id, ns)
    mod_cfi_seq = qpsk_modulation(scr_cfi_seq)
    lay_cfi_seq = layer_mapping(mod_cfi_seq, N_ant, style)
    prc_cfi_seq = pre_coding(lay_cfi_seq, N_ant, style)
    pcfich = encode_pcfich(cfi, cell_id, ns, N_ant)
    
    cfi1 = nrz_encoding(get_cfi_sequence(1))
    cfi2 = nrz_encoding(get_cfi_sequence(2))
    cfi3 = nrz_encoding(get_cfi_sequence(3))
    
    corr = 0
    for i in range(len(cfi1)):
        corr = corr + cfi2[i]*cfi2[i]
        
    print corr
    
    
    