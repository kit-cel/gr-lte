
lte_pss_filter_sync([r_ant1], 2048);


% input1=(0:2:20)+1i*(2:2:22);
% input2=(1:2:22)+1i*(3:2:24);

% save_complex(input1, '/home/maier/Schreibtisch/in1test.dat');
% save_complex(input2, '/home/maier/Schreibtisch/in2test.dat');



 
% N_rb_dl = 50;
% ns = 0;
% % l = 0;
% N_id = 0;
% N_id_1=floor(N_id/3);
% N_id_2=mod(N_id,3)
% Ncp = 1;
% N_ant = 2;
% fftl = 2048;
% p=0;
% 
% mib = lte_mib_pack(N_rb_dl, 'normal', 2, 102); 
% pbch = lte_pbch(mib, 102, N_ant);
% pss=lte_pss_zc(N_id_2);
% sss=sss_symbol(N_id_1,N_id_2);
% 
% test = lte_fdd_generate_testdata(N_id,fftl,mib,0,N_ant);



%fftl = 2048;
% cpl = 144*fftl/2048;
% cpl0 = 160*fftl/2048;
% slotl = 7*fftl+6*cpl+cpl0;
% 
% 
% bits=mod(randi(10,2*fftl,1),2);
% symbol=zeros(fftl, 1);
% for i=1:fftl-1
%     symbol(i)=lte_qpsk_modulator(bits(2*i),bits(2*i+1) );
% end
% 
% f_symbol=fft(symbol,fftl);
% 
% fmean = mean(f_symbol);
% 
% max_clip = 80*abs(fmean);
% papr = max(abs(f_symbol))/abs(fmean)
% 
% for i=1:length(f_symbol)
%     if abs(f_symbol(i)) > max_clip
%         %disp(phase(f_symbol(i)));
%         f_symbol(i) = f_symbol(i)*(max_clip / abs(f_symbol(i)));
%     end
% end
% 
% plot(f_symbol,'bx')
% 
% plot(ifft(f_symbol),'rx')
% 













