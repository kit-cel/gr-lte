function [off] = lte_cp_freq_sync(invec, pss_pos ,fftl)
% CP based frequency offset estimation with PSS estimated position
% 

cpl=floor(144*fftl/2048);
cpl0=floor(160*fftl/2048);
syml=fftl+cpl;
syml0=fftl+cpl0;
slot_symbs=7*fftl+6*cpl+cpl0;
fs=slot_symbs/0.0005;

off=zeros(2,1);

for i=1:2
    
    idh=pss_pos-cpl;
    a1=0;
    
    %averaging over 100 OFDM symbols
    for h=0:99
        vec1=invec(idh:idh+cpl,i);
        vec2=invec(idh+fftl:idh+cpl+fftl,i);
        a1=a1+angle(xcorr(vec1,vec2,0)); 
        disp(num2str(angle(xcorr(vec1,vec2,0))));
        if mod(h,7)==0
            idh=idh+syml0;
        else            
            idh=idh+syml;
        end
        
    end
    
    off(i)=a1/100*fs/(2*pi*fftl);
    
end

disp(['CP based freq estimate after PSS: ' num2str(off(1)) '  ' num2str(off(2))]);

off=(off(1)+off(2))/2;


end

