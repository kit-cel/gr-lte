function[N_id_1,sss_pos]=lte_sss_sync(invec,pss_pos,N_id_2,ni,fftl)
% Decodes SSS symbol and calculates cell ID group

cpl=floor(144*(fftl/2048));
syml=fftl+cpl;
sss_pos=pss_pos-syml;
N_id_1=-1;
%disp(pss_pos);
%disp(N_id_2);
%disp(ni);

% Extract the S-SCH signal according to the estimated
% OFDM symbol timing and apply an N -point FFT

disp(['sss_pos = ' num2str(sss_pos-fftl+1)]);
sss_t=invec(sss_pos:sss_pos+fftl-1);
sss_f=fft(sss_t,fftl);
sss_f=circshift(sss_f,0);

%disp(sss_f);
%stem(abs(sss_f));
sss=[sss_f(fftl-(30+5):end);sss_f(2:(32+5))];
%save('/home/demel/exchange/matlab_sss.mat','sss','-mat');
% Separate length 62 sequence d(n) into sequences d(2n)
% and d(2n + 1), consisting of even and odd sub-carrier symbols
d=[sss_f(fftl-30:end);sss_f(2:32)];
de=d(1:2:length(d)); % even
do=d(2:2:length(d)); % odd

%disp(d);

% In the following we use the notation for sub-frame 0:
% Divide d(2n)/c0(n) in order to obtain the sequence s0m0(n).
% Sequence c0(n) is known at the receiver as
% it depends only on the already estimated sector-ID Ns .1

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
% Generate c0 and c1
c0 = zeros(31,1);
c1 = zeros(31,1);
for n=0:30
    c0(n+1) = cX( mod( (n+N_id_2),31) +1);
    c1(n+1) = cX( mod( (n+N_id_2+3),31) +1);
end
% End generate c0 and c1

s0m0=de./c0;
%disp(['s0m0 = ' num2str(length(s0m0) )]);


% Build a reference sequence sref(n), which is a duplicated
% version s0(m=0)(n) of with the length of 62.
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
sX = zeros(31,1);
for i=1:31
    sX(i) = 1-2* x_s(i);
end
% End generate sX
% Generate s0 and s1
m0=0;
m1=0;
% two different m-sequences according to cell_id_g
s0 = zeros(31,1);
s1 = zeros(31,1);
for n=0:30
    s0(n+1) = sX( mod((n+m0),31)+1 );
    s1(n+1) = sX( mod((n+m1),31)+1 );
end
% End generate s0 and s1
sref=[s0;s0];
%disp(['sref: ' num2str(length(sref))]);

% Apply a cross-correlation between s0m0(n)
% and the reference sequence sref (n).
% The magnitude of the correlation term shows a significant maximum
% between sample 32 and 62,
% which indicates m0 = 31 − indexmax
abs_corr=abs(xcorr(sref,s0m0));
%disp(['length(abs_corr): ' num2str(length(abs_corr))]);

%plot(abs_corr);
%title('cross correlation');

[val,m0e]=max(abs_corr(32:end));
mpos = m0e;
m0e=m0e-31;
m0=m0e;
disp(['m0e: ' num2str(m0e) ' val: ' num2str(val) ' abs_pos = ' num2str(mpos)]);

% After estimating the integer m0, we are able to
% compute z1m0(n) and afterwards divide d(2n+1)/(c1(n) * z1m0(n))
% in order to obtain the sequence s1m1(n).





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
% Generate z0 and z1
z1m0 = zeros(31,1);
%z1m1 = zeros(31,1);
for n=0:30
    z1m0(n+1) = zX( mod( (n+mod(m0,8)) ,31) +1);
% z1m1(n+1) = zX( mod( (n+mod(m1,8)) ,31) +1);%never used!
end
%C = [xz, zX, z0, z1];
% End generate z0 and z1
s1m1=do./(c1 .* z1m0 );


% Apply a cross-correlation between s1m1(n) and sref(n).
% The magnitude of the correlation term shows a significant
% maximum between the 32-th and 62-th sample, which
% indicates m1 = 31 − indexmax , as illustrated in Figure2.

abs_corr=abs(xcorr(sref,s1m1));

[val,m1e]=max(abs_corr(32:95));
m1e=m1e-31;
m1=m1e;
disp(['m1e: ' num2str(m1e) ' val: ' num2str(val)]);

%subplot(2,2,3:4);
%plot(abs_corr);



% The pair of estimated m0 and m1 identifies the group-ID Ng .
% Compute the overall cell-ID: Nc = 3Ng + Ns .
Mat_id_1=zeros(168,3);
for i=0:167
    N=i;
    q_prime = floor(N/30);
    q = floor(( N + (q_prime*(q_prime+1)/2) )/30 );
    m_prime = N + q*(q+1)/2;
    m0m = mod(m_prime,31);
    m1m = mod( (m0m + floor(m_prime/31) +1),31);
    Mat_id_1(i+1,1)=i;
    Mat_id_1(i+1,2)=m0m;
    Mat_id_1(i+1,3)=m1m;
end

sss_pos=0;
if m0 > m1
    ms=m0;
    m0=m1;
    m1=ms;
    %disp('found subframe 5');
    sss_pos=5;
end

for i=0:167
    if Mat_id_1(i+1,2)==m0 && Mat_id_1(i+1,3)==m1
        N_id_1=Mat_id_1(i+1,1);
    end
end

N_id = 3 * N_id_1 + N_id_2;
disp(['N_id: ' num2str(N_id) ' N_id_1: ' num2str(N_id_1) ' N_id_2: ' num2str(N_id_2)]);




if N_id_1 == -1
    disp('lte_sss_sync->ERROR could not sync SSS');
end




