function[ldr]=lte_read_in_and_resample(num,res_rate)

path='/home/demel/Dokumente/Messungen/';
ldr=0;
if num==1 
    filename='Messung_LTE_2012-05-23_12:47:32_xaa.dat';
elseif num==2
    filename='Messung_LTE_2012-05-23_12:47:32_xab.dat';
elseif num==3
    filename='Messung_LTE_2012-05-23_12:47:32_xac.dat';
elseif num==4    
    filename='Messung_LTE_2012-05-23_12:47:32_xad.dat';
elseif num==5   
    filename='Messung_LTE_2012-05-23_12:49:57_xaa.dat';
elseif num==6
    filename='Messung_LTE_2012-05-23_12:49:57_xab.dat';
elseif num==7
    filename='Messung_LTE_2012-05-23_12:49:57_xac.dat';
elseif num==8
    filename='Messung_LTE_2012-05-23_12:49:57_xad.dat';
elseif num==9
    filename='Messung_LTE_2012-05-23_12:49:57_xae.dat';
elseif num==10
    filename='Messung_LTE_2012-05-23_12:49:57_xaf.dat';
elseif num==11
    filename='Messung_LTE_2012-05-23_12:49:57_xag.dat';
elseif num==12
    filename='Messung_LTE_2012-05-23_12:49:57_xah.dat';
elseif num==13
    filename='Messung_LTE_2012-05-23_12:49:57_xai.dat';
elseif num==14
    filename='Messung_LTE_2012-05-23_12:49:57_xaj.dat';
elseif num==15
    filename='Messung_LTE_2012-05-23_12:49:57_xak.dat';
elseif num==16
    filename='Messung_LTE_2012-05-23_12:49:57_xal.dat';
elseif num==17
    filename='Messung_LTE_2012-05-23_12:49:57_xam.dat';
elseif num==18
    filename='Messung_LTE_2012-05-23_12:49:57_xan.dat';
elseif num==19
    filename='Messung_LTE_2012-05-23_12:49:57_xao.dat';
elseif num==20
    filename='Messung_LTE_2012-05-23_12:49:57_xap.dat';
elseif num==21
    filename='Messung_LTE_2012-05-23_12:49:57_xaq.dat';
elseif num==22
    filename='Messung_LTE_2012-05-23_12:49:57_xar.dat';
elseif num==23
    filename='Messung_LTE_2012-05-23_12:49:57_xas.dat';
elseif num==24
    filename='Messung_LTE_2012-05-23_12:49:57_xat.dat';
elseif num==25
    filename='Messung_LTE_2012-05-23_12:49:57_xau.dat';
elseif num==26
    filename='Messung_LTE_2012-05-23_12:49:57_xav.dat';
elseif num==27
    filename='Messung_LTE_2012-05-23_12:49:57_xaw.dat';
elseif num==28
    filename='Messung_LTE_2012-05-23_12:49:57_xax.dat';
elseif num==29
    filename='Messung_LTE_2012-05-23_12:49:57_xay.dat';
elseif num==30
    filename='Messung_LTE_2012-05-23_12:49:57_xaz.dat';
elseif num==31
    filename='Messung_LTE_2012-05-23_12:49:57_xba.dat';
%be careful! file too large! 133.372.928 samples! resampled = 102.430.408
%elseif num==32
%    filename='Messung_LTE_2012-05-23_12:47:32.dat';
else
    filename='unknown';
end

disp(['filename: ' filename]);
if strcmp(filename,'unknown')
    disp('ERROR unknown file number');
    return;
end

path_filename=[path filename];


samp_rate=1000;
fftl=512;

ldr=lte_resample(path_filename,fftl,samp_rate);
% dec = 4;

% ld=read_complex_binary(path_filename);
% disp(['binary samples = ' num2str(length(ld))]);
% ld=ld(1:ceil(length(ld)/dec));
% 
% disp(['resample rate: ' num2str(res_rate)]);
% 
% if res_rate==3
%     ldr=resample(ld,3072,1000); % FFT size: 2048
% elseif res_rate==1.5
%     ldr=resample(ld,1536,1000); % FFT size: 1024
% elseif res_rate==0.7
%     ldr=resample(ld,768,1000);  % FFT size: 512
% elseif res_rate==0.4
%     ldr=resample(ld,384,1000);  % FFT size: 256
%   elseif res_rate==0.2
%     ldr=resample(ld,192,1000);  % FFT size: 128 
% else
%     disp('ERROR unknown resample rate');
%     ldr=-1;
% end
% 
% disp(['resampled = ' num2str(length(ldr)*dec)]);

end