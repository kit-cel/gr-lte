function[blockmatrev]=lte_rate_unmatch(blockraterev)
% decode code rate matched sequence of 120bit and returns 40x3 matrix for
% convolutional decoding.

%my_t_mib=[1 0 1 1 1 0 1 1 1 1 0 1 1 0 1 0 0 0 0 1 0 0 1 1]';
%crc_vec=crc16(my_t_mib); % output vector is 40bit long.
%my_t_mat=lte_tail_b_conv_coding(crc_vec); % output is matrix of size 40x3
%bch=lte_conv_code_rate_matching(my_t_mat); % 
%blockraterev=bch;

if length(blockraterev) ~=120
    disp('rate mismatch!!!');
end

%blockrev=bch(1:40);
ic_perm = [1,17,9,25,5,21,13,29,3,19,11,27,7,23,15,31,0,16,8,24,4,20,12,28,2,18,10,26,6,22,14,30];
% Generate dummy vector to identify NULL positions.
genvec=ones(40,1);
int_dummy=lte_conv_code_subblock_interleaver(genvec);
blockmatrev=zeros(40,3);
for lin=0:2
    blockrev=blockraterev(lin*40+1:(lin+1)*40);
   
    
    % insert NULL or NaN values
    v0=zeros(64,1);
    idx=0;
    for i=0:length(int_dummy)-1
        if int_dummy(i+1)==1
            v0(i+1)=blockrev(idx+1);
            idx=idx+1;
        else
            v0(i+1)=NaN;
        end
    end
       
    % invert row bit collection.
    mat0=zeros(2,32);
    csub=32;
    rsub=2;
    for i=1:csub
        for j=1:rsub
            mat0(j,i)=v0((i-1)*rsub+j);
        end
    end

    mat0rev=zeros(2,32);
    de_mat_p= mat0;
    for i=0:length(ic_perm)-1
        mat0rev(:,ic_perm(i+1)+1)=de_mat_p(:,i+1);
    end

    y0rev=[mat0rev(1,:)';mat0rev(2,:)'];

    vec_deint=y0rev(25:end);
    blockmatrev(:,lin+1)=vec_deint;

end


%disp(blockmatrev)
%disp(my_t_mat-blockmatrev)







end