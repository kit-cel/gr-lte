function[out]=lte_build_subframe(N_id,fftl,N_rb_dl,subfr_num,pbch,sss,pss,p)
%pbch only 240 symbol part of one subframe
%p ant port

% Calculated Variables
N_id_1=floor(N_id/3);
N_id_2=mod(N_id,3);
vshift=mod(N_id,6); % cell specific frequency shift.
cpl=144*(fftl/2048);
cpl0=160*(fftl/2048);
syml=fftl+cpl;
syml0=fftl+cpl0;
occ_t=N_rb_dl*12; % short for occupied_tones
slot_symbs=7*fftl+6*cpl+cpl0;

% Given variables
Ncp=1; % for normal cyclic prefix

sfr_mat=zeros(occ_t,14);


% random REs for simulation, prevents zero-symbols
for i=0:14     
    qpsk_vec = zeros(occ_t,1);
    for t=0:occ_t-1
        if(rand<0.8)
            bit1=round(rand);
            bit2=round(rand);
            qpsk_vec(t+1)=lte_qpsk_modulator(bit1,bit2);
        end
    end
    x_vec=lte_layer_mapper(qpsk_vec,2,'tx_diversity');    
    y_vec=lte_pre_coder(x_vec,2,'tx_diversity');
    sfr_mat(:,i+1)=y_vec(:,p+1);
end




% disp(['lte_build_subframe->syml: ' num2str(syml) ' cpl: ' num2str(cpl) ' slot_symbs: ' num2str(slot_symbs)]);

% used sequences and preparations
sss0=sss(:,1);
sss5=sss(:,2);


% Map Synchronization symbols to OFDM symbols
if subfr_num==0 % j==0 && k==0
    sfr_mat(occ_t/2-30:occ_t/2+31,6)=sss0;
    disp('lte_build_subframe-->SSS_0 inserted');
elseif subfr_num==5 % j==5 && k==0 
    sfr_mat(occ_t/2-30:occ_t/2+31,6)=sss5;
    disp('lte_build_subframe-->SSS_5 inserted');
end
if subfr_num==0 || subfr_num==5; % (j==0 || j==5) && k==0 
    sfr_mat(occ_t/2-30:occ_t/2+31,7)=pss;
    disp('lte_build_subframe-->PSS inserted');
end


% Part for mapping PBCH to REs.
if subfr_num==0
    pbch0=pbch(1:48,:);
    pbch1=pbch(48+1:2*48,:);
    s=0;
    for r=0:72-1
        idx=occ_t/2-36+r;
        if mod(N_id,3)~=mod(idx,3)
            sfr_mat(idx+1,8)=pbch0(s+1,p+1);
            sfr_mat(idx+1,9)=pbch1(s+1,p+1);
            s=s+1;
        end
        %disp([r sfr_mat(idx+1,8) sfr_mat(idx+1,9)]);
    end   
    sfr_mat(occ_t/2-35:occ_t/2+36,10)=pbch(2*48+1:2*48+72,p+1);
    sfr_mat(occ_t/2-35:occ_t/2+36,11)=pbch(2*48+72+1:end ,p+1);
    disp('lte_build_subframe-->PBCH inserted');
end


% Evaluation data extracted from OpenLTE
% load('/home/demel/exchange/subfr00_rs.mat');
% load('/home/demel/exchange/subfr04_rs.mat');
% load('/home/demel/exchange/subfr07_rs.mat');
% load('/home/demel/exchange/subfr011_rs.mat');


% Part for Mapping reference signals
ns0=subfr_num*2;
ns1=subfr_num*2+1;
symb0 =lte_rs_subc_mapping(N_rb_dl,ns0,0,N_id,Ncp,p);
symb4 =lte_rs_subc_mapping(N_rb_dl,ns0,4,N_id,Ncp,p);
symb7 =lte_rs_subc_mapping(N_rb_dl,ns1,0,N_id,Ncp,p);
symb11=lte_rs_subc_mapping(N_rb_dl,ns1,4,N_id,Ncp,p);
% disp(['lte_build_frame->RS0: ' num2str(isequal(symb0,modvectest1) )]);
% disp(['lte_build_frame->RS4: ' num2str(isequal(symb4,modvectest2) )]);
% disp(['lte_build_frame->RS7: ' num2str(isequal(symb7,modvectest3) )]);
% disp(['lte_build_frame->RS11: ' num2str(isequal(symb11,modvectest4) )]);

for i=1:occ_t
    if symb0(i)~=0
        sfr_mat(i,1)=symb0(i);
    end
    if symb4(i)~=0
        sfr_mat(i,5)=symb4(i);
    end
    if symb7(i)~=0
        sfr_mat(i,8)=symb7(i);
    end
    if symb11(i)~=0
        sfr_mat(i,12)=symb11(i);
    end
    %disp([sfr_mat(i,8) sfr_mat(i,12)]);
end
% disp('lte_build_subframe-->RSs inserted');
% disp(sfr_mat(occ_t/2-40:occ_t/2,8:10));



out=zeros(2*slot_symbs,1);
outidx=0;
for ns=0:1      %slot
    for l=0:6
        idx=7*ns+l;
        fvec=[0;sfr_mat(occ_t/2+1:occ_t,idx+1);zeros(fftl-1-occ_t,1);sfr_mat(1:occ_t/2,idx+1)];
        tvec=ifft(fvec,fftl); % transformation into time domain
        %disp(['lte_build_subframe ' num2str([idx length(fvec) length(tvec)])]);
        if l==0 % 0th symbol in slot has longer cyclic prefix
            tveccp=lte_cyclic_prefix(tvec,cpl0); % add cyclic prefix
            out(outidx+1:outidx+syml0)=tveccp; % add generated OFDM symbol to testvector
            outidx=outidx+syml0;
        else
            tveccp=lte_cyclic_prefix(tvec,cpl); % add cyclic prefix
            out(outidx+1:outidx+syml)=tveccp; % add generated OFDM symbol to testvector
            outidx=outidx+syml;
        end
    end
end
















end
