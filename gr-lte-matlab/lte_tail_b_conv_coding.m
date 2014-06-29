function[d]=lte_tail_b_conv_coding(ck)
% input is a vector of length 40
% output is a matrix of size 40x3


ck=[ck(end-5:end);ck];

d=zeros(length(ck),3);
reg=zeros(6,1);

for i=1:length(ck)
    

    %disp([ck(i) reg']);

    d(i,1)=mod(ck(i)+reg(2)+reg(3)+reg(5)+reg(6),2);
    d(i,2)=mod(ck(i)+reg(1)+reg(2)+reg(3)+reg(6),2);
    d(i,3)=mod(ck(i)+reg(1)+reg(2)+reg(4)+reg(6),2);   
    
    
    reg=circshift(reg,1);
    reg(1)=ck(i);
    
    
end
d=d(7:end,:);









end