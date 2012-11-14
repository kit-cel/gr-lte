% PSS generation for LTE
clc
clear
clf

%% PSS
def.sector_id=1;

% Zadoff-Chu root sequence index
sectors = [ 25, 29, 34 ];
chu = exp ( 1j * pi * sectors(def.sector_id) / 63 * (0:63-1).*(1:63) );

% set dc carrier to zero
PSS = [zeros(1,5) chu(1:31) 0 chu(33:end) zeros(1,5)];  

% corr = xcorr(chu); corr = corr(N:end) + [0 corr(1:N-1)];
% plot(abs(corr))

clear chu sectors

%% TX: OFDM
def.subc_spacing = 15e3; %Hz
def.bw = 5e6; %Hz

% lte PHY modes:
lte_mode.bw = [ 1.4e6 3e6 5e6 10e6 15e6 20e6 ];
lte_mode.num_res_blocks = [ 5 15 25 50 75 100 ];
assert( any(lte_mode.bw == def.bw) );

% each resource block uses 12 subcarriers + DC
num_subcs = 12 * lte_mode.num_res_blocks(lte_mode.bw == def.bw) + 1;
len_FFT = 2^ceil(log2( def.bw / def.subc_spacing ));

clear lte_mode

% place PSS in the center
PSS_symbol = [PSS(37:end) zeros(1,len_FFT-length(PSS)) PSS(1:36)].';

% modulate
s = ifft(PSS_symbol);

%% Channel
def.EsN0 = 10;%dB

% normalize tx power
s = s ./ sqrt(s'*s/length(s));
E.s = 1 / ( len_FFT/num_subcs ); % account for oversampling

r = awgn(s, def.EsN0 - 10*log10(E.s)); % SNR = EsN0 - OSR
E.r = r'*r/length(r);

%% RX: Sync
subplot(2,2,1)

[c,lags] = xcorr(r,s);
plot(lags, abs(c) / len_FFT / sqrt(E.s * E.r));
title('t domain')
clear c clags

% assume time sync --> OFDM demod
R = fftshift(fft(r));
E.R = R'*R/length(R);

% generate reference for int CFO
R_ref = PSS(6:end-5).';
E.R_ref = 62/len_FFT; % account for length(R)>length(R_ref)

subplot(2,2,3:4)
[c,lags] = xcorr(R, R_ref);
plot(lags - (len_FFT-62)/2, abs(c) / len_FFT / sqrt( E.R_ref * E.R) );
% shifted lags to have expected peak at 0
title('f domain')
clear c clags
