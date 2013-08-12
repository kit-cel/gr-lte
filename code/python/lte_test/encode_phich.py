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

def get_n_phich_groups(N_g, N_rb_dl):
    # be careful! only normal cyclic prefix is taken into account.
    # only frame type 1 - FDD
    N_phich_groups = int(math.ceil(N_g * (N_rb_dl/8.0)))
    return N_phich_groups
    
def get_phich_group_length_bits():
    return 3
    
def get_phich_length_bits(N_g, N_rb_dl):
    return get_phich_scr_length("normal") * get_n_phich_groups(N_g, N_rb_dl)
    
def encode_hi(hi):
    return [hi] * 3 # b(0), b(1), b(2)
    

w_seqs = {0:[complex(1,0), complex(1,0), complex(1,0), complex(1,0)],
          1:[complex(1,0), complex(-1,0), complex(1,0), complex(-1,0)],
          2:[complex(1,0), complex(1,0), complex(-1,0), complex(-1,0)],
          3:[complex(1,0), complex(-1,0), complex(-1,0), complex(1,0)],
          4:[complex(0,1), complex(0,1), complex(0,1), complex(0,1)],
          5:[complex(0,1), complex(0,-1), complex(0,1), complex(0,-1)],
          6:[complex(0,1), complex(0,1), complex(0,-1), complex(0,-1)],
          7:[complex(0,1), complex(0,-1), complex(0,-1), complex(0,1)] }
def scramble_hi_cw(cw, n_seq, cp_len, ns, cell_id):
    N_phich_sf = get_N_phich_sf(cp_len)
    seq = []
    for i in range(N_phich_sf):
        seq.extend(cw)
    w_scr = get_w_scr_seq(n_seq)
    cinit = get_phich_cinit(ns, cell_id)
    scr = get_phich_scr_seq(cinit, len(seq))
    return [seq[i]*scr[i]*w_scr[i] for i in range(len(seq))]
    
def get_w_scr_seq(n_seq):
    seq = w_seqs[n_seq]
    res = []
    for i in range(3):
        res.extend(seq)
    return res
    
def get_N_phich_sf(cp_len):
    if cp_len == "normal":
        return 4
    elif cp_len == "extended":
        return 2
    else:
        print "invalid input value"
        return 0
        
def get_phich_scr_length(cp_len):
    return 3 * get_N_phich_sf(cp_len)
    
def get_phich_cinit(ns, cell_id):
    cinit = (math.floor(ns/2.0)+1) * (2*cell_id + 1) * (2**9) + cell_id
    return cinit
    
def get_phich_scr_seq(cinit, scr_len):
    scr = pn_generator(scr_len, cinit)
    return nrz_encoding(scr)

if __name__ == "__main__":
    cell_id = 124
    N_ant = 2
    style= "tx_diversity"
    N_rb_dl = 6
    sfn = 0
    Ncp = 1
    N_g = 1
    cp_len = "normal"
    n_seq = 0
    ns = 0
    
    cw = encode_hi(1)
    scramble_hi_cw(cw, n_seq, cp_len, ns, cell_id)
    for i in range(8):
        print scramble_hi_cw(cw, i, cp_len, ns, cell_id)





