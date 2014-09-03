









invec = read_complex_binary('/home/maier/LTE test files/lte_capture_Wed_Aug_6_19:36:39_2014_RXant2_4MS_telekom_band1800_wg_karlruhe_hinten raus_antenna_port_0_channel_coeffs.dat',1000000000);
subcarriers=600;
channelmat = vec2mat(invec,subcarriers)';

powermat=abs(channelmat);
powermat=powermat.*powermat;

% impixelregion(imagesc(powermat(:,(10000:30000))));
contourf(powermat(:,(1000:3000)));

