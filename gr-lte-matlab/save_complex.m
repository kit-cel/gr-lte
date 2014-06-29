function [] = save_complex(compl_mat, n_vec, filename)
%SAVE_COMPLEX Save Vectors for GNURADIO
%   Vectors are interleaved for GNURADIO
%   all vectors have to be the same length


l=length(compl_mat);

a=zeros(n_vec*l,1);

%interleave rows of compl_mat
if n_vec==2
    a(1:2:end) = compl_mat(:,1);
    a(2:2:end) = compl_mat(:,2);
else
    a=compl_mat;
end
    
z(1:2:2*length(a)-1) = real(a);
z(2:2:2*length(a)) = imag(a);

fid=fopen(filename,'w');      % open a file for WRITING with the name testdata1
fwrite(fid,z,'float');         % write the variable z1 in that file
fclose(fid);



end

