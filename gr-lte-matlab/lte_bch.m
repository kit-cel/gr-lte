function[bch]=lte_bch(in_vec,N_ant)
% Function generates 1920bit vector for pbch channel containing the encoded
% data of the BCH channel. input vector is 24bit long.

% Scrambling sequences for different MIMO configurations. 
x_ant1=zeros(16,1); % 1 antenna
x_ant2=ones(16,1); % 2 antennas
x_ant4=[0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1]'; % 4 antennas

crc_vec=crc16(in_vec); % output vector is 40bit long.

% Scramble CRC checksum according to MIMO configuration.
if N_ant==1
    crc_vec=[crc_vec(1:24);mod(crc_vec(25:40)+x_ant1,2)];
elseif N_ant==2
    crc_vec=[crc_vec(1:24);mod(crc_vec(25:40)+x_ant2,2)];
elseif N_ant==4
    crc_vec=[crc_vec(1:24);mod(crc_vec(25:40)+x_ant4,2)];
else
    disp('unknown antenna configuration');
end

conv_coded_vec=lte_tail_b_conv_coding(crc_vec); % output is matrix of size 40x3
bch=lte_conv_code_rate_matching(conv_coded_vec); % output is 1920bit vector


end