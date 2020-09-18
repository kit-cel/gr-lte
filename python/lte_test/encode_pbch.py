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

from .mib import *
from .encode_bch import *
# from lte_core import *


def pbch_scrambling(data, cell_id):
    if len(data) != 120:
        print("wrong length")
        return data

    output = expand_for_scrambling(data)
    scrambled = scrambling(output, cell_id)
    return scrambled


def expand_for_scrambling(data):
    output = []
    for i in range(1920 / len(data)):
        output.extend(data)
    return output


def scrambling(data, cell_id):
    pn_sequence = pn_generator(len(data), cell_id)
    return [(data[i] + pn_sequence[i]) % 2 for i in range(len(data))]


def pre_decoding(data, h, N_ant, style):
    '''
    alamouti Coding
        time0   time1
    ant0  x0    x1
    ant1 -x1*   x0*
    
    RX
    r0 = h0 x0 - x1* h1
    r1 = h0 x1 + h1 x0*
    
    estimate
    e_x0 = h0* r0 + h1 r1*
    e_x1 = h0* r1 - h1 r0*
    '''
    print("pre_decoding\t" + style)
    print(len(data))
    output = [[], []]
    for n in range(len(data) / 2):
        h0 = h[0][2 * n]
        h1 = h[1][2 * n]
        r0 = data[2 * n]
        r1 = data[2 * n + 1]
        output[0].append(h0.conjugate() * r0 + h1 * r1.conjugate())
        output[1].append(h0.conjugate() * r1 - h1 * r0.conjugate())

    for n in range(len(output[0])):
        output[0][n] = output[0][n] / math.sqrt(2)
        output[1][n] = output[1][n] / math.sqrt(2)

    return output


def encode_pbch(bch, cell_id, N_ant, style):
    # print "encode_pbch"
    scrambled = pbch_scrambling(bch, cell_id)
    qpsk_modulated = qpsk_modulation(scrambled)
    layer_mapped = layer_mapping(qpsk_modulated, N_ant, style)
    pre_coded = pre_coding(layer_mapped, N_ant, style)

    return pre_coded


def main():
    cell_id = 124
    N_ant = 2
    style = "tx_diversity"
    mib = pack_mib(50, 0, 1.0, 511)

    bch = encode_bch(mib, N_ant)

    scrambled = pbch_scrambling(bch, cell_id)
    qpsk_modulated = qpsk_modulation(scrambled)
    layer_mapped = layer_mapping(qpsk_modulated, N_ant, style)
    pre_coded = pre_coding(layer_mapped, N_ant, style)

    pbch = encode_pbch(bch, cell_id, N_ant, style)

    print(np.shape(scrambled))
    print(np.shape(pre_coded))
    print(np.shape(pbch))
    print(np.shape(pre_coded[0]))
    print(np.shape(pbch[0]))

    for n in range(len(pbch[0])):
        if pbch[0][n] != pre_coded[0][n]:
            print("ant0 failed!")
        elif pbch[1][n] != pre_coded[1][n]:
            print("ant1 failed!")

    rx = [pre_coded[0][n] + pre_coded[1][n] for n in range(len(pre_coded[0]))]
    h = [[complex(1, 0)] * len(pre_coded[0]), [complex(1, 0)] * len(pre_coded[0])]
    pre_decoded = pre_decoding(rx, h, N_ant, style)


if __name__ == "__main__":
    main()