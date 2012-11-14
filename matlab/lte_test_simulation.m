clf;
close all;
%clear all;

% Constants
N_id_1=42;
N_id_2=1;
N_id=N_id_1*3+N_id_2;
Ncp=1; % for normal cyclic prefix
p=0; % specify antenna port
N_ant=2;
Nrbdl=6; % actual system bandwidth
Nrbdlmax = 110; % and largest downlink bandwidth
bw=20e6;
fftl=2^(ceil(log2(bw/15e3)));
cpl=144*(fftl/2048);
cpl0=160*fftl/2048;
syml=fftl+cpl;
syml0=fftl+cpl0;
int_shift = 0;
f_off=3072/2;
n_sub=10;
n_slot=2;
n_symb=7;
nfr_mult=n_sub*n_slot*n_symb;
nsu_mult=n_slot*n_symb;
nsl_mult=n_symb;
slot_symbs=7*fftl+6*cpl+cpl0;
fs=slot_symbs/0.0005;
disp(['fs: ' num2str(fs) ' f_off: ' num2str(f_off) ' fftl: ' num2str(fftl)]);

% Generate MIB, yet a random vector.
mib=lte_mib_pack(50,'normal',1,812);

% Get testvector
tic
test=lte_fdd_generate_testdata(N_id,fftl,mib,int_shift,N_ant);
sim_2ant_127=test;
toc

save('/home/demel/exchange/Matlab_sim_2ant_127.mat','sim_2ant_127','-v7');

% Frequency Offset
phi=f_off/fs;
rm_sim_offset=exp(1i*2*pi*phi*(0:length(test)-1)).';
test = test .* [rm_sim_offset rm_sim_offset];
clear f_off phi;
%ftest=test;



multipath1=test;
multipath2=test;
% White Noise
%tic
test=awgn(test,6,'measured');
multipath1=awgn(multipath1,6,'measured');
multipath2=awgn(multipath2,6,'measured');
%toc
%atest=test;
clear i;

test=test(:,1)+test(:,2); % 2 antennas
multipath1=multipath1(:,1)+multipath1(:,2);
multipath2=multipath2(:,1)+multipath2(:,2);

% Multipath
%multipath=test;
%test=test+1.0*exp(1i*2*pi*0.3)*circshift(multipath1,300);
%test=test+0.3*exp(1i*2*pi*0.7)*circshift(multipath2,700);
test=test+1.0*exp(1i*2*pi*0.3)*circshift(multipath1,40);
test=test+0.3*exp(1i*2*pi*0.7)*circshift(multipath2,80);


sim_2ant_127c=test;
save('/home/demel/exchange/Matlab_sim_2ant_127c.mat','sim_2ant_127c','-v7');

lte_rx(test,fftl);
return;






%% UUT
% Test Functions
tic
disp('lte_cp_time_freq_sync');
%off=0; % quick_off;
%symb=2208; % quick_symb;
[off,symb]=lte_cp_time_freq_sync(test,fftl);
quick_off=off;
quick_symb=symb;
disp(['estimated frequency offset: ' num2str(off) ]);
disp(symb);
toc
phi_corr=off/fs;
rm_offset_vec=exp(-1i*2*pi*phi_corr*(0:length(test)-1)).';
%test = test .* exp(-1i*2*pi*phi_corr*(0:length(test)-1)).';
%disp(angle(ltetest(100:110)-ctest(100:110)));
%disp(mean(ltetest(100:110)-ctest(100:110)));
tic
disp('lte_pss_sync');
pos=symb(1);
[N_id_2_rx,ni,pss_pos]=lte_pss_sync(test,pos,fftl);
disp(['N_id_2: ' num2str(N_id_2_rx) ' ni: ' num2str(ni) ' pss_pos: ' num2str(pss_pos) ]);
if pss_pos < 20000 && pss_pos < length(test)-25*slot_symbs
    pss_pos=pss_pos+10*slot_symbs;
end
toc

tic
disp('lte_sss_sync');
%pss_pos(1)=(nfr_mult+nsu_mult*5+n_symb)*syml
%pss_pos(1)=(11+10)*syml;
%ni=int_shift;
[N_id_1_rx,sss_pos]=lte_sss_sync(test,pss_pos(1),N_id_2_rx,ni,fftl);
disp(['N_id_1: ' num2str(N_id_1_rx) ' sss_pos: ' num2str(sss_pos) ]);
if sss_pos==5
    pss_pos=pss_pos-(n_sub*n_slot*slot_symbs/2);
end
toc
%clear off;
frame_start=pss_pos+1-slot_symbs;
test=lte_freq_estimate(test,frame_start,fftl);

N_id_rx=N_id_1_rx*3+N_id_2_rx; % calculate RX cell ID

disp('lte_rs_check');
lte_rs_check(test,N_id_rx,pss_pos,fftl,ni);


while(frame_start > 0)
    frame_start=frame_start-n_sub*n_slot*slot_symbs;    
end
disp(['lte_test_simulation-->pss_pos 1: ' num2str(frame_start)]);
if frame_start < 0
    frame_start=frame_start+n_sub*n_slot*slot_symbs; 
end
disp(['lte_test_simulation-->pss_pos 2: ' num2str(frame_start)]);
  
disp('lte_pbch_rx');
for i=0:10
    [mib_rx sfn N_ant_rx]=lte_pbch_rx(test,frame_start,ni,N_id_rx,fftl);
    % disp([i sfn]);
    if ~isequal(mib_rx,mib) && N_id_rx > 0
        if frame_start+n_sub*n_slot*slot_symbs < length(test)
            frame_start=frame_start+n_sub*n_slot*slot_symbs;
            % disp('+1 one frame ahead');
        else
            frame_start=frame_start-n_sub*n_slot*slot_symbs;
            % disp('-1 one frame back');
        end
    else
        break;
    end
end
disp(['[SFN number N_ant_rx]: ' num2str([sfn N_ant_rx]) ]);
disp(mib_rx');
disp(['mib_rx isequal mib: ' num2str(isequal(mib_rx,mib) ) ]);
%subplot(4,2,1:2);
%plot(abs(xcorr(mib_rx,pbch)));

disp(['N_id: ' num2str(N_id_rx) ' N_id_1: ' num2str(N_id_1_rx) ' N_id_2: ' num2str(N_id_2_rx) ' ni: ' num2str(ni) ' ffo: ' num2str(off) ]);
disp('END');







