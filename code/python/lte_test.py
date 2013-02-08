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
    
def nrz_encoding(data):
    out_data = range(len(data))
    for i in range(len(data)):
        out_data[i] = float( (-2.0*data[i]) +1 )
    return out_data
    
if __name__ == "__main__":
    #import sys
    mib = pack_mib(50,0,1.0, 511)
    mib_crc = crc_checksum(mib, 2)
    c_encoded = convolutional_encoder(mib_crc)
    print c_encoded
    print len(c_encoded)
    '''    
    #print mib
    N_ant = 1
    mib_crc = crc_checksum(mib, N_ant)
    print mib_crc[20:]
    mib_crc = crc_checksum(mib, 2)
    print mib_crc[20:]
    mib_crc = crc_checksum(mib, 4)
    print mib_crc[20:]
    #for i in range (100):        
    #    mib = pack_mib(50,0,1.0, i)
    #    print mib[0:14]
    '''
    
    
    
    
    
    
    