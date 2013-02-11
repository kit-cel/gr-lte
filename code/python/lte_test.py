#!/usr/bin/env python
#This file contains Python functions to generate testdata!
import math
import numpy

def test_print():
    print "Hello World"
    
    
#This function packs the mib with given values!
N_rb_dl_code = { 6:[0,0,0], 15:[0,0,1], 25:[0,1,0],50:[0,1,1],75:[1,0,0],100:[1,0,1]}
phich_res_code = {1.0/6.0:[0,0], 1.0/2.0:[0,1], 1.0:[1,0], 2.0:[1,1]}
def pack_mib(N_rb_dl, phich_dur, phich_res, sfn):
    sfn = sfn%1024    
    mib = range(24)
    mib[0:3] = N_rb_dl_code[N_rb_dl]
    mib[3] = phich_dur
    mib[4:6] = phich_res_code[phich_res]
    sfn_code = range(8)
    sfn = int(math.floor(sfn/4))
    
    for i in range(8):
        sfn_code[7-i] = sfn%2
        sfn = int(math.floor(sfn/2))
        
    mib[6:14] = sfn_code
    for i in range(10):
        mib[i+14] = 0
        
    return mib
    
def crc_checksum(mib, N_ant):
    if len(mib) != 24:
        print "FATAL ERROR: length mismatch!"
        return [0]*40
        
    crc16 = [1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1] #CRC16 Polynom for LTE
    mib_crc = []
    mib_crc.extend(mib)
    mib_crc.extend([0]*16)
    lfsr = [0]*17
    
    for i in range(len(mib_crc)):
        #print lfsr
        lead = lfsr[0]
        lfsr[0:16] = lfsr[1:17]
        lfsr[16] = lead
        if lfsr[0] == 1:
            for i in range(len(lfsr)):
                lfsr[i] = (lfsr[i]+crc16[i])%2
        lfsr[len(lfsr)-1] = (lfsr[len(lfsr)-1]+mib_crc[i])%2

    if N_ant == 1:
        for i in range(16):
            mib_crc[i+24] = lfsr[i+1]
    if N_ant == 2:
        for i in range(16):
            mib_crc[i+24] = (lfsr[i+1]+1)%2
    final_xor = [0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1]
    if N_ant == 4:
        for i in range(16):
            mib_crc[i+24] = (lfsr[i+1]+final_xor[i])%2
            
    return mib_crc
    
def convolutional_encoder(input_data):      
    data = input_data[len(input_data)-6:]
    data.extend(input_data)
    
    reg = [0]*6        
    c_encoded = [0]*3*len(data)
    for i in range(len(data)):
        c_encoded[i*3+0] = (data[i]+reg[1]+reg[2]+reg[4]+reg[5] )%2
        c_encoded[i*3+1] = (data[i]+reg[0]+reg[1]+reg[2]+reg[5] )%2
        c_encoded[i*3+2] = (data[i]+reg[0]+reg[1]+reg[3]+reg[5] )%2
        
        reg[1:6] = reg[0:5]
        reg[0] = data[i]
        
    return c_encoded[18:]

def convolutional_encoder_sorted(input_data):
    c_encoded = convolutional_encoder(input_data)
    c_encoded_sorted = range(120)
    for i in range(40):
        c_encoded_sorted[i] = c_encoded[i*3+0]
        c_encoded_sorted[i+40] = c_encoded[i*3+1]
        c_encoded_sorted[i+80] = c_encoded[i*3+2]

    return c_encoded_sorted    

def nrz_encoding(data):
    out_data = range(len(data))
    for i in range(len(data)):
        out_data[i] = float( (-2.0*data[i]) +1 )
    return out_data

def rate_match(data):
    output = []
    output.extend(interleave(data[0:40]))
    output.extend(interleave(data[40:80]))
    output.extend(interleave(data[80:120]))
    return output
    
def interleave(data):
    interleave_vector = tuple([1,17,9,25,5,21,13,29,3,19,11,27,7,23,15,31,0,16,8,24,4,20,12,28,2,18,10,26,6,22,14,30])
    n_col = 32
    n_row = int(math.ceil(40.0/(float(n_col))))
    n_null = n_col*n_row - 40
    y = [0]*n_null
    y.extend([i+1 for i in range(40)])
    matrix = [y[0:32], y[32:64]]
    int_matrix = [[0]*32, [0]*32]
    for i in range(n_col):
        int_matrix[0][i] = matrix[0][interleave_vector[i]]
        int_matrix[1][i] = matrix[1][interleave_vector[i]]

    int_vector = []    
    for i in range(32):
        int_vector.append(int_matrix[0][i])
        int_vector.append(int_matrix[1][i])
    
    output = range(len(data))
    idx = 0
    for i in range(len(int_vector)):
        if int_vector[i] != 0:
            output[idx] = data[int_vector[i]-1]
            idx = idx+1

    return output

def encode_bch(mib, N_ant):
    mib_crc = crc_checksum(mib, N_ant)
    c_enc_sorted = convolutional_encoder_sorted(mib_crc)
    return rate_match(c_enc_sorted)

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
    
if __name__ == "__main__":
    #import sys
    mib = pack_mib(50,0,1.0, 511)
    mib_crc = crc_checksum(mib, 2)
    c_encoded = convolutional_encoder(mib_crc)
    c_enc_sorted = convolutional_encoder_sorted(mib_crc)
    c_matched = rate_match(c_enc_sorted)
    
    bch = encode_bch(mib, 2)
    
    scrambled = scrambling(bch, 124)
    p_scrambled = pbch_scrambling(c_matched, 124)
    print len(p_scrambled)
    




















