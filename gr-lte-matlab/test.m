

a1=xcorr(rx_ant(313866+1:end,1),chu_t_high(1:fftl/2, 3),0);
a2=xcorr(rx_ant(313866+1+fftl/2:end,1),chu_t_high(fftl/2+1:fftl, 3),0);
cfo=1/pi*angle(conj(a1)*a2);

a1=xcorr(rx_ant(313866+1:end,2),chu_t_high(1:fftl/2, N_id_2+1),0);
a2=xcorr(rx_ant(313866+1+fftl/2:end,2),chu_t_high(fftl/2+1:fftl, N_id_2+1),0);
cfo=-(cfo+1/pi*angle(conj(a1)*a2))/2;


%disp(['CFO-pss-estimation:' num2str(cfo*15000)]);
freqoffset=cfo*15000
