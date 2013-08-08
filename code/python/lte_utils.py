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
import numpy as np

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
    
def rs_generator(ns, l, cell_id, Ncp):
    N_RB_MAX = 110
    SQRT2 = np.sqrt(2.0)
    cinit = 1024*(7*(ns+1)+l+1)*(2*cell_id+1)+2*cell_id+Ncp
    pn_seq = pn_generator(4*N_RB_MAX,cinit)
    rs_seq = []
    for m in range(2*N_RB_MAX):
        rs_seq.append(complex((1-2*pn_seq[2*m])/SQRT2, (1-2*pn_seq[2*m+1])/SQRT2 ) )
    return rs_seq
    
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
    
    
if __name__ == "__main__":
    N_rb_dl = 6
    cell_id = 124
    Ncp = 1
    ant = 0    
    
    [pilot_pos, pilot_vals] = frame_pilot_value_and_position(N_rb_dl, cell_id, Ncp, ant)
    print pilot_pos

























