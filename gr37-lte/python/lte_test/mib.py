# -*- coding: utf-8 -*-
"""
Created on Wed Mar 13 18:03:52 2013

@author: johannes
"""

import math

N_rb_dl_code = { 6:[0,0,0], 15:[0,0,1], 25:[0,1,0],50:[0,1,1],75:[1,0,0],100:[1,0,1]}
phich_res_code = {1.0/6.0:[0,0], 1.0/2.0:[0,1], 1.0:[1,0], 2.0:[1,1]}
def pack_mib(N_rb_dl, phich_dur, phich_res, sfn):
#    print "pack_mib"
    sfn = sfn%1024    
    mib = range(24)
    mib[0:3] = N_rb_dl_code[N_rb_dl] # sometimes written as N_g
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

def main():
    cell_id = 124    
    N_ant = 2
    style= "tx_diversity"
    mib = pack_mib(50,0,1.0, 511)
    print mib
    
if __name__ == "__main__":
    main()
    

