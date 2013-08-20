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
    seq = nrz_encoding(seq)
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

def align_phich_block(data, cp_len, n_phich_group):
    # This block is still a dummy.
    # Only with extended CP things must be taken care of.
    return data
    
def encode_one_phich(hi, cp_len, ns, cell_id, n_phich_group, n_phich_seq, N_ant, style):
    cw = encode_hi(hi)
    scr_phich = scramble_hi_cw(cw, n_phich_seq, cp_len, ns, cell_id)
    ali_phich = align_phich_block(scr_phich, cp_len, n_phich_group)
    lay_phich = layer_mapping(ali_phich, N_ant, style)
    pre_phich = pre_coding(lay_phich, N_ant, style)
    return pre_phich
    
def encode_phich_group(hi, cp_len, ns, cell_id, n_phich_group, N_ant, style):
    group = []
    for n_seq in range(len(hi)):
        one_phich = encode_one_phich(hi[n_seq], cp_len, ns, cell_id, n_phich_group, n_seq, N_ant, style)
        group.append(one_phich)
    return group
            
def get_phich_group_sum_ant(phich_group):
    res = []
    for i in range(len(phich_group[0])):
        val = 0
        for n in range(len(phich_group)):
            val = val + phich_group[n][i]
            #print str(n) + "\t" + str(val)
        res.append(val)
    return res

def get_phich_group_sum(phich_group, N_ant):
    if N_ant == 1:
        return get_phich_group_sum_ant(phich_group)
    else:
        res = []
        for i in range(N_ant):
            ants = []
            for n in range(len(phich_group)):
                ants.append(phich_group[n][i])
            ant_sum = get_phich_group_sum_ant(ants)
            res.append(ant_sum)
        return res
        
def map_phich_group_to_unit(data, cp_len):
    # If one wants to use EXTENDED CP len.
    # Here is the starting point for impl
    return data
    
def encode_phich(hi, N_rb_dl, N_g, cp_len, ns, cell_id, N_ant, style):
    n_groups = get_n_phich_groups(N_g, N_rb_dl) # for normal cp all fine
    groups = []
    for n in range(n_groups):
        enc_group = encode_phich_group(hi[8*n:8*(n+1)], cp_len, ns, cell_id, n, N_ant, style)
        sum_group = get_phich_group_sum(enc_group, N_ant)
        groups.append(sum_group)
    if N_ant > 1:
        groups = zip(*groups)
    else:
        groups = [groups]
    res = []
    for p in range(N_ant):
        ant = groups[p]
        part = []
        for i in range(len(ant)):
            part.extend(ant[i])
        res.append(part)
    return res

        
N_g_lut = {0:1/6.0, 1:1/2.0, 2:1, 3:2}
N_rb_dl_lut = {0:6, 1:15, 2:25, 3:50, 4:75, 5:100}
if __name__ == "__main__":
    cell_id = 124
    N_ant = 1
    style= "tx_diversity"
    N_rb_dl = 50
    sfn = 0
    Ncp = 1
    N_g = 1
    cp_len = "normal"
    n_seq = 0
    ns = 0
    n_phich_group = 0
    
    
    for i in range(4):
        n_g = N_g_lut[i]
        for n in range(len(N_rb_dl_lut)):
            n_rb_dl = N_rb_dl_lut[n]
            n_groups = get_n_phich_groups(n_g, n_rb_dl)
            print '{0:+.3f}\t{1}\t{2}'.format(n_g, n_rb_dl, n_groups)
        
    
    n_groups = get_n_phich_groups(N_g, N_rb_dl)
    hi = []
    for i in range(8*n_groups):
        hi.append(i%2)
    
    #phich_group = encode_phich_group(hi, cp_len, ns, cell_id, n_phich_group, N_ant, style)
    #print phich_group
    #print np.shape(phich_group)
    #group_sum = get_phich_group_sum(phich_group, N_ant)
    #print np.shape(group_sum)
    
    phich = encode_phich(hi, N_rb_dl, N_g, cp_len, ns, cell_id, N_ant, style)
    print "this is the result"
    print np.shape(phich)
    print n_groups
    print 12 * 7
        
    
        
    





