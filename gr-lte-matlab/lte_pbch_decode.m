function[mib sfn N_ant]=lte_pbch_decode(pbch_frame1,pbch_ce1,pbch_frame2,pbch_ce2,N_id,N_ant_as)
% function demodulates QPSK, descrambles and calls lte_bch_decode

style='tx_diversity';
disp(['lte_pbch_decode: ' num2str(size(pbch_frame1)) ', assumed antenna ports: ' num2str(N_ant_as)]);


% pbch_frame1=pbch_frame(:,1,1);
% save('/home/demel/exchange/matlab_pbch_frame.mat','pbch_frame1','-mat');
% pbch_ce1=pbch_ce(:,1);
% save('/home/demel/exchange/matlab_pbch_ce1.mat','pbch_ce1','-mat');
% %disp([num2str(N_ant_as) '   size(pbch_ce) = ' num2str(size(pbch_ce)) '   size(pbch_ce1) = ' num2str(size(pbch_ce1))]);
% pbch_ce1=pbch_ce(:,2);
% save('/home/demel/exchange/matlab_pbch_ce2.mat','pbch_ce1','-mat');
%disp([num2str(N_ant_as) '   size(pbch_ce) = ' num2str(size(pbch_ce)) '   size(pbch_ce1) = ' num2str(size(pbch_ce1))]);



%pbch_layer_demap=lte_pre_decoder_old(pbch_frame1,pbch_ce1, 2,style);


pbch_layer_demap=lte_pre_decoder(pbch_frame1,pbch_ce1,pbch_frame2,pbch_ce2, N_ant_as,style);



% scatterplot(pbch_layer_demap(:,1),1,0,'b+');
% hold on;
% %scatter(real(pbch_frame_pre_dec(:,2)),imag(pbch_frame_pre_dec(:,2)),'ro');
% plot_name=['LTE PBCH decode with ' num2str(N_ant_as) ' antenna ports'];
% title(plot_name);
% grid on;

%plot(abs(pbch_frame_pre_dec));
% if N_ant_as == 1
%     pbch_layer_demap1=pbch_layer_demap(:,1,1);
%     save('/home/demel/exchange/matlab_pbch_layer_demap1.mat','pbch_layer_demap1','-mat');
% end
% if N_ant_as == 2
%     % pbch_layer_demap1=[pbch_layer_demap(:,1,1);pbch_layer_demap(:,2,1)];
%     pbch_layer_demap1=pbch_layer_demap;
%     save('/home/demel/exchange/matlab_pbch_layer_demap2.mat','pbch_layer_demap1','-mat');
% end

%disp(size(pbch_frame_pre_dec));
pbch_frame_demap=lte_layer_demapper(pbch_layer_demap,1,style);

%disp(size(pbch_frame_demap));

plot(pbch_frame_demap,'*');
title(['received QPSK constellation with ' num2str(N_ant_as) ' assumed antenna ports']);
xlabel('Inphase');
ylabel('Quadrature');

if N_ant_as == 1
%     save('/home/demel/exchange/matlab_pbch_frame_demap1.mat','pbch_frame_demap','-mat');
end
if N_ant_as == 2
%     save('/home/demel/exchange/matlab_pbch_frame_demap2.mat','pbch_frame_demap','-mat');
end
% save('/home/demel/exchange/matlab_pbch_frame_demap.mat','pbch_frame_demap','-mat');
pbch_demod=zeros(length(pbch_frame_demap)*2,1);
for i=0:length(pbch_frame_demap)-1
    bits=lte_qpsk_demodulator(pbch_frame_demap(i+1));
    pbch_demod(i*2+1:i*2+2)=bits;
end

% Save successfully demodulated string
% save('/home/demel/exchange/matlab_pbch_demod_p.txt','pbch_demod','-ascii');
% Get cell-specific scrambling sequence and descramble for all possible
% PBCH positions
len=1920;
% descrambling
c_seq=1-2*lte_pn_generator(N_id,len);
pbch_descr=[pbch_demod;pbch_demod;pbch_demod;pbch_demod];
%disp(size(pbch_descr));
% size(pbch_descr)
% size(c_seq)
pbch_descr=c_seq.*pbch_descr;

% save('/home/demel/exchange/matlab_pbch_descr_p.txt','pbch_descr','-ascii');
% save('/home/demel/exchange/Matlab_ld_pbch_descr.mat','pbch_descr','-v7');
pbch_descr_save=pbch_descr;
[mib sfn N_ant]=lte_bch_decode(pbch_descr_save);
if N_ant > 0
    disp(['lte_pbch_decode: MIB found: ' num2str([sfn N_ant])]);
    return;
end

% Soft combine different repetitions of PBCH channel
pbch_descr0=pbch_descr(1:480);
pbch_descr1=pbch_descr(481:960);
pbch_descr2=pbch_descr(961:1440);
pbch_descr3=pbch_descr(1441:1920);
pbch_sc0=pbch_descr0(1:120)+pbch_descr0(121:240)+pbch_descr0(241:360)+pbch_descr0(361:480);
pbch_sc1=pbch_descr1(1:120)+pbch_descr1(121:240)+pbch_descr1(241:360)+pbch_descr1(361:480);
pbch_sc2=pbch_descr2(1:120)+pbch_descr2(121:240)+pbch_descr2(241:360)+pbch_descr2(361:480);
pbch_sc3=pbch_descr3(1:120)+pbch_descr3(121:240)+pbch_descr3(241:360)+pbch_descr3(361:480);
pbch_sc0=pbch_sc0/4;
pbch_sc1=pbch_sc1/4;
pbch_sc2=pbch_sc2/4;
pbch_sc3=pbch_sc3/4;
pbch_d0=[pbch_sc0;pbch_sc0;pbch_sc0;pbch_sc0];
pbch_d1=[pbch_sc1;pbch_sc1;pbch_sc1;pbch_sc1];
pbch_d2=[pbch_sc2;pbch_sc2;pbch_sc2;pbch_sc2];
pbch_d3=[pbch_sc3;pbch_sc3;pbch_sc3;pbch_sc3];
pbch_descr=[pbch_d0;pbch_d1;pbch_d2;pbch_d3];
% 
% save('/home/demel/exchange/matlab_pbch_descr_p.txt','pbch_descr','-ascii');
% save('/home/demel/exchange/Matlab_ld_pbch_descr.mat','pbch_descr','-v7');
% call lte_bch_decode for a descrambled sequence.
[mib sfn N_ant]=lte_bch_decode(pbch_descr);
if N_ant > 0
    disp(['lte_pbch_decode: MIB found: ' num2str([sfn N_ant])]);
end

end


