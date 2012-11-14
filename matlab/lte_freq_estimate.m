function[invec]=lte_freq_estimate(invec,frame_start,fftl)


cpl=floor(144*fftl/2048);
cpl0=floor(160*fftl/2048);
syml=fftl+cpl;
syml0=fftl+cpl0;
slot_symbs=7*fftl+6*cpl+cpl0;
fs=slot_symbs/0.0005;

max_av_mag=0;
for test=1:10
    
    phase=zeros(7,1);
    freq=zeros(7,1);
    mag=zeros(7,1);

    
    
    for i=0:6
        if i==0
            idx=frame_start;
            idx1=frame_start+slot_symbs;
            idx2=frame_start+2*slot_symbs;
            p1=(corr(invec(idx+1:idx+cpl0),invec(idx+1+fftl:idx+fftl+cpl0)));
            p2=(corr(invec(idx1+1:idx1+cpl0),invec(idx1+1+fftl:idx1+fftl+cpl0)));
            p3=(corr(invec(idx2+1:idx2+cpl0),invec(idx2+1+fftl:idx2+fftl+cpl0)));
            phase(i+1)=(angle(p1)+angle(p2)+angle(p3))/3;
            freq(i+1) =phase(i+1)*fs/(2*pi*fftl);
            mag(i+1)  =(abs(p1)+abs(p2)+abs(p3))/3;
        else
            idx=frame_start+syml0+(i-1)*syml;
            idx1=frame_start+slot_symbs;
            idx2=frame_start+2*slot_symbs;
            p1=(corr(invec(idx+1:idx+cpl),invec(idx+1+fftl:idx+fftl+cpl)));
            p2=(corr(invec(idx1+1:idx1+cpl),invec(idx1+1+fftl:idx1+fftl+cpl)));
            p3=(corr(invec(idx2+1:idx2+cpl),invec(idx2+1+fftl:idx2+fftl+cpl)));
            
            phase(i+1)=(angle(p1)+angle(p2)+angle(p3))/3;
            freq(i+1) =phase(i+1)*fs/(2*pi*fftl);
            mag(i+1)  =(abs(p1)+abs(p2)+abs(p3))/3;
        end
    end
    
    
    av_mag =sum(mag) /length(mag);
    [~,mypos]=max(mag);
    av_freq=freq(mypos);
    if av_mag >= max_av_mag
        
        %av_freq=sum(freq)/length(freq);


        phi_corr=av_freq/fs;
        rm_offset_vec=exp(-1i*2*pi*phi_corr*(0:length(invec)-1)).';
        %disp(rm_offset_vec(1:20));
        invec = invec .* rm_offset_vec;
        max_av_mag=av_mag;
        disp('lte_freq_estimate--> av_mag >= max_av_mag');
    end
    
    
    disp(['lte_freq_estimate-->[it freq mag] ' num2str([test av_freq av_mag])]);
    %disp([mag]);
    %disp('lte_freq_estimate-->freq: ');
    %disp(freq);
    frame_start=frame_start+slot_symbs;


end


















end