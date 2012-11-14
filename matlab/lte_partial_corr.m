function[c]=lte_partial_corr(in1,in2)
% Function calculates correlation of input vectors by calculating partial
% correlations of length 7.

% assert(length(in1)==length(in2));

c=0;
for z=0:7:length(in1)-7
    %idx=z+1
    %length(in1(z+1:z+7))
    c=c+abs(corr( in1(z+1:z+7) , in2(z+1:z+7) ));
end
z=z+7;
%disp(['lte_partial_corr-->c1: ' num2str(c)]);
%disp('end loop');
%disp(z);
%disp(length(in1( z+1:length(in1) ))   )
% disp(['lte_partial_corr: ' num2str([length(in1) length(in2)] )]);
% disp(['lte_partial_corr: ' num2str([length(in1( z+1:length(in1) )) length(in2( z+1:length(in1) ))] )]);

if length(in1( z+1:length(in1) )) < 2
    c=c+sqrt(in1( z+1:length(in1) )*in2( z+1:length(in1) ));
    %disp(['lte_partial_corr-->length=1 c2: ' num2str(c)]);
else
    c=c+abs( corr( in1( z+1:length(in1) ),in2( z+1:length(in1) )) );
    %disp(['lte_partial_corr-->length>1 c2: ' num2str(c)]);
end
%disp(['lte_partial_corr-->c2: ' num2str(c)]);
c=c/ ceil( length(in1)/7 );

end
