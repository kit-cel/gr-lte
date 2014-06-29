function[c]=lte_pn_generator(cinit,len)
% 1.arg:cinit ; 2.arg:len
% Returns vector with specified length which correspond to the leading elements
% of the scrambling sequence used for several operations during lte
% transmission.




Nc=1600;

x2init=zeros(31,1);
for i=0:30,
    val = mod(cinit,2);
    cinit = floor(cinit/2);
    x2init(i+1) = val;
end

c = zeros(len,1);
x1 = zeros(4*len+Nc,1);
x1(1)=1;
x2 = zeros(4*len+Nc,1);
x2(1:31)=x2init;

for n=1:4*len+Nc
    x1(n+31)=mod( (x1(n+3)+x1(n) ) ,2);
    x2(n+31)=mod( (x2(n+3)+x2(n+2)+x2(n+1)+x2(n)) ,2);
end

for n=1:len
    c(n)=mod( (x1(n+Nc)+x2(n+Nc) ) ,2);
end


end