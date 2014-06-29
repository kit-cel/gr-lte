function[cr]=lte_qpsk_modulator(bit1,bit2)

if bit1==0 && bit2==0
    cr = complex(1,1);    
end
if bit1==0 && bit2==1
    cr = complex(1,-1);    
end
if bit1==1 && bit2==0
    cr = complex(-1,1);    
end
if bit1==1 && bit2==1
    cr = complex(-1,-1);    
end
cr = cr/sqrt(2);


end
