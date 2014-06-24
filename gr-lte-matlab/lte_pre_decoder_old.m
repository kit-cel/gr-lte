function[x]=lte_pre_decoder_old(y,ce,N_ant,style)
% function decodes pre coded vector
% style may be 'tx_diversity' or 'spatial_multiplexing', however, only
% tx_diversity is supported!

[M_ap_symb,~]=size(y);
%disp(['lte_pre_decoder size(y) = ' num2str(size(y) )]);
if strcmp(style,'tx_diversity')
    if N_ant==1
        x=y(:,1,1)./ce(:,1);
    elseif N_ant==2
        M_layer_symb=M_ap_symb/2;
        x =zeros(M_layer_symb,N_ant);
        x1=zeros(M_layer_symb,N_ant);
        x2=zeros(M_layer_symb,N_ant);
        for n=0:M_layer_symb-1
            ce_n1=abs(ce(2*n+0+1,1))^2+abs(ce(2*n+0+1,2))^2;
            ce_n2=abs(ce(2*n+0+1,2))^2+abs(ce(2*n+0+1,1))^2;
            x (n+1,0+1)   = (conj(ce(2*n+0+1,1)) *y(2*n+0+1,1,1) + ce(2*n+1+1,2) *conj(y(2*n+1+1,1,1)))/ce_n1;
            % x1(n+1,0+1)   = y(2*n+0+1)+conj(y(2*n+1+1));
            % x2(n+1,0+1)   = (y(2*n+0+1,0+1)+conj(y(2*n+1+1,1+1)))/sqrt(2);
            % x (n+1,1+1)   = conj(-conj(ce(2*n+0+1,2)) *y(2*n+0+1) + ce(2*n+1+1,1) *conj(y(2*n+1+1)))/ce_n2;
            x (n+1,1+1)   = conj(-conj(ce(2*n+0+1,2)) *y(2*n+0+1,1,1) + ce(2*n+0+1,1) *conj(y(2*n+1+1,1,1)))/ce_n2;
            % x1(n+1,1+1)   = -1*y(2*n+0+1)+conj(y(2*n+1+1));  
            % x2(n+1,1+1)   = (y(2*n+1+1,0+1)-conj(y(2*n+0+1,1+1)))/sqrt(2);

            % disp([xvgl(n+1,0+1) x2(n+1,0+1) abs(x2(n+1,0+1)-xvgl(n+1,0+1))]);
            
            % disp([xvgl(n+1,1+1) x2(n+1,1+1) abs(x2(n+1,1+1)-xvgl(n+1,1+1))]);
            
        end
        % x10c=corr(xvgl(:,0+1),x1(:,0+1));
        % x20c=corr(xvgl(:,0+1),x2(:,0+1));
        %disp([abs(x20c) angle(x20c) norm(xvgl(:,1)) norm(x2(:,1))]);
        % x11c=corr(xvgl(:,1+1),x1(:,1+1));
        % x21c=corr(xvgl(:,1+1),x2(:,1+1));
        %disp([abs(x21c) angle(x21c) norm(xvgl(:,2)) norm(x2(:,2))]);
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