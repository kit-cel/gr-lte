function[mib sfn N_ant]=lte_pbch_rx(invec,frame_start,ni,N_id,fftl)
% receive PBCH and decode it!


% Calculate constants, derived from fftl
cpl=floor(144*fftl/2048);
cpl0=floor(160*fftl/2048);
cpldiff=floor(16*fftl/2048);
syml=fftl+cpl;
syml0=fftl+cpl0;
slot_symbs=7*fftl+6*cpl+cpl0;
subfr_num=0;
Ncp=1;

% Variables for further processing
% pss_pos=pss_pos-1; % -1 pss_pos points at last sample of the OFDM symbol containing PSS
frame_start=frame_start-1;
% frame_start=pss_pos-slot_symbs; % pss_pos now points at 1st slot --> frame starts at 0th slot.
% disp(['lte_pbch_rx: ' num2str([pss_pos frame_start])]);
N_rb_dl=6;
vshift=mod(N_id,6); % cell specific reference symbol shift.
disp('lte_pbch_rx-->get_subframe!');
subframe_mat=lte_get_subframe(invec,N_id,frame_start,fftl,ni,N_rb_dl);
disp('lte_pbch_rx-->ce_and_correction!');
[subfr_eq ce_mat]=lte_ce_and_correction(subframe_mat,N_id,subfr_num,2);
%pbch_m=subframe_mat(:,8:11,:);
pbch_m=subfr_eq(:,8:11,:);


% for GnuRadio TestCase purposes
% frame_samples = invec(frame_start+1:frame_start+20*slot_symbs);
% save('/home/demel/exchange/matlab_frame_samples.mat','frame_samples','-mat');

% for GnuRadio TestCase purposes
% mat_t=zeros(fftl,1);
% idx=frame_start;
% num_symbs=140;
% for i=0:num_symbs-1
%     if mod(i,7)==0
%         idx=idx+cpl0;
%     else
%         idx=idx+cpl;
%     end
%     vec_t=invec(idx+1:idx+fftl);
%     mat_t=[mat_t;vec_t];
%     idx=idx+fftl;
% end
% mat_t=mat_t(fftl+1:end);
% save('/home/demel/exchange/matlab_mat_t.mat','mat_t','-mat');

% for GnuRadio TestCase purposes
% mat_f=zeros(fftl,1);
% idx=frame_start;
% num_symbs=140;
% for i=0:num_symbs-1
%     if mod(i,7)==0
%         idx=idx+cpl0;
%     else
%         idx=idx+cpl;
%     end
%     vec_t=invec(idx+1:idx+fftl);
%     vec_f=fft(vec_t,fftl);
%     mat_f=[mat_f;vec_f];
%     idx=idx+fftl;
% end
% mat_f=mat_f(fftl+1:end);
% save('/home/demel/exchange/matlab_mat_f.mat','mat_f','-mat');

% for GnuRadio TestCase purposes
%disp(['size(ce_mat) = ' num2str(size(ce_mat))]);
sub_pos=frame_start;
% sub_mat = lte_get_subframe(invec,N_id,sub_pos,fftl,ni,N_rb_dl);
% frame_mat=sub_mat(:,1:14);
% [~,ce_sub_mat]=lte_ce_and_correction(sub_mat,N_id,0,2);
% ce_frame_mat1=ce_sub_mat(:,1:14,1);
% ce_frame_mat2=ce_sub_mat(:,1:14,2);
% sub_pos = sub_pos + 2*slot_symbs;
% for i = 1 : 10-1
%     sub_mat = lte_get_subframe(invec,N_id,sub_pos,fftl,ni,N_rb_dl);
%     frame_mat=[frame_mat sub_mat(:,1:14)];
%     [~,ce_sub_mat]=lte_ce_and_correction(sub_mat,N_id,i,2);
%     ce_frame_mat1=[ce_frame_mat1 ce_sub_mat(:,1:14,1)];
%     ce_frame_mat2=[ce_frame_mat2 ce_sub_mat(:,1:14,2)];
% 
%     sub_pos = sub_pos + 2*slot_symbs;
% end
%frame_mat = [frame_mat lte_get_subframe(invec,N_id,sub_pos,fftl,ni,N_rb_dl)];
%ce_sub_mat=lte_ce_and_correction(sub_mat,N_id,9,2);
%ce_frame_mat1=[ce_frame_mat1 ce_sub_mat(:,:,1)];
%ce_frame_mat2=[ce_frame_mat2 ce_sub_mat(:,:,2)];
%disp(['size(frame_mat)     = ' num2str(size(frame_mat))]);
%disp(['size(ce_frame_mat1) = ' num2str(size(ce_frame_mat1))]);
%disp(['size(ce_frame_mat2) = ' num2str(size(ce_frame_mat2))]);
% frame_mat=reshape(frame_mat,72*140,1);
% save('/home/demel/exchange/matlab_frame.mat','frame_mat','-mat');
% ce_frame_mat1=reshape(ce_frame_mat1,72*140,1);
% save('/home/demel/exchange/matlab_ce_frame_mat1.mat','ce_frame_mat1','-mat');
% ce_frame_mat2=reshape(ce_frame_mat2,72*140,1);
% save('/home/demel/exchange/matlab_ce_frame_mat2.mat','ce_frame_mat2','-mat');



% disp(subfr_eq(10,8,1));
% disp(subfr_eq(1:6,8:11,:));
pbch0=pbch_m(:,1,:);
pbch1=pbch_m(:,2,:);
pbch2=pbch_m(:,3,:);
pbch3=pbch_m(:,4,:);
ce0=ce_mat(:,8,:);
ce1=ce_mat(:,9,:);
ce2=ce_mat(:,10,:);
ce3=ce_mat(:,11,:);

% Identify REs which are reserved for reference symbols and remove them.
idx=0;
pbch_rs=zeros(length(pbch0)*2/3,2,length(pbch_m(1,1,:)));
ce_rs=zeros(length(ce0)*2/3,2,length(ce_mat(1,1,:)));
for i=0:length(pbch0)-1
    if mod(N_id,3)~=mod(i,3)
        %pbch_ref(i+1)=pbch0(i+1);
        pbch_rs(idx+1,1,:)=pbch0(i+1,:);
        ce_rs  (idx+1,1,:)=ce0  (i+1,:);
        pbch_rs(idx+1,2,:)=pbch1(i+1,:);
        ce_rs  (idx+1,2,:)=ce1  (i+1,:);
        idx=idx+1;
    end
end
%ind=find(abs(pbch0)); % Find all non-zero elements.
%pbch0=pbch0(ind); % extract non-zero elements.
%pbch1=pbch1(ind);
pbch0=pbch_rs(:,1,:);
pbch1=pbch_rs(:,2,:);
ce0=ce_rs(:,1,:);
ce1=ce_rs(:,2,:);
pbch_frame=[pbch0;pbch1;pbch2;pbch3]; % put values together.
pbch_ce=[ce0;ce1;ce2;ce3];




% pbch_ce1=pbch_ce(:,1);
% save('/home/demel/exchange/matlab_pbch_ce1d.mat','pbch_ce1','-mat');
% pbch_ce1=pbch_ce(:,2);
% save('/home/demel/exchange/matlab_pbch_ce2d.mat','pbch_ce1','-mat');


% disp([pbch_m(:,1:3)]);
% disp([pbch0 pbch1]);
%disp('lte_pbch_rx-->pbch_decode!');
% close all;
% Does all the decoding stuff.
for n=1:2
    for m=1:length(pbch_m(1,1,:))
        [mib sfn N_ant]=lte_pbch_decode(pbch_frame,pbch_ce,N_id,n);
        if N_ant >0
            disp(['lte_pbch_rx: MIB found: ' num2str([sfn N_ant])]);
            return;
        end
    end
end





end