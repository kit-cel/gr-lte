function[e]=lte_conv_code_rate_matching(in_mat)
% Generate vector for code rate matched vector for convolutional coding


%disp('lte_conv_code_rate_matching!');
v0=lte_conv_code_subblock_interleaver(in_mat(:,1));
v1=lte_conv_code_subblock_interleaver(in_mat(:,2));
v2=lte_conv_code_subblock_interleaver(in_mat(:,3));
w=[v0;v1;v2];

Kw=length(w);
k=0;
j=0;
E=1920;
e=zeros(E,1);

%disp('lte_conv_code_rate_matching! -> bit collection');
while k < E
    if ~isnan( w(mod(j,Kw)+1) )
        e(k+1)=w(mod(j,Kw)+1);
        %disp([' k:' ,num2str(k), ' mod(j,Kw)+1:' , num2str( mod(j,Kw)+1 ) , ' k:' , num2str(k)]);
        k=k+1;
    end
    j=j+1;
end

    
        
        
end

    
    
    