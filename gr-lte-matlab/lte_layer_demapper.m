function[d]=lte_layer_demapper(x,N_cw,style)
% arguments:    x is a matrix with layer symbols
%               N_cw is the number of codewords
%               style is 'tx_diversity' or 'spatial_multiplexing'
% only 'tx_diversity' is supported


[M_layer_symb v]=size(x);


if strcmp(style,'tx_diversity')
    if v==1
        d=x;
    elseif v==2
        M_symb=M_layer_symb*2;
        d=zeros(M_symb,1);
        for i=0:M_layer_symb-1
            d(2*i+0+1)=x(i+1,0+1);
            d(2*i+1+1)=x(i+1,1+1);
        end
        % disp('lte_layer_demapper-->d');
        % disp(d(1:10));
    elseif v==4
        M_symb=M_layer_symb*4;
        d=zeros(M_symb,1);
        for i=0:M_layer_symb-1
            d(4*i+0+1)=x(i+1,0+1);
            d(4*i+1+1)=x(i+1,1+1);
            d(4*i+2+1)=x(i+1,2+1);
            d(4*i+3+1)=x(i+1,3+1);
        end
        % disp('lte_layer_demapper-->d');
        % disp(d(1:10));
        % May cause problems at this point if x and d do mismatch.
    else
        disp([num2str(v) ' antenna ports not supported!']);
    end
else
    disp([style ' is not supported']);
end

end