# -*- coding: utf-8 -*-
"""
Created on Wed Mar 13 18:15:38 2013

@author: johannes
"""

from mib import *
from encode_bch import *
from lte_test import *

def nrz_encoding(data):
    out_data = range(len(data))
    for i in range(len(data)):
        out_data[i] = float( (-2.0*data[i]) +1 )
    return out_data


def pbch_scrambling(data, cell_id):
    if len(data) != 120:
        print "wrong length"
        return data
    
    output = []
    for i in range(1920/len(data)):
        output.extend(data)
    scrambled = scrambling(output, cell_id)
    return scrambled
    
def scrambling(data, cell_id):
    pn_sequence = pn_generator(len(data), cell_id)
    return [(data[i]+pn_sequence[i])%2 for i in range(len(data))]

def qpsk_modulation(data):
    output = []
    nrz_data = nrz_encoding(data)
    for i in range(len(nrz_data)/2):
        mod = complex(nrz_data[i*2+0]/math.sqrt(2),nrz_data[i*2+1]/math.sqrt(2))
        output.extend([mod])
    return output
    
def layer_mapping(data, N_ant, style):
    #Do layer Mapping according to ETSI 136.211 Sec 6.3.3.3    
    M_symb = len(data)
    
    if(style != "tx_diversity"):
        print style + "\tnot supported!"
        return data
    
    output = []
    if(N_ant == 1):
        output = data
    elif(N_ant == 2):
        M_layer_symb=M_symb/2
        x=[[],[]]
        for i in range(M_layer_symb):
            x[0].append(data[2*i+0])
            x[1].append(data[2*i+1])
        output = x
    elif(N_ant == 4):
        M_layer_symb = 0
        if(M_symb%4 ==0):
            M_layer_symb=M_symb/4
        else:
            M_layer_symb=(M_symb+2)/4;
            data.extend([0]*2)

        x=[[],[],[],[]]
        for i in range(M_layer_symb):
            x[0].append(data[4*i+0])
            x[1].append(data[4*i+1])
            x[2].append(data[4*i+2])
            x[3].append(data[4*i+3])
        output = x
    return output
    
def pre_coding(data, N_ant, style):
    if(style != "tx_diversity"):
        print style + "\tnot supported!"
        return data
    
    output = []
    if N_ant == 1:
        output = data
        
    elif N_ant == 2:
        y = [[0]*len(data[0])*2,[0]*len(data[0])*2]
        x = data
        for n in range(len(data[0])):
            y[0][2*n]   = complex( 1*x[0][n].real,  1*x[0][n].imag)/math.sqrt(2)
            y[1][2*n]   = complex(-1*x[1][n].real,  1*x[1][n].imag)/math.sqrt(2)
            y[0][2*n+1] = complex( 1*x[1][n].real,  1*x[1][n].imag)/math.sqrt(2)
            y[1][2*n+1] = complex( 1*x[0][n].real, -1*x[0][n].imag)/math.sqrt(2)
        output = y

    else:
        print str(N_ant) + "\tantenna port not supported!"
        return data

    return output
   
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
    print "pre_decoding\t" + style
    print len(data)
    output = [[],[]]
    for n in range(len(data)/2):
        h0 = h[0][2*n]
        h1 = h[1][2*n]
        r0 = data[2*n]
        r1 = data[2*n+1]
        output[0].append(h0.conjugate()*r0 + h1*r1.conjugate())
        output[1].append(h0.conjugate()*r1-h1*r0.conjugate())
        
    for n in range(len(output[0])):
        output[0][n] = output[0][n]/math.sqrt(2)
        output[1][n] = output[1][n]/math.sqrt(2)
                
    return output
    
def encode_pbch(bch, cell_id, N_ant, style):
    print "encode_pbch"
    scrambled = pbch_scrambling(bch, cell_id)
    qpsk_modulated = qpsk_modulation(scrambled)
    layer_mapped = layer_mapping(qpsk_modulated, N_ant, style)
    pre_coded = pre_coding(layer_mapped, N_ant, style)
    
    return pre_coded

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
    
    
    '''    
    for n in range(10):
        print cmpl_str(layer_mapped[0][n]) + "\t" + cmpl_str(pre_decoded[0][n]) + "\t" + cmpl_str(pre_decoded[0][n]-layer_mapped[0][n])

    for n in range(len(pre_decoded[0])):
        if abs(pre_decoded[0][n]-layer_mapped[0][n]) > 0.0001:
            print "ant1 fail"
        elif abs(pre_decoded[1][n]-layer_mapped[1][n]) > 0.0001:
            print "ant2 fail"
    '''

    
    

