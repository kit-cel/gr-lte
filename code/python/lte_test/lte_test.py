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
from mib import *
from encode_bch import *
from encode_pbch import *



def cmpl_str(val):
    #return '%+03f%+03fj' %(val.real, val.imag)
    return '{0.real:+.4f} {0.imag:+.4f}j'.format(val)
    
def nrz_hard_decode(data):
    res = []
    for i in range(len(data)):
        if data[i] < 0.0:
            res.append(0)
        else:
            res.append(1)
    return res
    
    
if __name__ == "__main__":
    cell_id = 124    
    N_ant = 2
    style= "tx_diversity"
    mib = pack_mib(50,0,1.0, 511)
    
    bch = encode_bch(mib, N_ant)

    scrambled = pbch_scrambling(bch, cell_id)
    qpsk_modulated = qpsk_modulation(scrambled)
    layer_mapped = layer_mapping(qpsk_modulated, N_ant, style)
    pre_coded = pre_coding(layer_mapped, N_ant, style)

    pbch = encode_pbch(bch, cell_id, N_ant, style)
    
    print len(pre_coded)
    print len(pbch)
    print len(pre_coded[0])
    print len(pbch[0])
    
    for n in range(len(pbch[0])):
        if pbch[0][n] != pre_coded[0][n]:
            print "ant0 failed!"
        elif pbch[1][n] != pre_coded[1][n]:
            print "ant1 failed!"
    
    
    rx = [pre_coded[0][n]+pre_coded[1][n] for n in range(len(pre_coded[0]))]
    h = [[complex(1,0)]*len(pre_coded[0]),[complex(1,0)]*len(pre_coded[0])]
    pre_decoded = pre_decoding(rx, h, N_ant, style)
    
    

    




















