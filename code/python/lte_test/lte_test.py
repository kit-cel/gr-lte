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
from encode_pcfich import *

def rs_generator(ns, l, cell_id, Ncp):
    N_RB_MAX = 110
    SQRT2 = np.sqrt(2.0)
    cinit = 1024*(7*(ns+1)+l+1)*(2*cell_id+1)+2*cell_id+Ncp
    pn_seq = pn_generator(4*N_RB_MAX,cinit)
    rs_seq = []
    for m in range(2*N_RB_MAX):
        rs_seq.append(complex((1-2*pn_seq[2*m])/SQRT2, (1-2*pn_seq[2*m+1])/SQRT2 ) )
    return rs_seq
    
def nrz_encoding(data):
    out_data = range(len(data))
    for i in range(len(data)):
        out_data[i] = float( (-2.0*data[i]) +1 )
    return out_data

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
    Ncp = 1
    pcfich = encode_pcfich(2, cell_id, 0, N_ant)
    for p in range(N_ant):
        frame[p] = map_pbch_to_frame(frame[p], pbch, cell_id, sfn, p)
        frame[p] = frame_map_rs_symbols(frame[p], N_rb_dl, cell_id, Ncp, p)
        frame[p] = map_pcfich_to_frame(frame[p], pcfich[p], N_rb_dl, cell_id, p)
    return frame
    
def generate_stream_frame(frame, N_ant):
    if N_ant == 1:
        return frame.flatten()
    elif N_ant == 2:
        frame0 = frame[0].flatten()
        frame1 = frame[1].flatten()
        combined = []
        for i in range(len(frame0)):
            combined.append(frame0[i] + frame1[i])
        return combined
        print "test"

def rs_symbol_mapper(ofdm_symbol, N_rb_dl, ns, l, cell_id, Ncp, p):
    #print "rs_symbol_mapper\t" + str(p) + "\t" + str(ns) + "\t" + str(l)
    rs_seq = rs_generator(ns, l, cell_id, Ncp)
    N_RB_MAX = 110
    offset = calc_offset(ns, l, cell_id, p)
    for m in range(2*N_rb_dl):
        mx = m +N_RB_MAX-N_rb_dl
        k = 6*m + offset
        ofdm_symbol[k] = rs_seq[mx]

    return ofdm_symbol
    
def frame_map_rs_symbols(frame, N_rb_dl, cell_id, Ncp, p):
    [rs_pos_frame, rs_val_frame] = frame_pilot_value_and_position(N_rb_dl, cell_id, Ncp, p)
    for i in range(len(frame)):
        if len(rs_pos_frame[i]) > 0:
            for sym in range(len(rs_pos_frame[i])):
                frame[i][rs_pos_frame[i][sym]] = rs_val_frame[i][sym]
    return frame
    
def frame_pilot_value_and_position(N_rb_dl, cell_id, Ncp, p):
    rs_pos_frame = []
    rs_val_frame = []
    for ns in range(20):
        sym0 = symbol_pilot_value_and_position(N_rb_dl, ns, 0, cell_id, Ncp, p)
        sym4 = symbol_pilot_value_and_position(N_rb_dl, ns, 4, cell_id, Ncp, p)
        rs_pos_frame.extend([sym0[0], [], [], [], sym4[0], [], [] ])
        rs_val_frame.extend([sym0[1], [], [], [], sym4[1], [], [] ])
    return [rs_pos_frame, rs_val_frame]
         
def symbol_pilot_value_and_position(N_rb_dl, ns, l, cell_id, Ncp, p):
    N_RB_MAX = 110
    rs_seq = rs_generator(ns, l, cell_id, Ncp)
    offset = calc_offset(ns, l, cell_id, p)
    rs_sym_pos = range(offset, 12*N_rb_dl, 6)
    rs_sym_val = rs_seq[N_RB_MAX-N_rb_dl:N_RB_MAX+N_rb_dl]
    return [rs_sym_pos, rs_sym_val]
    
def calc_offset(ns, l, cell_id, p):
    v = calc_v(ns, l,  p)
    return ( v + (cell_id%6) )%6

def calc_v(ns, l, p):
    v = 0
    if p == 0 and l == 0:
        v = 0
    elif p == 0 and l != 0:
        v = 3
    elif p == 1 and l == 0:
        v = 3
    elif p == 1 and l != 0:
        v = 0
    elif p == 2:
        v = 3*(ns%2)
    elif p==3:
        v = 3*3*(ns%2)
    return v

def map_pbch_to_frame(frame, pbch, cell_id, sfn, ant):
    sfn_mod4 = sfn%4
    cell_id_mod3 = cell_id%3
    pbch = pbch[ant]
    pbch_part = pbch[240*sfn_mod4:240*(sfn_mod4+1)]
    n_carriers = np.shape(frame)[1]
    #print n_carriers
    pbch_pos = (n_carriers/2)-(72/2)
    #print pbch_pos
    pbch_element = 0
    for i in range(72):
        if i%3 != cell_id_mod3:
            #print "hello" + str(i+pbch_pos)
            frame[7][i+pbch_pos]=pbch_part[pbch_element]
            frame[8][i+pbch_pos]=pbch_part[pbch_element+48]
            pbch_element = pbch_element+1
        frame[9][i+pbch_pos]=pbch_part[i+2*48]
        frame[10][i+pbch_pos]=pbch_part[i+2*48+72]

    return frame

def map_pcfich_to_frame(frame, pcfich, N_rb_dl, cell_id, ant):
    #print "tadada map pcfich " + str(ant)
    for i in range(len(frame)):
        if i%14 == 0:
            frame[i] = map_pcfich_to_symbol(frame[i], pcfich, N_rb_dl, cell_id, ant)
    return frame
            
        
def map_pcfich_to_symbol(symbol, pcfich, N_rb_dl, cell_id, ant):     
    #print "map pcfich"
    N_sc_rb = 12 # number of subcarriers per resource block
    k_mean = (N_sc_rb/2) * (cell_id%(2*N_rb_dl))
    regs = 4
    cell_id_mod3 = cell_id%3
    n_pcfi = 0
    pcfich_pos = calculate_pcfich_pos(N_rb_dl, cell_id)
    for n in range(len(pcfich)):
        symbol[pcfich_pos[n]] = pcfich[n]
#    for n in range(regs):
#        k = int( (k_mean + (N_sc_rb/2) * math.floor(n*N_rb_dl/2))%(N_rb_dl*N_sc_rb) )
#        for i in range(6):
#            if i%3 != cell_id_mod3:
#                symbol[k+i] = pcfich[n_pcfi]
#                n_pcfi = n_pcfi + 1

    return symbol
    
def calculate_pcfich_pos(N_rb_dl, cell_id):
    N_sc_rb = 12 # number of subcarriers per resource block
    k_mean = (N_sc_rb/2) * (cell_id%(2*N_rb_dl))
    regs = 4
    cell_id_mod3 = cell_id%3
    pcfich_pos = []
    for n in range(regs):
        k = int( (k_mean + (N_sc_rb/2) * math.floor(n*N_rb_dl/2))%(N_rb_dl*N_sc_rb) )
        for i in range(6):
            if i%3 != cell_id_mod3:
                pcfich_pos.append(k+i)
    return pcfich_pos

if __name__ == "__main__":
    cell_id = 124
    N_ant = 2
    style= "tx_diversity"
    N_rb_dl = 6
    sfn = 0
    Ncp = 1

    mib = pack_mib(50,0,1.0, 511)
    bch = encode_bch(mib, N_ant)
    pbch = encode_pbch(bch, cell_id, N_ant, style)

    pn_seq = pn_generator(220, cell_id)
    rs_seq = rs_generator(3, 4, cell_id,Ncp)

    frame = generate_frame(pbch, N_rb_dl, cell_id, sfn, N_ant)
    
    pcfich = encode_pcfich(2, cell_id, 4, N_ant)
    
    print calculate_pcfich_pos(N_rb_dl, cell_id)
    
    stream = generate_stream_frame(frame, 2)
    print np.shape(stream)

    

    



















