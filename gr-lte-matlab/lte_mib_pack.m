function[mib]=lte_mib_pack(bw,phich_dur,phich_res,sfn)
% Generate MIB with actual information.
% Specification: ETSI TS 136.331 Section 6.2.2 (V10.1.0 page 138)

disp([num2str(bw) ' ' phich_dur ' ' num2str(phich_res) ' ' num2str(sfn)]);

% Value specified by number of used resource blocks
bw_bits=zeros(1,3);
if(bw == 6)
    bw_bits = [0,0,0];
elseif(bw == 15)
    bw_bits = [0,0,1];
elseif(bw == 25)
    bw_bits = [0,1,0];
elseif(bw == 50)
    bw_bits = [0,1,1];
elseif(bw == 75)
    bw_bits = [1,0,0];
elseif(bw == 100)
    bw_bits = [1,0,1];
else
    disp('Bandwidth not available');
end

phich_dur_bit=0;
if phich_dur=='normal'
    phich_dur_bit=0;
elseif phich_dur=='extended'
    phich_dur_bit=1;
else
    disp('phich duration not properly entered');
end
    
phich_res_bits=zeros(1,2);
if(phich_res == 1/6)
    phich_res_bits = [0,0];
elseif(phich_res == 1/2)
    phich_res_bits = [0,1];
elseif(phich_res == 1)
    phich_res_bits = [1,0];
elseif(phich_res == 2)
    phich_res_bits = [1,1];
else
    disp('unknown phich resource size');
end

% System Frame Number(SFN) must have a value between 0 and 1023 (10bit).
sfn_bits=zeros(1,8);
sfn_dec=sfn;
sfn_dec=floor(sfn_dec/2);
sfn_dec=floor(sfn_dec/2);
for i=0:7
    sfn_bits(8-i)=mod(sfn_dec,2);
    sfn_dec=floor(sfn_dec/2);
end
    

mib=[bw_bits phich_dur_bit phich_res_bits sfn_bits zeros(1,10)]';


end