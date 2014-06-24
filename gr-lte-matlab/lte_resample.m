function[ldr]=lte_resample(filename,fftl,samp_rate)
% Read in specified file and resample it. Return!
% Parameters:   string filename
%               int fftl
%               samp_rate (e.g. 10MSps == 1000)

dec = 4;

ld=read_complex_binary(filename);
disp(['binary samples = ' num2str(length(ld))]);
ld=ld(1:ceil(length(ld)/dec));

disp(['fft = ' num2str(fftl) ' samp_rate = ' num2str(samp_rate)]);

if fftl==2048
    ldr=resample(ld,3072,samp_rate); % FFT size: 2048
elseif fftl==1024
    ldr=resample(ld,1536,samp_rate); % FFT size: 1024
elseif fftl==512
    ldr=resample(ld,768,samp_rate);  % FFT size: 512
elseif fftl==256
    ldr=resample(ld,384,samp_rate);  % FFT size: 256
  elseif fftl==128
    ldr=resample(ld,192,samp_rate);  % FFT size: 128 
else
    disp('ERROR unknown resample rate');
    ldr=-1;
end

disp(['resampled = ' num2str(length(ldr)*dec)]);

end