function[my_slot]=slot7()
% Returns a Matrix with the values of 1 slot
% there are 20 slots per frame
% arguments are: 
% slot_number
cell_id_g=42;
cell_id_n=0;
cell_id=3*cell_id_g+cell_id_n;
ns=0; % is the slot number within a radio frame
Ncp=1; 

p=0; % is the antenna port
Nrbdlmin = 6; % are the smallest
Nrbdlmax = 110; % and largest downlink bandwidth
Nrbdl = 6; % actual downlink bandwidth



% slot_number=0;
my_slot = zeros(72,7);
my_slot((1:72),7) = pss_symbol(cell_id_n);
sss_channel=sss_symbol(cell_id_g,cell_id_n);
my_slot((6:67),6) = sss_channel;

% Generate and Map for first OFDM symbol
l=0; % is the OFDM symbol number within the slot
% Get reference signals
r=ref_sig_generation(ns,l,cell_id,Ncp);

% calculate values for reference signal mapping
vshift=mod(cell_id,6);
v=0;
if p==0 && l==0
    v=0;
elseif p==0 && l~=0
    v=3;
elseif p==1 && l==0
    v=3;
elseif p==1 && l~=0
    v=0;    
 elseif p==2
    v=3*mod(ns,2);   
 elseif p==3
    v=3*3*mod(ns,2);
end


for m=0:2*Nrbdl-1
    mX=m+Nrbdlmax-Nrbdl;
    k=6*m+mod( (v+vshift),6);
    my_slot(k+1,l+1)=r(mX+1); 
end


% Generate and Map PBCH channel to ressource elements in slot 0 of a radio
% frame. The part of the PBCH channel which will be mapped to the resource
% elements is chosen by mod(ns,4).

% testdata generation
mib=mod(randi(1000,24,1),2);

pbch_vec=lte_pbch(mib);
mpbch=1;
for l=0:3
    for k=0:71
        if (l==0||l==1) && mod(k,3)~=0
            my_slot(k+1,l+1)=pbch_vec(mpbch);
            mpbch=mpbch+1;
        elseif l==2 || l==3
            my_slot(k+1,l+1)=pbch_vec(mpbch);
            mpbch=mpbch+1;
        end
    end
end


% Generate and Map for fifth OFDM symbol
l=4;
% Get reference signals
r=ref_sig_generation(ns,l,cell_id,Ncp);

% calculate values for reference signal mapping
vshift=mod(cell_id,6);
v=0;
if p==0 && l==0
    v=0;
elseif p==0 && l~=0
    v=3;
elseif p==1 && l==0
    v=3;
elseif p==1 && l~=0
    v=0;    
 elseif p==2
    v=3*mod(ns,2);   
 elseif p==3
    v=3*3*mod(ns,2);
end

for m=0:2*Nrbdl-1
    mX=m+Nrbdlmax-Nrbdl;
    k=6*m+mod( (v+vshift),6);
    my_slot(k+1,l+1)=r(mX+1); 
end




end