% Unit tests


% %% QPSK mod/demod
% disp('QPSK mod/demod test');
% d=[0 0 0 1 1 0 1 1];
% for i=1:2:8
%     q=lte_qpsk_modulator(d(i),d(i+1) );
%     bits=lte_qpsk_demodulator(q);
%     bits=(1-bits)/2;
%     dt(i:i+1)=bits;
% end
% isequal(round(d),round(dt))
% disp(d);
% disp(dt);
% clear d dt q bits;

% %% Rate matching/unmatching
% disp('Rate matching/unmatching');
% conv_mat=mod(randi(1000,40,3),2);
% rate_matched=lte_conv_code_rate_matching(conv_mat);
% length(rate_matched);
% 
% for i=1:120:length(rate_matched)
%     part=rate_matched(i:i+119);
%     erg=lte_rate_unmatch(part);
%     disp(isequal(conv_mat,erg));
% end
% 
% clear part erg conv_mat rate_matched;
% 
% %% convolutional Coding / Viterbi Decoding
% disp('convolutional Coding / Viterbi Decoding');
% 
% for i=0:2
%     code_vec=mod(randi(1000,40,1),2);
%     code_mat=lte_tail_b_conv_coding(code_vec);
%     [m n]=size(code_mat);
%     for j=1:m
%         for k=1:n
%             if code_mat(j,k)==1
%                 code_mat(j,k)=-0.1;
%             else
%                 code_mat(j,k)=0.1;
%             end
%         end
%     end
%     
%     erg=lte_viterbi_decode(code_mat);
%     disp([num2str(i) ' isequal: ' num2str(isequal(code_vec,erg)) ]);
% end
% 
% clear code_vec code_mat erg;
% 
% %% scrambling / descrambling
% disp('scrambling / descrambling');
% 
% len=1920;
% for N_id=0:2 % 503 % , for full test.
%     code_vec=mod(randi(1000,len,1),2);
% 
%     pn_s=lte_pn_generator(N_id,len);
%     pn_nrz=1-2*pn_s;
%     
%     scr=mod(code_vec+pn_s,2);
%     
%     for i=1:length(scr)
%         if scr(i)==1
%             scr(i)=-1;
%         else
%             scr(i)=1;
%         end
%     end
%     
%     des=scr.*pn_nrz;
%     
%     for i=1:length(des)
%         if des(i)==1
%             des(i)=0;
%         else
%             des(i)=1;
%         end
%     end
%  
%     disp(['N_id: ' num2str(N_id) ' isequal: ' num2str(isequal(code_vec,des)) ]);
% end
% 
% clear N_id code_vec pn_s scr des len;
% 
% %% BCH encode / decode
% disp(' ');
% disp(' ');
% disp('BCH encode/decode');
% 
% for N_ant=1:2
%     mib=lte_mib_pack(50,'normal',1,randi(1024,1,1)-1);
%     bch=lte_bch(mib,N_ant);
%     bch=[mod(ones(1800,1)+bch(1:1800),2);bch(1801:end)];
%     for i=1:length(bch)
%         if bch(i)==1
%             bch(i)=-1;
%         else
%             bch(i)=1;
%         end
%     end
% 
%     [erg sfn]=lte_bch_decode(bch);
%     disp(['BCH->N_ant: ' num2str(N_ant) ' isequal: ' num2str(isequal(mib,erg)) ' ' num2str(sfn)]);
% end
% 
% clear mib bch erg sfn_lsb N_ant;
% 
% 
% %% Layer Mapping/Demapping
% disp(' ');
% disp(' ');
% disp('Layer Mapping/Demapping');
% 
% style='tx_diversity';
% len=120;
% for v=1:4
%     d1=1-2*(randi(2,len,1)-1);
%     d2=1-2*(randi(2,len,1)-1);
%     d=(d1+1i*d2)/sqrt(2);
% 
%     x=lte_layer_mapper(d,v,style);
%     d_rx=lte_layer_demapper(x,1,style);
%     
%     disp(['d isequal d_rx: ' num2str(isequal(d,d_rx))]);
% end
% 
% clear style v d x d_rx;
% %% Pre Coding/Decoding
% disp(' ');
% disp(' ');
% disp('Pre Coding/Decoding');
% 
% style='tx_diversity';
% len=120;
% 
% for n=1:2
%     dtx=zeros(len,n);
%     for r=1:n
%         d1=1-2*(randi(2,len,1)-1);
%         d2=1-2*(randi(2,len,1)-1);
%         dtx(:,r)=(d1+1i*d2)/sqrt(2);
%     end
% 
%     
%     y=lte_pre_coder(dtx,n,style);
%     x=lte_pre_decoder(y,ones(length(y),n),n,style);
%     disp(['dtx isequal x: ' num2str(isequal(round(dtx),round(2*x)))]);
%     for m=1:len
%         if ~isequal(round(dtx(m,:)),round(2*x(m,:)))
%             disp([dtx(m,1) x(m,1)]);
%             disp([dtx(m,2) x(m,2)]);
%         end
%     end
% end
% 
% clear style len n dtx d1 d2 y x m;
% 
% %% Pre Coding/Decoding with Layer Mapping/Demapping
% disp(' ');
% disp(' ');
% disp('Pre Coding/Decoding with Layer Mapping/Demapping');
% 
% style='tx_diversity';
% len=240;
% 
% for v=1:2
%     d1=1-2*(randi(2,len,1)-1);
%     d2=1-2*(randi(2,len,1)-1);
%     dtx=(d1+1i*d2)/sqrt(2);
% 
%     x=lte_layer_mapper(dtx,v,style);
%     y=lte_pre_coder(x,v,style);
%     xrx=lte_pre_decoder(y,ones(length(y),v),v,style);
%     drx=lte_layer_demapper(xrx,1,style);
% 
%     disp(['dtx isequal x: ' num2str(isequal(round(dtx),round(drx)))]);
% 
% end
% 
%% PBCH encode/decode
disp(' ');
disp(' ');
disp('PBCH encode/decode');

%N_ant=1;
N_id=1;
for N_ant=1:2
    mib=lte_mib_pack(50,'normal',1,randi(1024,1,1)-1);
    pbch=lte_pbch(mib,N_id,N_ant);
    disp(['lte_unit_test-->size(pbch): ' num2str(size(pbch))]);
    for i=1:240:960
        pbch_frame=pbch(i:i+239,:);
        [erg sfn N_ant_rx]=lte_pbch_decode(pbch_frame,ones(length(pbch_frame),N_ant),N_id,N_ant);
        disp(['PBCH->N_id: ' num2str(N_id) ' isequal: ' num2str(isequal(mib,erg)) ' ' num2str(sfn) ' ' num2str(N_ant_rx)]);
    end

end

clear N_id mib pbch pbch_frame erg;




