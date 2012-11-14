function[x]=lte_layer_mapper(d,v,style)
% function shall only support TX diversity. No plans for spatial
% multiplexing support yet.

if strcmp(style,'tx_diversity') % Do layer Mapping according to ETSI 136.211 Sec 6.3.3.3
    M_symb=length(d);
    if v==1
        x=d;
    elseif v==2
        M_layer_symb=M_symb/2;
        x=zeros(M_layer_symb,2);
        for i=0:M_layer_symb-1
            x(i+1,0+1)=d(2*i+1);
            x(i+1,1+1)=d(2*i+2);
        end
    elseif v==4
        if mod(M_symb,4)==0
            M_layer_symb=M_symb/4;
        else
            M_layer_symb=(M_symb+2)/4;
            d=[d;zeros(2,1)];
        end
        x=zeros(M_layer_symb,4);
        for i=0:M_layer_symb-1
            x(i+1,0+1)=d(4*i+0+1);
            x(i+1,1+1)=d(4*i+1+1);
            x(i+1,2+1)=d(4*i+2+1);
            x(i+1,3+1)=d(4*i+3+1);
        end
    else
        disp([num2str(v) ' layers are not supported!']);
        x=-1;
    end
else
    disp([style ' is not supported!']);
end






end