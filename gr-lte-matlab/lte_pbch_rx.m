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
subframe_mat1=lte_get_subframe(invec(:,1),N_id,frame_start,fftl,ni,N_rb_dl);
subframe_mat2=lte_get_subframe(invec(:,2),N_id,frame_start,fftl,ni,N_rb_dl);

disp('lte_pbch_rx-->ce_and_correction!');
[subfr_eq1 ce_mat1]=lte_ce_and_correction(subframe_mat1,N_id,subfr_num,2);
[subfr_eq2 ce_mat2]=lte_ce_and_correction(subframe_mat2,N_id,subfr_num,2);
%pbch_m=subframe_mat(:,8:11,:);


pbch_m1=subfr_eq1(:,8:11,:);
pbch_m2=subfr_eq2(:,8:11,:);


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

%rx ant1
pbch10=pbch_m1(:,1,:);
pbch11=pbch_m1(:,2,:);
pbch12=pbch_m1(:,3,:);
pbch13=pbch_m1(:,4,:);

ce10=ce_mat1(:,8,:);
ce11=ce_mat1(:,9,:);
ce12=ce_mat1(:,10,:);
ce13=ce_mat1(:,11,:);


%rx ant2
pbch20=pbch_m2(:,1,:);
pbch21=pbch_m2(:,2,:);
pbch22=pbch_m2(:,3,:);
pbch23=pbch_m2(:,4,:);

ce20=ce_mat2(:,8,:);
ce21=ce_mat2(:,9,:);
ce22=ce_mat2(:,10,:);
ce23=ce_mat2(:,11,:);

% Identify REs which are reserved for reference symbols and remove them.
idx=0;
pbch_rs1=zeros(length(pbch10)*2/3,2,length(pbch_m1(1,1,:)));
ce_rs=zeros(length(ce10)*2/3,2,length(ce_mat1(1,1,:)));
for i=0:length(pbch10)-1
    if mod(N_id,3)~=mod(i,3)
        %pbch_ref(i+1)=pbch0(i+1);
        pbch_rs1(idx+1,1,:)=pbch10(i+1,:);
        ce_rs1  (idx+1,1,:)=ce10  (i+1,:);
        pbch_rs1(idx+1,2,:)=pbch11(i+1,:);
        ce_rs1  (idx+1,2,:)=ce11  (i+1,:);
        idx=idx+1;
    end
end


idx=0;
pbch_rs2=zeros(length(pbch20)*2/3,2,length(pbch_m2(1,1,:)));
ce_rs2=zeros(length(ce20)*2/3,2,length(ce_mat2(1,1,:)));
for i=0:length(pbch20)-1
    if mod(N_id,3)~=mod(i,3)
        %pbch_ref(i+1)=pbch0(i+1);
        pbch_rs2(idx+1,1,:)=pbch20(i+1,:);
        ce_rs2  (idx+1,1,:)=ce20  (i+1,:);
        pbch_rs2(idx+1,2,:)=pbch21(i+1,:);
        ce_rs2 (idx+1,2,:)= ce21  (i+1,:);
        idx=idx+1;
    end
end
%ind=find(abs(pbch0)); % Find all non-zero elements.
%pbch0=pbch0(ind); % extract non-zero elements.
%pbch1=pbch1(ind);


pbch10=pbch_rs1(:,1,:);
pbch11=pbch_rs1(:,2,:);
ce10=ce_rs1(:,1,:);
ce11=ce_rs1(:,2,:);
pbch_frame1=[pbch10;pbch11;pbch12;pbch13]; % put values together.
pbch_ce1=[ce10;ce11;ce12;ce13];

pbch20=pbch_rs2(:,1,:);
pbch21=pbch_rs2(:,2,:);
ce20=ce_rs2(:,1,:);
ce21=ce_rs2(:,2,:);
pbch_frame2=[pbch20;pbch21;pbch22;pbch23]; % put values together.
pbch_ce2=[ce20;ce21;ce22;ce23];


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
    for m=1:length(pbch_m1(1,1,:))
        [mib sfn N_ant]=lte_pbch_decode(pbch_frame1,pbch_ce1,pbch_frame2,pbch_ce2,N_id,n);
        if N_ant >0
            disp(['lte_pbch_rx: MIB found: ' num2str([sfn N_ant])]);
        end
    end
end





end