function[sub_data_mat]=lte_get_subframe(invec,N_id,subframe_pos,fftl,ni,N_rb_dl)
% arguments are:
% input vector
% cell ID
% subframe start
% FFT length
% integer shift

% disp(subframe_pos);
cpl=floor(144*(fftl/2048));
cpl0=floor(160*(fftl/2048));
%cpldiff=floor(16*(fftl/2048));
syml=fftl+cpl;
syml0=fftl+cpl0;
% slot_symbs=7*fftl+6*cpl+cpl0;
% Ncp=1;

% currently the functions extracts the OFDM symbols and no further
% processing is done.
% subframe_mat=zeros(fftl,14);

mat_f=zeros(fftl,1);
num_symbs=15;
sub_data_mat=zeros(N_rb_dl*12,num_symbs);
idx=subframe_pos;
for i=0:num_symbs-1
    if i==0 || i==7 || i==14
        % disp(['lte_get_subframe-->idx: ' num2str(idx+syml0) ' ' num2str(i+1)]);
        idx=idx+cpl0;
    else
        % disp(['lte_get_subframe-->idx: ' num2str(idx+syml) ' ' num2str(i+1)]);
        idx=idx+cpl;
    end
    vec_t=invec(idx+1:idx+fftl);
    vec_f=fft(vec_t,fftl);
    mat_f=[mat_f;vec_f];
%     if i==7
%         plot(abs(vec_f));
%         title('lte get subframe');
%     end
    vec_f=circshift(vec_f,-ni);
    %disp(['lte_get_subframe-->length(vec_f1): ' num2str(length(vec_f(fftl-(12*N_rb_dl/2)+1:end))) ' length(vec_f2): ' num2str(length(vec_f(1+1:(12*N_rb_dl/2)+1 )) )]);
    vec_d=[vec_f(fftl-(12*N_rb_dl/2)+1:end);vec_f(1+1:(12*N_rb_dl/2)+1 )]; % get occupied subcarriers and remove DC carrier.
    %disp(['lte_get_subframe-->length(vec_d): ' num2str(length(vec_d)) ' length(sub_data_mat): ' num2str(length(sub_data_mat(:,i+1)) )]);
    sub_data_mat(:,i+1)=vec_d;
    % subframe_mat(:,i+1)=vec_f;
    idx=idx+fftl;
end

mat_f=mat_f(fftl+1:end);
% save('/home/demel/exchange/matlab_mat_f_gsf.mat','mat_f','-mat');
[m n]=size(sub_data_mat);
mat_ext=reshape(sub_data_mat,m*n,1);
% save('/home/demel/exchange/matlab_mat_ext_gsf.mat','mat_ext','-mat');





% [m n]=size(sub_data_mat);
% disp(['lte_get_subframe--> m n: ' num2str([m n])]);

% disp('lte_get_subframe-->scatterplot');
% disp(length(sub_data_mat(:,8)) );
% % %subcarrier=subframe_mat(20,:);
% figure(2)
% scatterplot(sub_data_mat(:,8),1,0, 'r.');
% title('LTE get subframe, one subcarrier');
% hold on;
% grid on;

        




























end