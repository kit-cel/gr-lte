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

def pn_generator(vector_len, cinit):
    NC=1600
    x2 = [0]*(3*vector_len+NC)

    for i in range(31):
        x2[i] = cinit%2
        cinit = int(math.floor(cinit/2))

    x1 = [0]*(3*vector_len+NC)
    x1[0] = 1
    for n in range(2*vector_len+NC-3):
        x1[n+31]=(x1[n+3]+x1[n])%2
        x2[n+31]=(x2[n+3]+x2[n+2]+x2[n+1]+x2[n])%2

    output = [0] * vector_len
    for n in range(vector_len):
        output[n]=(x1[n+NC]+x2[n+NC])%2
    return output

def rs_generator(ns, l, cell_id, Ncp):
    N_RB_MAX = 110
    SQRT2 = np.sqrt(2.0)
    cinit = 1024*(7*(ns+1)+l+1)*(2*cell_id+1)+2*cell_id+Ncp
    pn_seq = pn_generator(4*N_RB_MAX,cinit)
    rs_seq = []
    for m in range(2*N_RB_MAX):
        rs_seq.append(complex((1-2*pn_seq[2*m])/SQRT2, (1-2*pn_seq[2*m+1])/SQRT2 ) )
    return rs_seq

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
    for p in range(N_ant):
        frame[p] = map_pbch_to_frame(frame[p], pbch, cell_id, sfn, p)
        frame[p] = frame_map_rs_symbols(frame[p], N_rb_dl, cell_id, Ncp, p)       
    return frame

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
    
    rbs = 15
    
    [p_pos0, p_sym0] = frame_pilot_value_and_position(rbs, 124, 1, 0)
    [p_pos1, p_sym1] = frame_pilot_value_and_position(rbs, 124, 1, 1)
    
    np.save("pilot_pos0", p_pos0)
    np.save("pilot_pos1", p_pos1)
    np.save("pilot_syms", p_sym0)
    
    print "done"
    

    #print rs_seq
    



















