close all;

z0=zadoff_chu_lte(0);
z1=zadoff_chu_lte(1);
z2=zadoff_chu_lte(2);

zx=zadoff_chu_lte(1);
zx1=zx;

xc=zeros(125,1);
for i = 0 : length(xc)-1
    zx=circshift(zx,1);
    xc(i+1) = corr(z1,zx);
end

plot(abs(xc),'cx-');
hold on;

zx1=circshift(zx1,-1);
fzx=fft(zx1);
fz1=fft(z1);
fres=conj(fz1).*fzx;

tres=ifft(fres);
res=[tres;tres(1:end-1)];


plot(abs(res)/63,'b*-');
hold on;





