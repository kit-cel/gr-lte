function[symb]=lte_rs_subc_mapping(N_rb_dl,ns,l,N_id,Ncp,p)
% N_rb_dl actual downlink bandwidth
% ns slot number within radio frame
% l OFDM symbol within slot
% N_id cell ID
% Ncp normal(=1) or extended cyclic prefix(=0)
% p specifies number of antenna port



%p=0; % is the antenna port
Nrbdlmin = 6; % are the smallest
Nrbdlmax = 110; % and largest downlink bandwidth
occupied_tones=N_rb_dl*12; % used subcarriers
N_dl_symb = 7; % ofdm symbols within slot

if Ncp == 0
    N_dl_symb = 6;
end


% no reference signals in various ofdm symbols
if p==0 || p==1
    if l~=0 && l~=N_dl_symb-3
        symb=zeros(N_rb_dl*12,1);
        return
    end
end
if p==2 || p==3
    if l~=1
        symb=zeros(N_rb_dl*12,1);
        return
    end
end




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
    v=3+3*mod(ns,2);
end


symb=zeros(N_rb_dl*12,1);
for m=0:2*N_rb_dl-1
    mX=m+Nrbdlmax-N_rb_dl;
    k=6*m+mod( (v+vshift),6);
    symb(k+1)=r(mX+1);
end










end