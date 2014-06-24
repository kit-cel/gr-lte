function[y]=lte_pre_coder(x, N_ant, style)
% function performs pre coding
% x is a layer mapped matrix
% N_ant represents the number of antenna ports (only 1 and 2 are supported)
% style may be 'tx_diversity' or 'spatial_multiplexing', however, only
% tx_diversity is supported!

[M_layer_symb v]=size(x);

if strcmp(style,'tx_diversity')
    if N_ant==1
        y=x;
    elseif N_ant==2
        M_ap_symb=2*M_layer_symb;
        y=zeros(M_ap_symb,v);
        for n=0:M_layer_symb-1  
            y(2*n+0+1,0+1)=( 1*real(x(n+1,0+1))+1i*imag(x(n+1,0+1)) )/sqrt(2);
            y(2*n+0+1,1+1)=(-1*real(x(n+1,1+1))+1i*imag(x(n+1,1+1)) )/sqrt(2);
            y(2*n+1+1,0+1)=( 1*real(x(n+1,1+1))+1i*imag(x(n+1,1+1)) )/sqrt(2);
            y(2*n+1+1,1+1)=( 1*real(x(n+1,0+1))-1i*imag(x(n+1,0+1)) )/sqrt(2);
        end
    else
        disp([num2str(v) ' antenna ports not supported!']);
        y=-1;
    end
else
    disp([style ' is not supported!']);
    y=-1;
end




end

