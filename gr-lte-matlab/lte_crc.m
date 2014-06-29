function[]=lte_crc()

disp('Hello'); 

invec=[0 1 1 0 1 0 0 1 1 0 1 1 1 0 0 0 0 0 0 0 0 0 0 0]';
%  Generatorpolynome
% 0x1864CFB 
% binary 1100001100100110011111011
% crca = [1 1 0 0 0 0 1 1 0 0 1 0 0 1 1 0 0 1 1 1 1 1 0 1 1]'; %CRC-24A
% crcb = [1 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 1 0 0 0 0 1]'; %CRC-24B
crc16 = [1 0 0 0 1 0 0 0 0 0 0 1 0 0 0 0 1]'; %CRC16 Polynom for LTE
% crc8 = [1 1 0 0 1 1 0 1 1]'; % CRC Polynom for LTE
% Z = 6144; % Code block length

% a = logical([1 0 1 1 1 0 0 1 0 0 1 1 1 1 0 1 1 0 0 0 0 1 1 0]'); % Testvektor

b = [invec;zeros(16,1)];
p = zeros(17,1);


for i=1:length(b)
    p=circshift(p,-1);
    if p(1)==1
        p=xor(p,crc16);
    end
    p(length(p))=xor(p(length(p)),b(i));
    disp([i p'])
end
mod(p(2:17)'+1,2)
outvec=[invec;p(2:17)];
end


