
function [x,y]=CXCORR(a,b)


for k=1:length(b)
    y(k)=b*a';  %'-> hermitian
    b=[b(end),b(1:end-1)]; %circular shift
end

len = floor(length(y)/2);
y = circshift(y', [len,0]);
x= (-len:len-1+mod(length(y),2))';
