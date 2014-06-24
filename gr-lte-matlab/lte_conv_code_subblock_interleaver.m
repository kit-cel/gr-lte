function[v0]=lte_conv_code_subblock_interleaver(in_vec)
% Function performs subblock interleaving according to ETSI 136.212 section 5.1.4.

%disp('lte_conv_code_subblock_interleaver! -> START');
m=length(in_vec);
csub=32;
rsub=ceil(m/csub);
Nd=rsub*csub-m;

y0=zeros(rsub*csub,1);
for i=1:Nd
    y0(i)=NaN;
end
for i=Nd+1:rsub*csub
    y0(i)=in_vec(i-Nd,1);
end
%disp('lte_conv_code_subblock_interleaver! -> NaN inserted');
%y0'
mat0=zeros(rsub,csub);
for i=1:rsub
    for j=1:csub
        mat0(i,j)=y0( csub*(i-1)+j );
    end
end
%disp('lte_conv_code_subblock_interleaver! -> matrix GEN column wise');
%mat0
% disp(mat0);
% Sub-Interleaving Block 1
% 1, 17, 9, 25, 5, 21, 13, 29
mat00=[mat0(:,2),mat0(:,18),mat0(:,10),mat0(:,26),mat0(:,6),mat0(:,22),mat0(:,14),mat0(:,30)];
% 3, 19, 11, 27, 7, 23, 15, 31
mat01=[mat0(:,4),mat0(:,20),mat0(:,12),mat0(:,28),mat0(:,8),mat0(:,24),mat0(:,16),mat0(:,32)];
% 0, 16, 8, 24, 4, 20, 12, 28
mat02=[mat0(:,1),mat0(:,17),mat0(:,9),mat0(:,25),mat0(:,5),mat0(:,21),mat0(:,13),mat0(:,29)];
% 2, 18, 10, 26, 6, 22, 14, 30 
mat03=[mat0(:,3),mat0(:,19),mat0(:,11),mat0(:,27),mat0(:,7),mat0(:,23),mat0(:,15),mat0(:,31)];

mat0=[mat00,mat01,mat02,mat03];
% disp(mat0');
v0=zeros(rsub*csub,1);
for i=1:csub
    for j=1:rsub
        v0((i-1)*rsub+j)=mat0(j,i);
    end
end
%disp('lte_conv_code_subblock_interleaver! -> vector trans');

end