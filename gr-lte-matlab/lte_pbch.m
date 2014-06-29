function[y_vec]=lte_pbch(mib,N_id,N_ant)
% argument is MIB vector
% Function does PBCH calculation

% Define Variable
len=1920;
cinit=N_id;
style='tx_diversity';

% Get 1920bit BCH channel vector.
bch=lte_bch(mib,N_ant);


% Get cell-specific scrambling sequence
c_seq=lte_pn_generator(cinit,len);
% Scramble
scrb=mod(bch+c_seq,2);

% Modulation
qpsk_vec = zeros(960,1);
for i=0:960-1
    qpsk_vec(i+1)=lte_qpsk_modulator(scrb(2*i+1),scrb(2*i+2));
end
%qpsk_vec;

% Signal Processing Chain is as follows
% Modulation --> Layer Mapping --> Precoding
disp('lte_pbch-->qpsk_vec');
disp(qpsk_vec(1:10));
x_vec=lte_layer_mapper(qpsk_vec,N_ant,style);
disp('lte_pbch-->x_vec');
disp(x_vec(1:10,:));
y_vec=lte_pre_coder(x_vec,N_ant,style);
disp('lte_pbch-->y_vec');
disp(y_vec(1:10,:));
end