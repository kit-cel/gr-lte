fftl = 2048;
cpl = 144*fftl/2048;
cpl0 = 160*fftl/2048;
slotl = 7*fftl+6*cpl+cpl0;


bits=mod(randi(10,2*fftl,1),2);
symbol=zeros(fftl, 1);
for i=1:fftl-1
    symbol(i)=lte_qpsk_modulator(bits(2*i),bits(2*i+1) );
end

f_symbol=fft(symbol,fftl);

fmean = mean(f_symbol);

max_clip = 80*abs(fmean);
papr = max(abs(f_symbol))/abs(fmean)

for i=1:length(f_symbol)
    if abs(f_symbol(i)) > max_clip
        %disp(phase(f_symbol(i)));
        f_symbol(i) = f_symbol(i)*(max_clip / abs(f_symbol(i)));
    end
end

plot(f_symbol,'bx')

plot(ifft(f_symbol),'rx')














