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
from encode_pcfich import *
from encode_phich import *
from lte_core import *

dci_format_lut = {0:"Format 0", 1:"Format 1", 2:"Format 1A",
                  3:"Format 1B", 4:"Format 1C", 5:"Format 1D",
                  6:"Format 2", 7:"Format 2A", 8:"Format 2B",
                  9:"Format 2C", 10:"Format 3", 11:"Format 3A",
                  12:"Format 4"}
def encode_dci(dci_format):
    print "hello world"
    for i in range(len(dci_format_lut)):
        print dci_format_lut[i]

def encode_dci_format0():
    length = 0
    print "Carrier indicator" # - 0 or 3 bits
    length = length + 3
    print "Flag Format 0/1A diff - 1bit"
    length = length + 1
    print "Flag: Frequency Hopping - 1bit"
    N_ul_rb = 50
    rbaha = int(math.ceil(math.log( N_ul_rb * (N_ul_rb+1)/2 ,2) ) )
    print "RB assignment/hopping alloc " + str(rbaha) + "bits"
    
def encode_format1():
    print "format1"
    
def get_pdcch_length_bits(dc_form):
    N_CCE = 2**dc_form
    N_REG = N_CCE * 9
    N_bits = N_REG * 8
    #print str(dc_form) + "\t" + str(N_CCE) + "\t" + str(N_REG) + "\t" + str(N_bits)
    return N_bits

def get_pdcch_cinit(ns, cell_id):
    return int(math.floor(ns/2) * 2**9 + cell_id)
    
def get_n_cce_available(N_ant, N_rb_dl, cfi, N_g):
    M_pcfich_bits = get_pcfich_length_bits()
    M_phich_bits = get_phich_length_bits(N_g, N_rb_dl)    
    M_used = int(M_pcfich_bits/2) + M_phich_bits
    n_syms = cfi
    if N_rb_dl < 10:
        n_syms = n_syms + 1
    total_re = n_syms * 12 * N_rb_dl
    rs_re = 0
    if N_ant < 4:
        rs_re = 4 * N_rb_dl
    elif N_ant == 4:
        rs_re = 4 * N_rb_dl
        if n_syms > 1:
            rs_re = rs_re + 4 * N_rb_dl
    n_avail = total_re - (rs_re + M_used)
    n_reg = n_avail / 4
    n_cce = int(math.floor(n_reg/9) )
    return n_cce
    
   
def get_num_candidates_for_pdcch(L, pdcch_type):
    M = 0
    if pdcch_type == "Common":
        if L == 4:
            M = 4
        elif L == 8:
            M = 2
        else:
            print "invalid value L = " + str(L) + " for type " + pdcch_type
    elif pdcch_type == "UE-specific":
        if L == 1:
            M = 6
        elif L == 2:
            M = 6
        elif L == 4:
            M = 2
        elif L == 8:
            M = 2
        else:
            print "invalid value L = " + str(L) + " for type " + pdcch_type
    else:
        print "invalid type = " + pdcch_type
    return M
    
def get_pdcch_search_space(L, Y_k, N_CCE, pdcch_type):
    M_cand = get_num_candidates_for_pdcch(L, pdcch_type)
    cands = []
    for m in range(M_cand):
        cces = []
        for i in range(L):
            cce = L * ( (Y_k + m)%(int(math.floor(N_CCE/L))) ) + i
            cces.append(cce)
        cands.append(cces)
    return cands
    
def get_si_rnti_value():
    return 0xFFFF
    
def get_p_rnti_value():
    return 0xFFFE
    
def get_m_rnti_value():
    return 0xFFFD
    
def get_rba_len(N_rb_dl):
    n_bits = int(math.ceil( math.log(N_rb_dl * (N_rb_dl+1) /2 ,2) ) )
    print "RBA length = " + str(n_bits)    
    return n_bits
    
def encode_dci_format1a(N_rb_dl, rap):
    length = 0
    print "carrier indicator"
    length = length + 3
    print "flag: format 0/1a diff"
    length = length + 1
    if rap == 1:
        print "flag: loc/dist VRB set 0"
        length = length + 1
        print "RB assignment"
        length = length + get_rba_len(N_rb_dl)
        print "preamble index"
        length = length + 6
        print "PRACH Mask Index"
        length = length + 4
        print "all other bits are set to zero... which bits???"
    elif rap == 0:
        print "flag: loc/dist VRB"
        length = length + 1
        print "RB assignment"
        length = length + get_rba_len(N_rb_dl)
        print "localized = all RBA bits used"
        print "distributed = first bit for GAP value if(N_rb_dl >= 50)"
        print "modulation and coding scheme"
        length = length + 5
        print "HARQ process number"
        length = length + 3 #for FDD, for TDD +4
        print "new data indicator"
        length = length + 1
        print "redundancy version"
        length = length + 2
        print "TPC command for PUCCH"
        length = length + 2
    return length
    
def get_pdcch(pdcch_format): #return a PDCCH of a specified format
    n_bits = get_pdcch_length_bits(pdcch_format)
    return [1] * n_bits
    
def get_all_pdcchs(N_CCE):
    res = []
    for i in range(N_CCE):
        res.append(get_pdcch(0))
    return res
    
def get_multiplexed_pdcch(pdcchs):
    res = []
    for i in range(len(pdcchs)):
        res.extend(pdcchs[i])
    return res

def scramble_pdcch(pdcch, ns, cell_id):
    cinit = get_pdcch_cinit(ns, cell_id)
    scr = pn_generator(len(pdcch), cinit)
    return [(pdcch[i]+scr[i])%2 for i in range(len(pdcch))]

lut_pdcch_types = {0:"Common", 1:"UE-specific"}
if __name__ == "__main__":
    cell_id = 124
    N_ant = 2
    style= "tx_diversity"
    N_rb_dl = 50
    sfn = 0
    ns = 0
    Ncp = 1 # (0,1)
    N_g = 1
    cfi = 2
    L = 4 # Aggregation Level (1,2,4,8)
    Y_k = 0 # set to 0 for common search space
    pdcch_type = lut_pdcch_types[0]
    
    
    
    #rap = 0
    #length = encode_dci_format1a(N_rb_dl, rap)
    #print "len(format1a) = " + str(length)
    #print "len(with crc) = " + str(length + 16)
    
        
    print "PDCCH - Encoding"
    print "1. DCI Multiplexing"
    N_CCE = get_n_cce_available(N_ant, N_rb_dl, cfi, N_g)
    print "available CCEs = " + str(N_CCE)
    #print get_pdcch_search_space(L, Y_k, N_CCE, pdcch_type)

    pdcchs = get_all_pdcchs(N_CCE)
    tot_pdcch = get_multiplexed_pdcch(pdcchs)

    print "\n2. Scrambling"
    scr_pdcch = scramble_pdcch(tot_pdcch, ns, cell_id)
    
    print "\n3. Modulation - QPSK"
    mod_pdcch = qpsk_modulation(scr_pdcch)   
    
    print "\n4. Layer Mapping and precoding"
    lay_pdcch = layer_mapping(mod_pdcch, N_ant, style)
    pre_pdcch = pre_coding(lay_pdcch, N_ant, style)

    
    test = 1
    print test == None
    
        
        
        