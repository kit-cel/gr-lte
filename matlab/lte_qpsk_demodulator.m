function[out]=lte_qpsk_demodulator(in)

% bits  I   Q (/sqrt(2))
% 00    1   1
% 01    1   −1
% 10    −1  1
% 11    −1  −1


out=zeros(2,1);
out(1)=real(in);
out(2)=imag(in);

% if real(in) > 0 && imag(in) > 0
%     symb=(1+1i)/sqrt(2);
%     sd=lte_dist(in,symb);
%     out = [sd,sd];
% elseif real(in) > 0 && imag(in) < 0
%     symb=(1-1i)/sqrt(2);
%     sd=lte_dist(in,symb);
%     out = [sd,-sd];
% elseif real(in) < 0 && imag(in) > 0
%     symb=(-1+1i)/sqrt(2);
%     sd=lte_dist(in,symb);
%     out = [-sd,sd];
% elseif real(in) < 0 && imag(in) < 0
%     symb=(-1-1i)/sqrt(2);
%     sd=lte_dist(in,symb);
%     out = [-sd,-sd];
% end

% disp(['lte_qpsk_demodulator: ' num2str(out)]);

end

function[out]=lte_dist(symb,ref)
dist=abs(symb-ref);
if dist >= 1
    dist=1-(1/1000);
end
out=1-dist;


end