function[test]=lte_fdd_generate_testdata(N_id,fftl,mib,int_shift,N_ant)
% functions takes cell ID FFT length and integer offset as arguments,
% calculates a number of frames (amount is specified within the function)
% and returns a vector with the generated testdata.



load('/home/demel/exchange/rs_vector.mat');
load('/home/demel/exchange/subfr00_rs.mat');

% Calculate additional values according to input arguments.
N_id_1=floor(N_id/3);
N_id_2=mod(N_id,3);
cpl=144*(fftl/2048);
cpl0=160*(fftl/2048);
syml=fftl+cpl;
syml0=fftl+cpl0;
occupied_tones=1200*fftl/2048;
N_rb_dl=50;
slot_symbs=7*fftl+6*cpl+cpl0;
subfrl=2*slot_symbs;
% Given variables
Ncp=1; % for normal cyclic prefix
% p=0; % specify antenna port

% used sequences and preparations
chu=lte_pss_zc(N_id_2);
pss=chu;
PSS=[zeros(5,1);chu(1:31);chu(32:end);zeros(5,1)];

[sss0,sss5]=sss_symbol(N_id_1,N_id_2);
sss=[sss0,sss5];
SSS_0=[zeros(5,1);sss0(1:31);sss0(32:end);zeros(5,1)];
SSS_5=[zeros(5,1);sss5(1:31);sss5(32:end);zeros(5,1)];

% Generate MIB and calculate complex PBCH symbols.
%mib=mod(randi(1000,24,1),2);
%crcmib=crc16(mib);
pbch=lte_pbch(mib,N_id,N_ant);

disp(size(pbch));



%% Generate Testvector
% Generate testvector
n_frame=10;
n_sub=10;
n_slot=2;
n_symb=7;

disp(['lte_gen->syml: ' num2str(syml) ' cpl: ' num2str(cpl) ' slot_symbs: ' num2str(slot_symbs)]);
% Prepare variables for reference signal mapping
vshift=mod(N_id,6); % cell specific frequency shift.

count=0;
idx=0;
test=zeros(n_frame*n_sub*n_slot*slot_symbs,N_ant);
for i=0:n_frame-1 % for each frame (10ms timing)
    for j=0:n_sub-1 % for each subframe (1ms timing)
        sub_idx=i*n_sub+j;
        disp(['lte_generate_testdata: ' num2str(sub_idx*subfrl)]);
        partl=mod(j,4);
        pbchq=pbch(partl*240+1:partl*240+240,:);
        subframe=lte_build_subframe(N_id,fftl,N_rb_dl,j,pbchq,sss,pss,1);
        test(sub_idx*subfrl+1:(sub_idx+1)*subfrl,1)=subframe;
        if N_ant==2
            subframe=lte_build_subframe(N_id,fftl,N_rb_dl,j,pbchq,sss,pss,2);
            test(sub_idx*subfrl+1:(sub_idx+1)*subfrl,2)=subframe;
        end
        
    end
end






end
