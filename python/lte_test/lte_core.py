# !/usr/bin/env python
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


def pn_generator(vector_len, cinit):
    NC = 1600
    vec_len = vector_len
    if vector_len < 32:
        vec_len = 32
    x2 = [0] * (3 * vec_len + NC)
    for i in range(31):
        x2[i] = cinit % 2
        cinit = int(math.floor(cinit / 2))
    x1 = [0] * (3 * vec_len + NC)
    x1[0] = 1
    for n in range(2 * vec_len + NC - 3):
        x1[n + 31] = (x1[n + 3] + x1[n]) % 2
        x2[n + 31] = (x2[n + 3] + x2[n + 2] + x2[n + 1] + x2[n]) % 2
    output = [0] * vector_len
    for n in range(vector_len):
        output[n] = (x1[n + NC] + x2[n + NC]) % 2
    return output


def scramble_sequence(seq, cinit):
    pn_seq = pn_generator(len(seq), cinit)
    return [(seq[i] + pn_seq[i]) % 2 for i in range(len(seq))]


def rs_generator(ns, l, cell_id, Ncp):
    N_RB_MAX = 110
    SQRT2 = np.sqrt(2.0)
    cinit = 1024 * (7 * (ns + 1) + l + 1) * (2 * cell_id + 1) + 2 * cell_id + Ncp
    pn_seq = pn_generator(4 * N_RB_MAX, cinit)
    rs_seq = []
    for m in range(2 * N_RB_MAX):
        rs_seq.append(complex((1 - 2 * pn_seq[2 * m]) / SQRT2, (1 - 2 * pn_seq[2 * m + 1]) / SQRT2))
    return rs_seq


def nrz_encoding(data):
    out_data = list(range(len(data)))
    for i in range(len(data)):
        out_data[i] = float((-2.0 * data[i]) + 1)
    return out_data


def qpsk_modulation(data):
    output = []
    nrz_data = nrz_encoding(data)
    for i in range(len(nrz_data) / 2):
        mod = complex(nrz_data[i * 2 + 0] / math.sqrt(2), nrz_data[i * 2 + 1] / math.sqrt(2))
        output.extend([mod])
    return output


def bpsk_modulation(data):
    res = []
    nrz_data = nrz_encoding(data)
    for i in range(len(nrz_data)):
        res.append(complex(nrz_data[i] / math.sqrt(2), nrz_data[i] / math.sqrt(2)))
    return res


def layer_mapping(data, N_ant, style):
    #Do layer Mapping according to ETSI 136.211 Sec 6.3.3.3    
    M_symb = len(data)
    if style != "tx_diversity":
        print(style + "\tnot supported!")
        return data
    output = []
    if N_ant == 1:
        output = [data, ]
    elif N_ant == 2:
        M_layer_symb = M_symb / 2
        x = [[], []]
        for i in range(M_layer_symb):
            x[0].append(data[2 * i + 0])
            x[1].append(data[2 * i + 1])
        output = x
    elif N_ant == 4:
        M_layer_symb = 0
        if M_symb % 4 == 0:
            M_layer_symb = M_symb / 4
        else:
            M_layer_symb = (M_symb + 2) / 4
            data.extend([0] * 2)

        x = [[], [], [], []]
        for i in range(M_layer_symb):
            x[0].append(data[4 * i + 0])
            x[1].append(data[4 * i + 1])
            x[2].append(data[4 * i + 2])
            x[3].append(data[4 * i + 3])
        output = x
    return output


def prepare_for_demapper_block(lay, N_ant, style):
    if N_ant == 1:
        return lay
    elif N_ant == 2 or N_ant == 4:
        res = []
        for i in range(len(lay)):
            res.extend(lay[i])
        return res
    else:
        print("invalid arguments")
        return lay


def pre_coding(data, N_ant, style):
    if style != "tx_diversity":
        print(style + "\tnot supported!")
        return data
    output = []
    if N_ant == 1:
        output = data
    elif N_ant == 2:
        y = [[0] * len(data[0]) * 2, [0] * len(data[0]) * 2]
        x = data
        for n in range(len(data[0])):
            y[0][2 * n] = complex(1 * x[0][n].real, 1 * x[0][n].imag) / math.sqrt(2)
            y[1][2 * n] = complex(-1 * x[1][n].real, 1 * x[1][n].imag) / math.sqrt(2)
            y[0][2 * n + 1] = complex(1 * x[1][n].real, 1 * x[1][n].imag) / math.sqrt(2)
            y[1][2 * n + 1] = complex(1 * x[0][n].real, -1 * x[0][n].imag) / math.sqrt(2)
        output = y
    else:
        print(str(N_ant) + "\tantenna port not supported!")
        return data
    return output


def interleave_row(data):
    interleave_vector = tuple(
        [1, 17, 9, 25, 5, 21, 13, 29, 3, 19, 11, 27, 7, 23, 15, 31, 0, 16, 8, 24, 4, 20, 12, 28, 2, 18, 10, 26, 6, 22,
         14, 30])  # defined in standard
    return [data[idx] for idx in interleave_vector]


def interleave(data):
    n_col = 32  # defined in standard
    data_len = len(data)
    n_row = int(math.ceil(data_len / (float(n_col))))
    n_null = n_col * n_row - data_len
    #print "col = {0}\trow = {1}\tnull = {2}".format(n_col, n_row, n_null)
    y = [None] * n_null
    y.extend(data)
    matrix = []
    for i in range(n_row):
        part = y[i * n_col: (i + 1) * (n_col)]
        matrix.append(part)
    zip_mat = list(zip(*matrix))
    intld = interleave_row(zip_mat)
    vec = []
    for i in range(len(intld)):
        vec.extend(intld[i])
    res = [0] * len(data)
    idx = 0
    for i in range(len(vec)):
        if vec[i] != None:
            res[idx] = vec[i]
            idx = idx + 1
    return res


def get_cp_length(fft_len):
    return int(144 * fft_len / 2048)


def get_ecp_length(fft_len):
    return int(160 * fft_len / 2048)


def get_slot_length(fft_len):
    return fft_len * 7 + get_ecp_length(fft_len) + 6 * get_cp_length(fft_len)


def main():
    cell_id = 124
    N_ant = 2
    style = "tx_diversity"
    N_rb_dl = 6
    sfn = 0
    Ncp = 1


    dataset = np.ones([30, ])
    print(np.shape(dataset))
    print(np.shape(layer_mapping(dataset, 1, style)))
    print(np.shape(pre_coding(layer_mapping(dataset, 1, style), 1, style)))


    mat = []
    for i in range(80):
        mat.append([i] * 3)
    inter = interleave(mat)

    arr = list(range(80))
    intl = interleave(arr)
    # for i in range(len(arr)):
    #     print "{0}\t{1}".format(intl[i], inter[i])


if __name__ == "__main__":
    main()
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    