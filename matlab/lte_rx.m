function[]=lte_rx(test,fftl)
% 10MHz bandwidth -> maximum of 50 RBs occupied -> 9MHz used bandwidth
% sampling rate = 15.36MSps
% cyclic prefix time is 4.69us
% only PBCH shall be decoded by now therefore only 72 RBs are demodulated.

disp('live data RX tests');
close all;

save('/home/demel/exchange/matlab_test_origin.mat','test','-mat');

%bw=10e6;
%fftl=512;%2^(ceil(log2(bw/15e3)));


cpl=144*(fftl/2048);
cpl0=160*fftl/2048;
syml=fftl+cpl;
syml0=fftl+cpl0;
n_sub=10;
n_slot=2;
slot_symbs=7*fftl+6*cpl+cpl0;
fs=slot_symbs/0.0005;

disp('lte_cp_time_freq_sync');
[off,symb]=lte_cp_time_freq_sync(test,fftl);
disp(['estimated frequency offset: ' num2str(off) ]);
disp(symb);
%s_symb=symb;
%off=-12.065;
phi_corr=off/fs;
rm_offset_vec=exp(-1i*2*pi*phi_corr*(0:length(test)-1)).';
test = test .* rm_offset_vec;


% save data after CP-based time/freq sync.
save('/home/demel/exchange/matlab_test_first_freq.mat','test','-mat');


disp('lte_pss_sync');
pos=symb(1);

[N_id_2_rx,ni,pss_pos]=lte_pss_sync(test,pos,fftl);
disp(['N_id_2: ' num2str(N_id_2_rx) ' ni: ' num2str(ni) ' pss_pos: ' num2str(pss_pos) ]);
disp(['PSS sync: pss_pos = ' num2str(pss_pos) ' input pos = ' num2str(pos)]);

%return;

pss_pos_save = pss_pos;
disp('lte_sss_sync');
%for i=0:100
[N_id_1_rx,sss_pos]=lte_sss_sync(test,pss_pos(1),N_id_2_rx,ni,fftl);
disp(['N_id_1: ' num2str(N_id_1_rx) ' sss_pos: ' num2str(sss_pos) ]);
pss_pos(1)=pss_pos(1)+10*slot_symbs;
%end

pss_pos=pss_pos_save;
if sss_pos==5
    pss_pos=pss_pos-(n_sub*slot_symbs);
end


%return;


frame_start=pss_pos+1-slot_symbs;
while(frame_start > 0)
    frame_start=frame_start-20*slot_symbs;    
end
disp(frame_start);
if frame_start < 0
    frame_start=frame_start+n_sub*n_slot*slot_symbs; 
end
disp(['assumed frame start: ' num2str(frame_start)]);    

N_id_rx=N_id_1_rx*3+N_id_2_rx;
if N_id_rx < 0
    disp('Could not calculate cell ID');
    disp(' ');
    disp(' ');
    disp(' ');
    disp(' ');
    disp(['Nid2 = ' num2str(N_id_2_rx) ' ni = ' num2str(ni)]);
    
    return;
end

test2=test;
test=lte_freq_estimate(test,frame_start,fftl);
save('/home/demel/exchange/matlab_test_freq_estimate1.mat','test','-mat');
% estimate frequency after completed sync process. for better results.
%test=lte_freq_estimate2(test2,frame_start,fftl);
%save('/home/demel/exchange/matlab_test_freq_estimate2.mat','test','-mat');


disp('lte_rs_check');
while pss_pos < 0
    pss_pos=pss_pos+20*slot_symbs;
end

disp('lte_pbch_rx');
mycount=0;        
mib_succ=ones(24,1)*-1;
sfn_succ=-1;
N_ant_succ=-1;

%frame_start = s_frame_start;
%ni = s_ni;
%N_id_rx = s_N_id_rx;
%test = s_test;
disp(['frame_start = ' num2str(frame_start)]);

disp(' ');
disp(' ');
disp(' ');
disp(' ');
disp(['Nid2 = ' num2str(N_id_2_rx) ' ni = ' num2str(ni)]);
return;

for i=0:length(test)/(n_sub*n_slot*slot_symbs)-1
    [mib_rx sfn N_ant]=lte_pbch_rx(test,frame_start,ni,N_id_rx,fftl);
    disp([i sfn]);
    if N_ant==-1 && N_id_rx > 0
        if frame_start+n_sub*n_slot*slot_symbs < length(test)
            frame_start=frame_start+n_sub*n_slot*slot_symbs;
            disp(['+1 one frame ahead ' num2str(frame_start) ' NOT FOUND!']);
        else
            break;
        end
    else
        if frame_start+n_sub*n_slot*slot_symbs < length(test)
            frame_start=frame_start+n_sub*n_slot*slot_symbs;
            disp(['+1 one frame ahead ' num2str(frame_start) ' SUCCESS']);
        else
            break;
        end
        
        sfn_iq=isequal(sfn_succ,sfn);
        mib_iq=isequal(mib_rx,mib_succ);
        N_ant_iq=isequal(N_ant_succ,N_ant);
        
        mib_succ=mib_rx;
        sfn_succ=sfn;
        N_ant_succ=N_ant;
        mycount=mycount+1;
        %disp(['found MIB ' num2str([mycount sfn_iq mib_iq N_ant_iq])]);
        break;
    end
end

% If a MIB could be decoded, display result
if mycount > 0
    %disp(['found MIB mycount: ' num2str(mycount)]);
    
    
    sfn_vec=[mib_succ(7:14)' 0 0];
    binary_vec=[2^9 2^8 2^7 2^6 2^5 2^4 2^3 2^2 2^1 2^0];
    disp(binary_vec);
    disp(sfn_vec)

    [N_rb_dl,phich_dur,phich_res,sfn_msb]=lte_mib_unpack(mib_succ);
    disp(['MIB! N_rb_dl: ' num2str(N_rb_dl) ', SFN: ' num2str(sfn_succ) ', N_ant: ' num2str(N_ant_succ) ', phich_dur: ' phich_dur ', phich_res: ' num2str(phich_res)]);

else
    disp(['[SFN N_ant]: ' num2str([sfn N_ant])]);
    disp(mib_rx');
end

disp(['N_id: ' num2str(N_id_rx) ' N_id_1: ' num2str(N_id_1_rx) ' N_id_2: ' num2str(N_id_2_rx) ' ni: ' num2str(ni) ' ffo: ' num2str(off) ]);
disp('END');







end