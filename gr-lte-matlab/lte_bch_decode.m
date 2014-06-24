function[mib,sfn,N_ant]=lte_bch_decode(pbch_descr)
% functions decodes BCH and returns MIB. 

% Initialize variables.
mib=ones(24,1);
mib=-1*mib;
sfn=-1;
N_rb_dl=-1;
phich_dur='unknown';
phich_res=-1;
N_ant=-1;

Hdec=crc.detector('Polynomial', [1 0 0 0 1 0 0 0 0 0 0 1 0 0 0 0 1], 'InitialState', '0x0000');
% CRC masks for different numbers of antennas
x_ant1=zeros(16,1);
x_ant2=ones(16,1);
x_ant4=[0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1]';



%mib=pbch_descr;
%disp('lte_pbch_decode -> now decoding MIB');
for i=1:120:1920
    my_pbch=pbch_descr(i:i+119);
%     save('/home/maier/Dropbox/gr-lte-matlab/save/matlab_pbch_descr.txt','my_pbch','-ascii');
    d=lte_rate_unmatch(pbch_descr(i:i+119) );
    dr=reshape(d',120,1);
%     save('/home/maier/Dropbox/gr-lte-matlab/save/matlab_d.txt','dr','-ascii');
    vit=lte_viterbi_decode(d);
%     save('/home/maier/Dropbox/gr-lte-matlab/save/matlab_vit.mat','vit','-mat');
    vit1=[vit(1:24);mod(vit(25:end)+x_ant1,2)];
    vit2=[vit(1:24);mod(vit(25:end)+x_ant2,2)];   
    vit4=[vit(1:24);mod(vit(25:end)+x_ant4,2)];
    matdecerr=ones(4,1)*-1;
    matdecerr(3)=-5;
    [matdect matdecerr(1)]=detect(Hdec,vit1);
%     save('/home/maier/Dropbox/gr-lte-matlab/save/matlab_dec1.mat','matdect','-mat');
    [matdect matdecerr(2)]=detect(Hdec,vit2);
%     save('/home/maier/Dropbox/gr-lte-matlab/save/matlab_dec2.mat','matdect','-mat');
    [matdect matdecerr(4)]=detect(Hdec,vit4);
%     save('/home/maier/Dropbox/gr-lte-matlab/save/matlab_dec4.mat','matdect','-mat');
    %disp(['Matlab CRC detector error: ' num2str(matdecerr')]);
    %crcvit=crc16(vit(1:24));
    %disp(vit);
    if matdecerr(1)==0
        N_ant=1;
    elseif matdecerr(2)==0
        N_ant=2;
    elseif matdecerr(4)==0
        N_ant=4;
    else
        %disp('lte_bch_decode: Could not decode MIB!');
    end
    
    if N_ant >0
        %disp(['MIB found ' num2str(i)  ]);
        disp(['Matlab CRC detector error: ' num2str(matdecerr')]);
        mib=vit(1:24);
        if i<480
            sfn_lsb=0;
        elseif i<960
            sfn_lsb=1;
        elseif i<480*3
            sfn_lsb=2;
        else
            sfn_lsb=3;
        end
        [N_rb_dl phich_dur phich_res sfn_msb]=lte_mib_unpack(mib);
        sfn=sfn_msb+sfn_lsb;
        break;
    end
end

% MIB data not yet returned to calling function.
res_sfn=[phich_res sfn];
disp(['lte_bch_decode-->decoded MIB: ' num2str(N_rb_dl) ', ' phich_dur ', ' num2str(res_sfn) ', N_ant: ' num2str(N_ant)]);














end