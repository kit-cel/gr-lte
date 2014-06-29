function[]=lte_rs_check(invec,N_id,pss_pos,fftl,ni)

pos=pss_pos+1;
cpl=floor(144*(fftl/2048));
cpl0=floor(160*(fftl/2048));
%cpldiff=floor(16*(fftl/2048));
syml=fftl+cpl;
syml0=fftl+cpl0;
slot_symbs=7*fftl+6*cpl+cpl0;
subfrl=2*slot_symbs;
Ncp=1;
N_rb_dl=6;

frame_start=pos-slot_symbs;

refvecgen=zeros(fftl,1);
refvecgen=lte_ref_sig_mapping(refvecgen,0,0,N_id,Ncp);
refcheck=[refvecgen(fftl-35:end);refvecgen(1:36)];

subframe_matg=lte_get_subframe(invec,N_id,frame_start,fftl,ni,N_rb_dl);
subframe_mat1=lte_ce_and_correction(subframe_matg,N_id,0,0);
rs00=   lte_rs_subc_mapping(N_rb_dl,0,0,N_id,Ncp,0);
rs04=   lte_rs_subc_mapping(N_rb_dl,0,4,N_id,Ncp,0);
rs07=   lte_rs_subc_mapping(N_rb_dl,1,0,N_id,Ncp,0);
rs011=  lte_rs_subc_mapping(N_rb_dl,1,4,N_id,Ncp,0);
rs10=   lte_rs_subc_mapping(N_rb_dl,2,0,N_id,Ncp,0);

corr00=corr(rs00,subframe_mat1(:,1));
disp(['LTE RS check-->corr: rs00  ' num2str(abs(corr00))]);
corr04=corr(rs04,subframe_mat1(:,5));
disp(['LTE RS check-->corr: rs04  ' num2str(abs(corr04))]);
corr07=corr(rs07,subframe_mat1(:,8));
disp(['LTE RS check-->corr: rs07  ' num2str(abs(corr07))]);
corr011=corr(rs011,subframe_mat1(:,12));
disp(['LTE RS check-->corr: rs011 ' num2str(abs(corr011))]);
corr10=corr(rs10,subframe_mat1(:,15));
disp(['LTE RS check-->corr: rs10  ' num2str(abs(corr10))]);

disp(['lte_rs_check->[pss_pos frame_start]: ' num2str([pss_pos frame_start])]);

% subplot(2,2,1)
scatterplot(subframe_mat1(:,8),1,0,'g+');
title('LTE RS check PBCH1');
hold on;
rsf=real(subframe_matg(:,8))./abs(subframe_matg(:,8));
isf=imag(subframe_matg(:,8))./abs(subframe_matg(:,8));
%disp([rsf isf]);
scatter(rsf,isf,'r*');
grid on;
hold on;

% subplot(2,2,2)
% scatterplot(subframe_mat1(:,9));
% title('LTE RS check PBCH2');
% hold on;
% scatter(real(subframe_matg(:,9)),imag(subframe_matg(:,9)));
% grid on;
% 
% subplot(2,2,3)
% scatterplot(subframe_mat1(:,10));
% title('LTE RS check PBCH3');
% hold on;
% scatter(real(subframe_matg(:,10)),imag(subframe_matg(:,10)));
% grid on;
% 
% subplot(2,2,4)
% scatterplot(subframe_mat1(:,11));
% title('LTE RS check PBCH4');
% hold on;
% scatter(real(subframe_matg(:,11)),imag(subframe_matg(:,11)));
% grid on;


% myplotvec1=subframe_mat1(:,5);
% myplotvec1=myplotvec1/(norm(myplotvec1)/sqrt(length(myplotvec1)));
% subplot(3,2,1:2);
% plot(abs(myplotvec1));
% hold on;
% plot(abs(rs04),'r');
% title('LTE RS check --> RS04 positions');
% hold on;
% 
% myplotvec2=subframe_mat1(:,8);
% myplotvec2=myplotvec2/(norm(myplotvec2)/sqrt(length(myplotvec2)));
% subplot(3,2,3:4);
% plot(abs(myplotvec2));
% hold on;
% plot(abs(rs07),'r');
% title('LTE RS check --> RS07 positions');
% hold on;
% 
% myplotvec3=subframe_mat1(:,12);
% myplotvec3=myplotvec3/(norm(myplotvec3)/sqrt(length(myplotvec3)));
% subplot(3,2,5:6);
% plot(abs(myplotvec3));
% hold on;
% plot(abs(rs011),'r');
% title('LTE RS check --> RS10 positions');



refmat=zeros(72,40);
rxmat=zeros(72,40);
dist_0_4=syml0+3*syml;
refcorr=zeros(40,1);
rs_corr=0;
rs_corr_num=0;


% subplot(2,2,1:2);
% plot(abs(rxmat(:,3)));
% title('LTE RS check OFDM symbol magnitude');
% 
% subplot(2,2,3:4);
% plot(angle(refcorr));
% title('LTE RS check phase correlation');



refmat=[refmat(1:36,:);refmat(38:end,:)]; % remove DC carrier.
rxmat=[rxmat(1:36,:);rxmat(38:end,:)]; % remove DC carrier.

reft=refmat(:,3);
rxt=rxmat(:,3);

% quad_rx=zeros(length(rxt),1);
% phase_quad=zeros(length(rxt),1);
% for i=1:length(quad_rx)
%     quad_rx(i)=(rxt(i))^4;
%     phase_quad(i)=angle(quad_rx(i));
% end
% phase_rx=sum(phase_quad)/length(phase_quad);

%disp('lte_rs_check: fourth power');
% quad_rx;
%plot(abs(fft(quad_rx,fftl)));
%title('LTE RS check fourth power FFT');
%quad_rx(1)=5*exp(1i*phase_rx);
%scatterplot(quad_rx);
%title('LTE RS check fourth power scatter');
%grid on;
%disp('lte_rs_check: fourth power end');

rxcorr=lte_phase_error_correction(reft,rxt);


% scatterplot(rxcorr,1,0, 'g.');
% title('LTE RS check phase shift compensated');
% grid on;
% hold on;
% scatterplot(rxt,1,0, 'b.');
% title('LTE RS check RX with phase shift');
% grid on;


% scatterplot(mlvec,1,0, 'b.');
% title('LTE RS check symbols weighted with magnitude');
% grid on;
%[rxmat(:,3) rxcorr mlvec]

% subcarrier=rxmat(20,:);
% scatterplot(subcarrier,1,0, 'r.');
% title('LTE RS check subcarrier');
% grid on;




end