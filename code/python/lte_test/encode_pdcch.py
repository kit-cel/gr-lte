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

def encode_format0():
    print "Carrier indicator - 0 or 3 bits"
    print "Flag Format 0/1A diff - 1bit"
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
    
def get_n_cce_available(N_ant, N_rb_dl, cfi, M_pcfich_bits, M_phich_bits):
    M_used = int(M_pcfich_bits/2) + M_phich_bits
    M_used = M_used/3
    n_syms = cfi
    if N_rb_dl < 10:
        n_syms = n_syms + 1
    total_re = n_syms * 12 * N_rb_dl
    
    # continue HERE! 
    n_sym0 = 2 * N_rb_dl
    n_sym1 = 2 * N_rb_dl
    if N_ant < 4:
        n_sym1 = 3 * N_rb_dl
    n_sym2 = 3 * N_rb_dl
    if N_rb_dl < 10:
        cfi = cfi + 1
    n_total = 0
    if cfi == 1:
        n_total = n_sym0 - M_used
    elif cfi == 2:
        n_total = n_sym0 + n_sym1 - M_used
    elif cfi == 3:
        n_total = n_sym0 + n_sym1 + n_sym2 - M_used
    elif cfi == 4:
        n_total = n_sym0 + n_sym1 + 2*n_sym2 - M_used
    else:
        print "invalid arguments!"
        print cfi
    print n_total
    n_cce = int(math.floor(n_total/9) )
    return n_cce


if __name__ == "__main__":
    cell_id = 124
    N_ant = 2
    style= "tx_diversity"
    N_rb_dl = 15
    sfn = 0
    Ncp = 1
    N_g = 1
    cfi = 2
    
    print "PDCCH - Encoding"
    print "1. DCI Multiplexing"
    M_pcfich_bits = get_pcfich_length_bits()
    M_phich_bits = get_phich_length_bits(N_g, N_rb_dl)
    print M_pcfich_bits
    print M_phich_bits
    N_REG_unused = 10
    N_CCE_avail = get_n_cce_available(N_ant, N_rb_dl, cfi, M_pcfich_bits, M_phich_bits)
    print N_CCE_avail    
    M_bit = get_pdcch_length_bits(0) #number of bits in one PDCCH
    n_pdcch = 4 #number of PDCCHs in one subframe
    
    total_pdcch_bits = M_bit * n_pdcch # Can PDCCHs have different lengths?

    
    
    print "\n\n\n\n2. Scrambling"
    
    #print "1 CCE == 9REGs"
    #print "RNTI - Radio Network Temporary Identifier - a scrambling sequence"
    
    #encode_dci("mow")
    #encode_format0()
    #encode_format1()
    

    #
    pdcch_cinit = get_pdcch_cinit(1,0)
    
    print "format\tCCEs\tN_REGs\tN_bits"
    for i in range(4):
        get_pdcch_length_bits(i)
            
        
        
        