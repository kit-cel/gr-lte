function[x]=lte_pre_decoder(y1,ce1,y2,ce2,N_ant,style)
% function decodes pre coded vector
% style may be 'tx_diversity' or 'spatial_multiplexing', however, only
% tx_diversity is supported!
% load('/home/maier/Dropbox/gr-lte-matlab/save/Matlab_pre_decoded_x.mat');

[M_ap_symb,~]=size(y1);
%disp(['lte_pre_decoder size(y) = ' num2str(size(y) )]);
if strcmp(style,'tx_diversity')
    if N_ant==1
        x=y1(:,1,1)./ce1(:,1)+y2(:,1,1)./ce2(:,1);
    elseif N_ant==2
        M_layer_symb=M_ap_symb/2;
        x =zeros(M_layer_symb,N_ant);
        
        for n=0:M_layer_symb-1
            ce_n1=abs(ce1(2*n+0+1,1))^2+abs(ce1(2*n+0+1,2))^2+abs(ce2(2*n+1+1,1))^2+abs(ce2(2*n+1+1,2))^2;
            
            ce_n2=abs(ce1(2*n+0+1,1))^2+abs(ce1(2*n+0+1,2))^2+abs(ce2(2*n+0+1,1))^2+abs(ce2(2*n+0+1,2))^2;
            
            
            x (n+1,0+1)   = (conj(ce1(2*n+0+1,1)) *y1(2*n+0+1,1,1) + ce2(2*n+1+1,1) *conj(y1(2*n+1+1,1,1)) ...
                            +conj(ce1(2*n+0+1,2)) *y2(2*n+0+1,1,1) + ce2(2*n+1+1,2) *conj(y2(2*n+1+1,1,1)) )/ce_n1;
                        
            x (n+1,1+1)   = (conj(ce2(2*n+0+1,1)) *y1(2*n+0+1,1,1) - ce1(2*n+1+1,1) *conj(y1(2*n+1+1,1,1)) ...
                            +conj(ce2(2*n+0+1,2)) *y2(2*n+0+1,1,1) - ce2(2*n+1+1,1) *conj(y2(2*n+1+1,1,1)) )/ce_n2;
            
            
            
            %x (n+1,0+1)   = (conj(ce(2*n+0+1,1)) *y(2*n+0+1,1,1) + ce(2*n+1+1,2) *conj(y(2*n+1+1,1,1)))/ce_n1;

            %x (n+1,1+1)   = conj(-conj(ce(2*n+0+1,2)) *y(2*n+0+1,1,1) + ce(2*n+0+1,1) *conj(y(2*n+1+1,1,1)))/ce_n2;

            
        end
    else
        disp([num2str(N_ant) ' antenna ports not supported']);
        x=-1;
    end

else
    disp([style ' is not supported']);
    x=-1;
end


%save('/home/demel/exchange/Matlab_pre_decoded_x.mat','x','-v7');

% x(n+1,0+1)=(y(2*n+0+1,0+1)+conj(y(2*n+1+1,1+1)))/sqrt(2); % y(2*n+0+1)+conj(y(2*n+1+1));
% x(n+1,1+1)=(y(2*n+1+1,0+1)-conj(y(2*n+0+1,1+1)))/sqrt(2); % -1*y(2*n+0+1)+conj(y(2*n+1+1));   






end
