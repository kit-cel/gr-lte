function[symb]=lte_rs_subc_mapping(N_rb_dl,ns,l,N_id,Ncp,p)
% N_rb_dl actual downlink bandwidth
% ns slot number within radio frame
% l OFDM symbol within slot
% N_id cell ID
% Ncp normal or extended cyclic prefix
% p specifies number of antenna port



%p=0; % is the antenna port
Nrbdlmin = 6; % are the smallest
Nrbdlmax = 110; % and largest downlink bandwidth
occupied_tones=N_rb_dl*12; % used subcarriers




r=lte_ref_sig_gen(ns,l,N_id,Ncp);

% calculate values for reference signal mapping
vshift=mod(N_id,6);
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


symb=zeros(N_rb_dl*12,1);
for m=0:2*N_rb_dl-1
    mX=m+Nrbdlmax-N_rb_dl;
    k=6*m+mod( (v+vshift),6);
    symb(k+1)=r(mX+1);
end










end