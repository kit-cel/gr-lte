%  Generatorpolynom
% 0x1864CFB 
% binary 1100001100100110011111011
%crcaz = [1 1 0 0 0 0 1 1 0 0 1 0 0 1 1 0 0 1 1 1 1 1 0 1 1]; %CRC-24A
%crcbz = [1 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 1 0 0 0 0 1]; %CRC-24B
%crca = transpose(crcaz);
%crcb = transpose(crcbz);
crc16z = [1 0 0 0 1 0 0 0 0 0 0 1 0 0 0 0 1]'; %CRC16 Polynom for LTE
%crc8z = [1 1 0 0 1 1 0 1 1]; % CRC Polynom for LTE
%Z = 6144; % Code block length
%poly=[1 0 0 0 1 0 0 0 0 0 0 1 0 0 0 0 1];
poly = '0x1021';
Hdec=crc.detector('Polynomial', poly, 'InitialState', '0x0000');
Hgen=crc.generator('Polynomial', poly, 'InitialState', '0x0000');

expo1=zeros(17,1);
for i=1:17
    expo1(i)=2^(17-i);
end
%expo1
%cd1 =sum(expo1.*poly');
%disp(cd1);

expo2=zeros(24,1);
for i=1:24
    expo2(i)=2^(24-i);
end
%                B       9       3       D       8       6
%a = logical([1 0 1 1 1 0 0 1 0 0 1 1 1 1 0 1 1 0 0 0 0 1 1 0]'); % Testvektor
a = ([0 1 1 0 1 0 0 1 1 0 1 1 1 0 0 0 0 0 0 0 0 0 0 0]');
%disp(length(a));
ad= sum(expo2.*a);
disp(ad);
b = [a;zeros(16,1)];
p = [zeros(17,1)];
[N_rb_dl,phich_dur,phich_res,sfn_msb]=lte_mib_unpack(a)


x_ant1=zeros(16,1);
x_ant2=ones(16,1);
x_ant4=[0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1]';

%Mgen = comm.CRCGenerator(crc16z);
%Mdet = comm.CRCDetector(crc16z);
%cw = step(Mgen,a);
%[rx,m] = step(Mdet,cw);
cw = generate(Hgen,a);
c = cw(25:40);
disp(c');
%disp(length(c));
cm=mod(c+x_ant2,2);
disp(cm');

expo=zeros(16,1);
for i=1:16
    expo(i)=2^(16-i);
end
%expo

cd1 = sum(expo.* mod(c+x_ant1,2) );
cd2 = sum(expo.* mod(c+x_ant2,2) );
cd4 = sum(expo.* mod(c+x_ant4,2) );
cd =sum(expo.*cm);

disp(cd1);
disp(cd2);
disp(cd4);

disp(cd);

% for i=1:length(b)
%     p=circshift(p,-1);
%     if p(1)==1
%         p=xor(p,crc16z);
%     end
%     p(length(p))=xor(p(length(p)),b(i));
%     %disp(p(1:10)')
% end
% p'
% p(2:17)-cw(25:40)
% 
% 
% 
% 
% test = [1 1 0 0 1 1 0]';
% rc = [1 1 0 1]';
% gt = comm.CRCGenerator(rc);
% gcw = step(gt,test);
% testb=[test;zeros(3,1)];
% k = zeros(4,1);
% os = zeros(10,1);
% gcw(8:10)';
% 
% for i=1:10
%     k=circshift(k,-1);
%     if k(1)==1
%         k=xor(k,rc);
%     end
%     k(4)=xor(k(4),testb(i));
%     %disp(k')
% end
% k(2:4)';


% test';
% for i=1:4
%     test=circshift(test,-1);
%     %disp(test')
% end





