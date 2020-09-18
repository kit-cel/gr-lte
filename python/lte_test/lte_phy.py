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
from . import lte_core
import math
import matplotlib.pyplot as plt


# this function returns a frame with reference and sync symbols.
# Can be used to be repeatedly for frames. Just put payload etc on it.
def generate_phy_frame(cell_id, N_rb_dl, N_ant):
    frame = []
    for ant in range(N_ant):
        frame.append(generate_phy_frame_layer(ant, cell_id, N_rb_dl))
    frame[0][5] = map_sss_to_symbol(frame[0][5], N_rb_dl, cell_id, 0)
    frame[0][6] = map_pss_to_symbol(frame[0][6], N_rb_dl, cell_id % 3)
    frame[0][75] = map_sss_to_symbol(frame[0][75], N_rb_dl, cell_id, 1)
    frame[0][76] = map_pss_to_symbol(frame[0][76], N_rb_dl, cell_id % 3)
    return frame


def generate_phy_frame_layer(ant, cell_id, N_rb_dl):
    Ncp = 1  # a value defined by LTE standard. Here never used different
    frame = np.zeros((140, 12 * N_rb_dl), dtype=np.complex)
    if ant == 0 or ant == 1:
        for i in range(20):
            frame[7 * i] = map_pilots_to_symbol(frame[7 * i], i, 0, ant, cell_id, N_rb_dl, Ncp)
            frame[7 * i + 4] = map_pilots_to_symbol(frame[7 * i + 4], i, 4, ant, cell_id, N_rb_dl, Ncp)
    elif ant == 2 or ant == 3:  # different position for layer 2 and 3
        for i in range(20):
            frame[7 * i + 1] = map_pilots_to_symbol(frame[7 * i + 1], i, 1, ant, cell_id, N_rb_dl, Ncp)
    return frame


def map_pilots_to_symbol(symbol, ns, l, p, cell_id, N_rb_dl, Ncp):
    [vals, pos] = symbol_pilot_value_and_position(N_rb_dl, ns, l, cell_id, Ncp, p)
    for i in range(len(pos)):
        symbol[pos[i]] = vals[i]
    return symbol


def symbol_pilot_value_and_position(N_rb_dl, ns, l, cell_id, Ncp, p):
    N_RB_MAX = 110
    rs_seq = lte_core.rs_generator(ns, l, cell_id, Ncp)
    offset = calc_offset(ns, l, cell_id, p)
    rs_sym_pos = list(range(offset, 12 * N_rb_dl, 6))
    rs_sym_val = rs_seq[N_RB_MAX - N_rb_dl:N_RB_MAX + N_rb_dl]
    return [rs_sym_val, rs_sym_pos]


def calc_offset(ns, l, cell_id, p):
    v = calc_v(ns, l, p)
    return (v + (cell_id % 6)) % 6


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
        v = 3 * (ns % 2)
    elif p == 3:
        v = 3 + 3 * (ns % 2)
    return v


def map_sync_to_symbol(symbol, sync, N_rb_dl):
    for n in range(len(sync)):
        k = n - 31 + (6 * N_rb_dl)  # see LTE standard 211 for details
        symbol[k] = sync[n]
    return symbol


def map_pss_to_symbol(symbol, N_rb_dl, nid):
    pss = get_pss(nid)
    return map_sync_to_symbol(symbol, pss, N_rb_dl)


def get_pss(nid):
    nidu = {0: 25, 1: 29, 2: 34}
    u = nidu[nid]
    seq = np.zeros(63, dtype=np.complex64)
    for n in range(63):
        ep = np.complex64(-1j * (math.pi * u * n * (n + 1) / 63.0))
        seq[n] = math.e ** ep
    return np.delete(seq, 31)


def map_sss_to_symbol(symbol, N_rb_dl, cell_id, pos):
    sss = get_sss(cell_id)[pos]
    return map_sync_to_symbol(symbol, sss, N_rb_dl)


def get_sss(cell_id):
    nid1 = int(np.floor(cell_id / 3))  # [0, 1, 2]
    nid2 = cell_id % 3  # [0, ... , 167]

    q_prime = int (np.floor(nid1 / 30))
    q = int (np.floor(( nid1 + (q_prime * (q_prime + 1) / 2) ) / 30))
    m_prime = nid1 + q * (q + 1) / 2
    m0 = m_prime % 31
    m1 = (m0 + int (np.floor(m_prime / 31)) + 1) % 31

    x_s = np.zeros(31)
    x_s[4] = 1
    for i in range(26):
        x_s[i + 5] = ((x_s[i + 2] + x_s[i] )) % 2
    sX = 1 - 2 * x_s

    x_c = np.zeros(31)
    x_c[4] = 1
    for i in range(26):
        x_c[i + 5] = (x_c[i + 3] + x_c[i] ) % 2
    cX = 1 - 2 * x_c

    x_z = np.zeros(31)
    x_z[4] = 1
    for i in range(26):
        x_z[i + 5] = (x_z[i + 4] + x_z[i + 2] + x_z[i + 1] + x_z[i] ) % 2
    zX = 1 - 2 * x_z

    s0 = np.zeros(31)
    s1 = np.zeros(31)
    for n in range(31):
        s0[n] = sX[(n + m0) % 31]
        s1[n] = sX[(n + m1) % 31]

    c0 = np.zeros(31)
    c1 = np.zeros(31)
    for n in range(31):
        c0[n] = cX[(n + nid2  ) % 31]
        c1[n] = cX[(n + nid2 + 3) % 31]

    z1m0 = np.zeros(31)
    z1m1 = np.zeros(31)
    for n in range(31):
        z1m0[n] = zX[(n + (m0 % 8)) % 31]
        z1m1[n] = zX[(n + (m1 % 8)) % 31]

    sss0 = np.zeros(62)
    sss5 = np.zeros(62)
    for n in range(31):
        sss0[2 * n + 0] = s0[n] * c0[n]
        sss0[2 * n + 1] = s1[n] * c1[n] * z1m0[n]

        sss5[2 * n + 0] = s1[n] * c0[n]
        sss5[2 * n + 1] = s0[n] * c1[n] * z1m1[n]

    return [sss0, sss5]


def transform_to_time_domain(frame, fftlen):
    return np.fft.fft(frame, fftlen)


def transform_to_stream_cp(time_domain_frame, fft_size):
    cp_length = lte_core.get_cp_length(fft_size)
    ecp_length = lte_core.get_ecp_length(fft_size)
    dim = list(np.shape(time_domain_frame)[0:-1])
    dim[-1] = 140 *fft_size + (140 // 7) * ecp_length + (140 - (140 // 7)) * cp_length
    res = np.zeros(dim, dtype=complex)
    for i, layer in enumerate(time_domain_frame):
        pos = 0
        for j, symbol in enumerate(layer):
            if j % 7 == 0:
                res[i, pos:pos+ (fft_size + ecp_length)] = np.pad(symbol, (ecp_length, 0), 'wrap')
                pos += fft_size + ecp_length
            else:
                res[i, pos:pos+ (fft_size + cp_length)] = np.pad(symbol, (cp_length, 0), 'wrap')
                pos += fft_size + cp_length
    return res


def lte_ofdm_mod_frame(frame, fft_size):
    modded = transform_to_time_domain(frame, fft_size)
    return transform_to_stream_cp(modded, fft_size)


def get_mod_frame(cell_id, N_rb_dl, N_ant, fftlen):
    frame = generate_phy_frame(cell_id, N_rb_dl, N_ant)
    return lte_ofdm_mod_frame(frame, fftlen)


def main():
    cell_id = 124
    N_ant = 2
    N_rb_dl = 6
    style = "tx_diversity"
    frame = generate_phy_frame(cell_id, N_rb_dl, N_ant)

    # print np.shape(frame)
    # print np.shape(get_sss(cell_id))
    # mod_frame = lte_ofdm_mod_frame(frame, 128)
    # np.save('lte_samples.npy', mod_frame)
    # plt.plot(np.abs(mod_frame[0]))
    # plt.show()

    s = np.arange(6)
    print(s)
    r = np.tile(s, 2)
    print(r)



if __name__ == "__main__":
    main()
