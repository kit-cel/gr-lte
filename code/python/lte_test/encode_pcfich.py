# -*- coding: utf-8 -*-
"""
Created on Wed Jul 24 15:57:03 2013

@author: johannes
"""
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
    cinit = int( (math.floor(ns/2)+1) * (2*cell_id+1) * (2**9) + cell_id )
    return scramble_sequence(cfi_seq, cinit)

def encode_pcfich(cfi, cell_id, ns, N_ant):
    style = "tx_diversity"
    cfi_seq = get_cfi_sequence(cfi)
    scr_cfi_seq = scramble_cfi_sequence(cfi_seq, cell_id, ns)
    mod_cfi_seq = qpsk_modulation(scr_cfi_seq)
    lay_cfi_seq = layer_mapping(mod_cfi_seq, N_ant, style)
    return pre_coding(lay_cfi_seq, N_ant, style)
    
if __name__ == "__main__":
    cfi = 2    
    N_rb_dl = 50
    cell_id = 124
    ns = 4
    N_ant = 2
    style = "tx_diversity"
    
    print math.pow(2,9)
    print 2**9
    

    for i in range(5):
        cfi_seq = get_cfi_sequence(i)
        #print i
        #print len(cfi_seq)
        print cfi_seq
  
    cfi_seq = get_cfi_sequence(cfi)
    for i in range(10):
        ns = 2*i # For tests, CFI at beginning of subframe
        scr_cfi_seq = scramble_cfi_sequence(cfi_seq, cell_id, ns)
        print scr_cfi_seq
        #print qpsk_modulation(scr_cfi_seq)
    scr_cfi_seq = scramble_cfi_sequence(cfi_seq, cell_id, ns)
    mod_cfi_seq = qpsk_modulation(scr_cfi_seq)
    lay_cfi_seq = layer_mapping(mod_cfi_seq, N_ant, style)
    prc_cfi_seq = pre_coding(lay_cfi_seq, N_ant, style)
    pcfich = encode_pcfich(cfi, cell_id, ns, N_ant)
    print pcfich
    print np.shape(pcfich)
    print len(lay_cfi_seq)
    print np.shape(lay_cfi_seq)
    print np.shape(prc_cfi_seq)
    
    
    