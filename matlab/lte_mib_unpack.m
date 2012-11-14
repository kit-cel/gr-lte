function[N_rb_dl,phich_dur,phich_res,sfn_msb]=lte_mib_unpack(mib)

mib=mib';
N_rb_dl=0;
phich_dur='unknown';


% Value specified by number of used resource blocks

if(mib(1:3) == [0,0,0])
    N_rb_dl = 6;
elseif(mib(1:3) == [0,0,1])
    N_rb_dl = 15;
elseif(mib(1:3) == [0,1,0])
    N_rb_dl = 25;
elseif(mib(1:3) == [0,1,1])
    N_rb_dl = 50;
elseif(mib(1:3) == [1,0,0])
    N_rb_dl = 75;
elseif(mib(1:3) == [1,0,1])
    N_rb_dl = 100;
else
    disp('unknown bandwidth');
end

if mib(4)==0
    phich_dur='normal';
elseif mib(4)==1
    phich_dur='extended';
else
    disp('unknown PHICH duration');
end
    
phich_res=0;
if mib(5:6) == [0,0]
    phich_res = 1/6;
elseif mib(5:6) ==[0,1]
    phich_res = 1/2;
elseif mib(5:6) == [1,0]
    phich_res = 1;
elseif mib(5:6) == [1,1]
    phich_res = 2;
else
    disp('unknown PHICH resources');
end

sfn_vec=[mib(7:14) 0 0];
binary_vec=[2^9 2^8 2^7 2^6 2^5 2^4 2^3 2^2 2^1 2^0];
sfn_msb=sum(binary_vec.*sfn_vec);
   













end