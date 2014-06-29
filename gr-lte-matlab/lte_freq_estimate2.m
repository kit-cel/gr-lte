function[data]=lte_freq_estimate2(data,frame_start,fftl)


phase_abs = 0;
phase_inc = 0;


cpl=floor(144*fftl/2048);
cpl0=floor(160*fftl/2048);
syml=fftl+cpl;
syml0=fftl+cpl0;
slotl=7*fftl+6*cpl+cpl0;
fs=slotl/0.0005;

f_av_arr=zeros(100,1);

f_av_save=zeros(ceil(length(data)/slotl),1);
count=1;

for i=0:length(data)-1-3*slotl
    if mod(i,slotl)==mod(frame_start,slotl)
        %disp([ 'slot! ' num2str([mod(i,slotl) mod(frame_start,slotl)]) ]);
        c0=corr(data(i+1:i+1+cpl0),data(i+1+fftl:i+1+fftl+cpl0) );
        for j=0:6-1
            pos=i+1+syml0+j*syml;
            c0=c0+corr(data(pos:pos+cpl),data(pos+fftl:pos+fftl+cpl));
        end
        f_off = angle(c0) * fs /(2*pi*fftl);
        f_sum = sum(f_av_arr)/length(f_av_arr);
        f_av_arr(2:end)=f_av_arr(1:end-1);
        f_av_arr(1)=f_sum+f_off;
        f_av = sum(f_av_arr)/length(f_av_arr);
        phase_inc=f_av/fs;
        %disp([f_sum f_av f_off]);
        f_av_save(count)=f_av;
        count=count+1;
        
        
        data(i+slotl+1:i+2*slotl+1) = data(i+slotl+1:i+2*slotl+1).*(exp(-1i*2*pi*phase_inc*(i+slotl+1:i+2*slotl+1) ).');
        %for k=i+slotl:i+2*slotl
        %    data(k+1)=data(k+1)*exp(-1i*2*pi*phase_abs);
        %    phase_abs = phase_abs + phase_inc;
        %end
        
    end
end
test = data;
%save('/home/demel/exchange/matlab_test_freq_estimate2.mat','test','-mat');

plot(f_av_save);
title('lte freq estimate2    f av save');


end









