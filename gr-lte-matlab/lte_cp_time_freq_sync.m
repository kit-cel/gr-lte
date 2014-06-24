function[off,coarse_start]=lte_cp_time_freq_sync(invec,fftl)
% Timing synchronization by cyclic prefix correlation.
% invec might be very large, but function does not use the whole vector.



%disp('coarse correlation')

cvec=zeros(ceil(length(invec(:,1))/100),2,2);

pos=0;

cpl=floor(144*fftl/2048);
cpl0=floor(160*fftl/2048);
stp=cpl0/4;
syml=fftl+cpl;
slot_symbs=7*fftl+6*cpl+cpl0;
fs=slot_symbs/0.0005;

disp(['lte_cp_time_freq_sync: ' num2str([fftl cpl0 stp fs])]);

disp('lte_cp_time_freq_sync->BEGIN coarse corr!');
tic
cval=0;
corr_v=0;

for i=0:stp:length(invec(:,1))/100-1
    %c=c+lte_partial_corr(invec(i+1:i+cpl) , invec(i+1+fftl:i+fftl+cpl));
    
    vec1=invec(i+1:i+cpl, :);
    vec2=invec(i+1+fftl:i+fftl+cpl, :);
    corr_v = corr(vec1, vec2);
    cvec(i+1,:,:)=abs(corr_v);
    %disp(['lte_cp_time_freq_sync-->abs(c): ' num2str(corr(invec(i+1:i+cpl) , invec(i+1+fftl:i+fftl+cpl)))]);
    
    if abs(corr_v(1,1)+corr_v(2,2)) > abs(cval) %abs(corr_v(1,1))+abs(corr_v(2,2)) > abs(cval)
        disp(['found max at: ' num2str(i)]);
        pos=i;
        cval=corr_v(1,1)+corr_v(2,2);
    end
    
end
toc
disp('lte_cp_time_freq_sync->END coarse corr!');
disp(['pos1 = ' num2str(pos)]);


off=0;
coarse_start=pos;

subplot(2,2,1);
plot(cvec(:,1,1)+cvec(:,2,2));
title('rough correlation with autocorr ant1+ant2');


clear cvec
if pos < 40
    pos=40;
end


%disp('Begin FINE correlation')

fine_pos=0;
fine_corr_max=0;
cvec=zeros(cpl+90,1);
freq_corr=0; % variable to calculate frequency offset
idx_c=1;

tic

for i=pos-stp:pos+cpl+stp
    max_c=0;
    max_f=0;
    iter=10;
    for h=0:syml:iter*syml
        idh=h+i;
        max_c=max_c+corr(invec(idh+1:idh+cpl,1), invec(idh+1+fftl:idh+fftl+cpl,1) );        
        max_c=max_c+corr(invec(idh+1:idh+cpl,2), invec(idh+1+fftl:idh+fftl+cpl,2) );
        
        a1=   angle((corr(invec(idh+1:idh+cpl,1), invec(idh+1+fftl:idh+fftl+cpl,1))));
        a1=a1+angle((corr(invec(idh+1:idh+cpl,2), invec(idh+1+fftl:idh+fftl+cpl,2))));
        max_f=max_f+a1*fs/(2*pi*fftl);
    end
    max_c=max_c/(iter+1)/2;
    max_f=max_f/(iter+1)/2;
    
    cvec(idx_c,1)=max_c;
    idx_c=idx_c+1;
    if max_c > fine_corr_max
        fine_corr_max=max_c;
        fine_pos=i;
        freq_corr=max_f; 
    end
    
end


toc


disp(['fine_pos = ' num2str(fine_pos) ' modulo = ' num2str(mod(fine_pos,slot_symbs))]);

%coarse_start;
off=freq_corr;

















end