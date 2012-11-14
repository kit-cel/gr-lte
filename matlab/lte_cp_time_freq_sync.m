function[off,coarse_start]=lte_cp_time_freq_sync(invec,fftl)
% Timing synchronization by cyclic prefix correlation.
% invec might be very large, but function does not use the whole vector.



%disp('coarse correlation')

cvec=zeros(ceil(length(invec)/999),1);
pos=0;

cpl=floor(144*fftl/2048);
cpl0=floor(160*fftl/2048);
stp=cpl0/4;
syml=fftl+cpl;
slot_symbs=7*fftl+6*cpl+cpl0;
fs=slot_symbs/0.0005;

disp(['lte_cp_time_freq_sync: ' num2str([fftl cpl0 stp fs])]);

%tic
%for i=0:40:100000
%    absc(i+1)=abs(corr(invec(i+1:i+cpl) , invec(i+1+fftl:i+fftl+cpl)));
%end
%asub=subplot(2,2,1:2);
%title(asub,'rough correlation with MATLAB corr func');
%toc
%plot(absc(10000:20000));
disp('lte_cp_time_freq_sync->BEGIN coarse corr!');
tic
cval=0;
for i=0:stp:length(invec)/1000
    iter=1;
    c=0;
    for h=0:syml:iter*syml
        c=c+lte_partial_corr(invec(i+1:i+cpl) , invec(i+1+fftl:i+fftl+cpl));
    end
    c=c/(iter+1);
    
    cvec(i+1)=abs(corr(invec(i+1:i+cpl) , invec(i+1+fftl:i+fftl+cpl)));
    % disp(['lte_cp_time_freq_sync-->abs(c): ' num2str(corr(invec(i+1:i+cpl) , invec(i+1+fftl:i+fftl+cpl)))]);
    if abs(c) > abs(cval) % && abs(c)>=0.45
        index=i+fftl+cpl;
        disp(['found c max at: ' num2str(i)]);
        f=lte_partial_corr(invec(index+1:index+cpl) , invec(index+1+fftl:index+fftl+cpl));
        if abs(f)>=abs(cval)*0.8 % abs(f)>=0.45
            disp(['found max at: ' num2str(i)]);
            pos=i;
            %disp(c);
            %disp(f);
            cval=c;
        end
    end
end
toc
disp('lte_cp_time_freq_sync->END coarse corr!');
disp(['pos = ' num2str(pos) ' modulo = ' num2str(mod(pos,slot_symbs))]);
%bsub=subplot(2,2,1:2);

plot(cvec);
title('rough correlation with lte partial corr');
clear cvec
if pos < 40
    pos=40;
end

%disp('Begin FINE correlation')

fine_pos=0;
fine_corr_max=0;
cvec=zeros(cpl+90,1);
freq_corr=0; % variable to calculate frequency offset
freq_corr_div=0; % divisor for freq_corr
idx_c=1;

tic
for i=pos-stp:pos+cpl+stp
    %disp(i);
    max_c=0;
    max_f=0;
    iter=10;
    for h=0:syml:iter*syml
        idh=h+i;
        max_c=max_c+lte_partial_corr(invec(idh+1:idh+cpl) , invec(idh+1+fftl:idh+fftl+cpl) );
        a1=angle((corr(invec(idh+1:idh+cpl),invec(idh+1+fftl:idh+fftl+cpl))));
        max_f=max_f+a1*fs/(2*pi*fftl);
    end
    max_c=max_c/(iter+1);
    max_f=max_f/(iter+1);
    
    cvec(idx_c)=max_c;
    idx_c=idx_c+1;
    if max_c > fine_corr_max
        fine_corr_max=max_c;
        fine_pos=i;
        %disp(fine_pos);
        freq_corr=max_f; % (freq_corr+max_f*max_c);
        freq_corr_div=freq_corr_div+max_c;
        %disp(freq_corr);
    end
    
end
%freq_corr=freq_corr/freq_corr_div;
%disp(fine_corr_max);
%disp(fine_pos);
%disp('END CORR LOOP correlation')

%disp('end second loop');
toc


%csub=subplot(2,2,4);
%title(csub,'FINE correlation');
%plot(cvec);


disp(['fine_pos = ' num2str(fine_pos) ' modulo = ' num2str(mod(fine_pos,slot_symbs))]);

tmp = fine_pos;
coarse_start=zeros(1,7);
%while(tmp-slot_symbs > 0)
%    tmp = tmp - slot_symbs;
%end
tmp = mod(tmp,slot_symbs);
for n=0:6
    coarse_start(n+1) = tmp + (n*syml);
end

disp(['coarse = ' num2str(coarse_start(1) )]);
%coarse_start;
off=freq_corr;



















end