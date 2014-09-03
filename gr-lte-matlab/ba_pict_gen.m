clear all;
close all;


fontsize = 17;


% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% %PSS corr pictures freq domain
% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Nzc=62;
u=0;
cell_id=1;

labelyaxis='Betrag d. Autokorrelation';
labelxaxis='n';

if cell_id==0
    u=25;
end
if cell_id==1
    u=29;
end
if cell_id==2
    u=34;
end

% zyklische AKF, sequenz mit dc carrier,
for n=0:62
    a(n+1)=exp(complex(0,-1)*pi*u*n*(n+1)/63);
end  
[l1,l2]=(cxcorr(a,a));
figure();
plot(l1,abs(l2/63));
axis([-35, 35, -0.05,1.1]);
xlabel(labelxaxis,'FontSize', fontsize);
ylabel(labelyaxis,'FontSize',fontsize);
set(gcf,'renderer','painters')
saveas(gcf,'images\zadoff_chu_cyclic_akf_with_dc','epsc');


% AKF, sequenz mit dc carrier
figure()
plot(-62:62,abs(xcorr(a)/63));
axis([-70, 70, 0,1.1]);
xlabel(labelxaxis,'FontSize',fontsize);
ylabel(labelyaxis,'FontSize',fontsize);
saveas(gcf,'images\zadoff_chu_normal_akf_with_dc','epsc');

% zyklische AKF, sequenz ohne dc carrier,
a = lte_pss_zc(cell_id)';
[l1,l2]=(cxcorr(a,a));
figure();
plot(l1,abs(l2/62));
axis([-35, 35, 0,1.1]);
xlabel(labelxaxis,'FontSize',fontsize);
ylabel(labelyaxis,'FontSize',fontsize);
saveas(gcf,'images\zadoff_chu_cyclic_akf_without_dc','epsc');

% AKF, sequenz ohne dc carrier
figure()
plot(-61:61,abs(xcorr(a)/62));
axis([-70, 70, 0,1.1]);
xlabel(labelxaxis,'FontSize',fontsize);
ylabel(labelyaxis,'FontSize',fontsize);
saveas(gcf,'images\zadoff_chu_normal_akf_without_dc','epsc');


% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% %PSS corr pictures time domain
% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


fftl=1024;
labelyaxis='Betrag d. Autokorrelation';
labelxaxis='n';
powercof=1.5;

% PSS to time domain
%id=0
chu_seq=lte_pss_zc(0); %a';%  
chu=[chu_seq(1:31);0;chu_seq(32:end)];     
pss=[zeros(5,1);chu(1:end);zeros(5,1)];
f_vec_fftl=[pss(37:end);zeros(fftl-length(pss),1);pss(1:36)];
chu_t_high0=ifft(f_vec_fftl, fftl)*fftl;
figure()
correl=power(abs(xcorr(chu_t_high0','coeff')),powercof);
plot(-fftl+1:fftl-1,correl);
axis([-1200, 1200, 0,1.1]);
xlabel(labelxaxis,'FontSize',fontsize);
ylabel(labelyaxis,'FontSize',fontsize);
saveas(gcf,'images\pss_time_domain_akf_Nid0','epsc');
%id=1
chu_seq=lte_pss_zc(1); %a';%  
chu=[chu_seq(1:31);0;chu_seq(32:end)];     
pss=[zeros(5,1);chu(1:end);zeros(5,1)];
f_vec_fftl=[pss(37:end);zeros(fftl-length(pss),1);pss(1:36)];
chu_t_high1=ifft(f_vec_fftl, fftl)*fftl;
figure()
correl=power(abs(xcorr(chu_t_high1','coeff')),powercof);
plot(-fftl+1:fftl-1,correl);
axis([-1200, 1200, 0,1.1]);
xlabel(labelxaxis,'FontSize',fontsize);
ylabel(labelyaxis,'FontSize',fontsize);
saveas(gcf,'images\pss_time_domain_akf_Nid1' ,'epsc');
%id=2
chu_seq=lte_pss_zc(2); %a';%  
chu=[chu_seq(1:31);0;chu_seq(32:end)];     
pss=[zeros(5,1);chu(1:end);zeros(5,1)];
f_vec_fftl=[pss(37:end);zeros(fftl-length(pss),1);pss(1:36)];
chu_t_high2=ifft(f_vec_fftl, fftl)*fftl;
figure()
correl=power(abs(xcorr(chu_t_high2','coeff')),powercof);
plot(-fftl+1:fftl-1,correl);
axis([-1200, 1200, 0,1.1]);
xlabel(labelxaxis,'FontSize',fontsize);
ylabel(labelyaxis,'FontSize',fontsize);
saveas(gcf,'images\pss_time_domain_akf_Nid2' ,'epsc');

% [x, correl1] = cxcorr(chu_t_high1',chu_t_high0');
% [x, correl2] = cxcorr(chu_t_high1',chu_t_high1');
% plot(x,abs(correl1),x,abs(correl2));

% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%






