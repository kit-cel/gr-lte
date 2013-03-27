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

import numpy as np
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
    
def generate_frame(pbch, N_rb_dl, cell_id, sfn, N_ant):
    n_carriers = 12*N_rb_dl
    frame = np.zeros((N_ant,140,n_carriers),dtype=np.complex)
    
    for n in range(N_ant):
        frame = map_pbch_to_frame(frame, pbch, cell_id, sfn, n)
    
    
    return frame
    
def map_pbch_to_frame(frame, pbch, cell_id, sfn, ant):    
    sfn_mod4 = sfn%4
    cell_id_mod3 = cell_id%3
    pbch = pbch[ant]
    pbch_part = pbch[240*sfn_mod4:240*(sfn_mod4+1)]
    n_carriers = np.shape(frame)[2]
    #print n_carriers
    pbch_pos = (n_carriers/2)-(72/2)
    #print pbch_pos
    pbch_element = 0
    for i in range(72):
        if i%3 != cell_id_mod3:
            #print "hello" + str(i+pbch_pos)
            frame[ant][7][i+pbch_pos]=pbch_part[pbch_element]
            frame[ant][8][i+pbch_pos]=pbch_part[pbch_element+48]
            pbch_element = pbch_element+1
        frame[ant][9][i+pbch_pos]=pbch_part[i+2*48]
        frame[ant][10][i+pbch_pos]=pbch_part[i+2*48+72]

    return frame
    
    
if __name__ == "__main__":
    cell_id = 124    
    N_ant = 2
    style= "tx_diversity"
    N_rb_dl = 6
    sfn = 0
    
    mib = pack_mib(50,0,1.0, 511)
    bch = encode_bch(mib, N_ant)
    pbch = encode_pbch(bch, cell_id, N_ant, style)
    
    frame = generate_frame(pbch, N_rb_dl, cell_id, sfn, N_ant)
    print frame[0][10]
    #print np.ndim(frame)
    print np.shape(frame)
    
    '''
    scrambled = pbch_scrambling(bch, cell_id)
    qpsk_modulated = qpsk_modulation(scrambled)
    layer_mapped = layer_mapping(qpsk_modulated, N_ant, style)
    pre_coded = pre_coding(layer_mapped, N_ant, style)
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
    '''
    

    




















