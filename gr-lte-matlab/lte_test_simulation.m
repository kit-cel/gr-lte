 

clf; 
close all;
clear all;
% 
rmc.RC='R.11';
rmc.NCellID=113;
rmc.PDSCH.TxScheme='TxDiversity';
rmc.CyclicPrefix='Normal';
rmc.DuplexMode='FDD';
rmc.TotSubframes=10*50;
rmc.NFrame=650;
waveconfig=lteRMCDL(rmc);

[waveform, grid, waveconfig]=lteRMCDLTool(waveconfig,[1;0;0;1]);

% chcfg.Seed=3;
% chcfg.DelayProfile='ETU';
% chcfg.NRxAnts=2;
% chcfg.DopplerFreq=0;
% chcfg.MIMOCorrelation='medium';
% chcfg.SamplingRate=waveconfig.SamplingRate;
% chcfg.InitTime = 10;
% [rx_ant, chinfo]=lteFadingChannel(chcfg,waveform);


rx_ant(:,1)= waveform(:,1)+waveform(:,2);
rx_ant(:,2)= waveform(:,1)+waveform(:,2);

%rx_ant(:,1)=rx_ant(:,1)+0.6*exp(1i*0.3)*circshift(rx_ant(:,1), 20)+0.2*exp(1i*0.3)*circshift(rx_ant(:,1), 60);
%rx_ant(:,2)=rx_ant(:,2)+0.4*exp(1i*0.33)*circshift(rx_ant(:,2), 30)+0.21*exp(1i*0.5)*circshift(rx_ant(:,2), 55);


% f_off=2000;
% phi=f_off/waveconfig.SamplingRate;
% rm_sim_offset=exp(1i*2*pi*phi*(0:length(rx_ant(:,1))-1))';
% rx_ant = rx_ant .* [rm_sim_offset rm_sim_offset];
% clear f_off phi;

rx_ant(:,1)=awgn(rx_ant(:,1),30, 'measured');
rx_ant(:,2)=awgn(rx_ant(:,2),30, 'measured');

test=rx_ant;
samplerate=waveconfig.SamplingRate;
fftl=single(waveconfig.Nfft);
% test=invec;
% fftl=1024;
% samplerate=15360000;


%save_complex(rx_ant, 2, '/home/maier/Schreibtisch/lte50framesFadingChannelETU_fOff2000.dat');
return;
%  lte_rx(test, fftl);

cpl=144*(fftl/2048);
cpl0=160*fftl/2048;
slot_symbs=7*fftl+6*cpl+cpl0;
fs=slot_symbs/0.0005;


% lte_rx(test, fftl);
[psspos, N_id_2, cfo_pss]=lte_pss_filter_sync(test,fftl);
halfframestart=psspos-6*fftl-6*cpl-cpl0;

cffo_cp=lte_cp_freq_sync(test,psspos,fftl);

phi=cfo_pss/samplerate;
% phi=cffo_cp/waveconfig.SamplingRate;
rm_sim_offset=exp(-1i*2*pi*phi*(0:length(test(:,1))-1))';
test = test .* [rm_sim_offset rm_sim_offset];

[N_id_1,sss_pos]=lte_sss_sync(test,psspos,N_id_2,0,fftl);

if sss_pos==0
   framestart=halfframestart; 
else
   framestart=halfframestart+10*slot_symbs;  
end

framestart=mod(framestart, 20*slot_symbs);
framestart=1;
N_id_rx=N_id_1*3+N_id_2;

%  [mib_rx sfn N_ant]=lte_pbch_rx(test,framestart,0,N_id_rx,fftl);
% disp([sfn]);
    
    
return;







