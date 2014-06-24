function[sss_0,  sss_5] = sss_symbol(N_id_1,N_id_2)
% 1.arg:cell_id_g ; 2.arg:cell_id ; 3.arg:subframe
% arguments needed:
% N_id_2 [0, 1, 2]
% N_id_1 [0, ... , 167]

% different subframes need different sequences
% possible values are 0 and 5


% Generate m0 and m1
q_prime = floor(N_id_1/30);
q = floor(( N_id_1 + (q_prime*(q_prime+1)/2) )/30 );
m_prime = N_id_1 + q*(q+1)/2;
m0 = mod(m_prime,31);
m1 = mod( (m0 + floor(m_prime/31) +1),31);
disp(['m0: ' num2str(m0)]);
disp(['m1: ' num2str(m1)]);  

% Generate sX (or s_tilda)
x_s = zeros(31,1);
% initial conditions:
% x(1) = 0
% x(2) = 0
% x(3) = 0
% x(4) = 0
% x(5) = 1
x_s(5) = 1;
for i=1:26
    x_s(i+5) = mod((x_s(i+2) + x_s(i) ) , 2);
end
disp(length(x_s))
sX = zeros(31,1);
for i=1:31
    sX(i) = 1-2* x_s(i);
end
% End generate sX



% Generate cX (c_tilda)
x_c = zeros(31,1);
cX = zeros(31,1);

% x_c(1) = 0;
% x_c(2) = 0;
% x_c(3) = 0;
% x_c(4) = 0;
x_c(5) = 1;

for i=1:26
    x_c(i+5) = mod((x_c(i+3) + x_c(i) ) , 2);
end
for i=1:31
    cX(i) = 1-2*x_c(i);
end
% End generate cX



% Generate zX
x_z = zeros(31,1);
zX = zeros(31,1);
% xz(1) = 0;
% xz(2) = 0;
% xz(3) = 0;
% xz(4) = 0;
x_z(5) = 1;

for i=1:26
    x_z(i+5) = mod((x_z(i+4) + x_z(i+2) + x_z(i+1) + x_z(i) ) , 2);
end
for i=1:31
    zX(i) = 1-2*x_z(i);
end
% End generate zX


% Generate s0 and s1
% two different m-sequences according to cell_id_g
s0 = zeros(31,1);
s1 = zeros(31,1);
for n=0:30
    s0(n+1) = sX( mod((n+m0),31)+1 );
    s1(n+1) = sX( mod((n+m1),31)+1 );
end
% End generate s0 and s1


% Generate c0 and c1
c0 = zeros(31,1);
c1 = zeros(31,1);
for n=0:30
    c0(n+1) = cX( mod( (n+N_id_2),31) +1);
    c1(n+1) = cX( mod( (n+N_id_2+3),31) +1);
end
%B = [xc, cX, c0, c1];
% End generate c0 and c1

% Generate z0 and z1
z1m0 = zeros(31,1);
z1m1 = zeros(31,1);
for n=0:30
    z1m0(n+1) = zX( mod( (n+mod(m0,8)) ,31) +1);
    z1m1(n+1) = zX( mod( (n+mod(m1,8)) ,31) +1);
end
%C = [xz, zX, z0, z1];
% End generate z0 and z1

% Generate SSS_0 and SSS_5
sss_0=zeros(62,1);
sss_5=zeros(62,1);
for n=0:30
    sss_0((2*n)+1)=s0(n+1)*c0(n+1);
    sss_0((2*n)+2)=s1(n+1)*c1(n+1)*z1m0(n+1);
    
    sss_5((2*n)+1)=s1(n+1)*c0(n+1);
    sss_5((2*n)+2)=s0(n+1)*c1(n+1)*z1m1(n+1);
end
% End generate sss_0 and sss_5


end