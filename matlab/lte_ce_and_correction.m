function[subfr_mat_r,ce_mat]=lte_ce_and_correction(subfr_mat,N_id,subfr_num,N_ant)
% function takes matrix with data of one subframe and does channel
% estimation and correction.
% subfr_mat must have 15 columns and the number of rows is calculated by
% N_rb_dl*12.
N_ant=2;
ns=subfr_num*2; % value of first slot within the subframe
[msm nsm]=size(subfr_mat);
N_rb_dl=msm/12;
rs_mat=zeros(N_rb_dl*12,nsm,2);
ce_mat=zeros(N_rb_dl*12,nsm,2);
Ncp=1;
vshift=mod(N_id,6);

% disp(['lte_ce_and_correction: ' num2str([ns N_rb_dl Ncp])]);

symb10 = lte_rs_subc_mapping(N_rb_dl,ns  ,0,N_id,Ncp,0);
symb14 = lte_rs_subc_mapping(N_rb_dl,ns  ,4,N_id,Ncp,0);
symb17 = lte_rs_subc_mapping(N_rb_dl,ns+1,0,N_id,Ncp,0);
symb111= lte_rs_subc_mapping(N_rb_dl,ns+1,4,N_id,Ncp,0);
symb114= lte_rs_subc_mapping(N_rb_dl,ns+2,0,N_id,Ncp,0);

symb20 = lte_rs_subc_mapping(N_rb_dl,ns  ,0,N_id,Ncp,1);
symb24 = lte_rs_subc_mapping(N_rb_dl,ns  ,4,N_id,Ncp,1);
symb27 = lte_rs_subc_mapping(N_rb_dl,ns+1,0,N_id,Ncp,1);
symb211= lte_rs_subc_mapping(N_rb_dl,ns+1,4,N_id,Ncp,1);
symb214= lte_rs_subc_mapping(N_rb_dl,ns+2,0,N_id,Ncp,1);

rs_mat(:,1,1) =symb10 ;
rs_mat(:,5,1) =symb14 ;
rs_mat(:,8,1) =symb17 ;
rs_mat(:,12,1)=symb111;
rs_mat(:,15,1)=symb114;

rs_mat(:,1,2) =symb20 ;
rs_mat(:,5,2) =symb24 ;
rs_mat(:,8,2) =symb27 ;
rs_mat(:,12,2)=symb211;
rs_mat(:,15,2)=symb214;




% Interpolation for OFDM symbols carrying Reference symbols.
% Interpolation in frequency domain.
for ant=1:2
    first_rs=0;
    last_rs=0;
    symb_idx=[0 4 7 11 14];
    for kidx=1:length(symb_idx)
        i=symb_idx(kidx);
        for j=0:N_rb_dl*12-1
            if rs_mat(j+1,i+1,ant)~=0
                tmp=subfr_mat(j+1,i+1)/rs_mat(j+1,i+1,ant);
                mag=abs(tmp);
                phase=angle(tmp);
                %phasebug=phase;
                if j > 6
                    while phase - angle(subfr_mat(j+1-6,i+1)/rs_mat(j+1-6,i+1,ant)) > pi
                        phase=phase-2*pi;
                    end
                    while phase - angle(subfr_mat(j+1-6,i+1)/rs_mat(j+1-6,i+1,ant)) < -pi
                        phase=phase+2*pi;
                    end
                    last_rs=j;
                else
                    first_rs=j;
                end
                ce_mat(j+1,i+1,ant)=mag*exp(1i*phase);

                if j>6
                    magdiff=( abs(ce_mat(j+1,i+1,ant))-abs(ce_mat(j+1-6,i+1,ant)) )/6;
                    phasediff=( angle(ce_mat(j+1,i+1,ant))-angle(ce_mat(j+1-6,i+1,ant)) )/6;

                    for k=1:5
                        ce_e=(abs(ce_mat(j+1-6,i+1,ant))+magdiff*k)*exp(1i*(angle(ce_mat(j+1-6,i+1,ant))+phasediff*k   ));
                        %disp([ce_e abs(ce_e) angle(ce_e)]);
                        ce_mat(k+j+1-6,i+1,ant)=ce_e;
                    end
                end

                %disp([rs_mat(j+1,i+1) subfr_mat(j+1,i+1) ce_mat(j+1,i+1)]);
                %disp([phase phasebug]);
            end
        end

        % Calculate CEs before first RS
        magdiff=( abs(ce_mat(first_rs+1+6,i+1,ant))-abs(ce_mat(first_rs+1,i+1,ant)) )/6;
        phasediff=( angle(ce_mat(first_rs+1+6,i+1,ant))-angle(ce_mat(first_rs+1,i+1,ant)) )/6;
        magc=abs(ce_mat(first_rs+1,i+1,ant));
        phasec=angle(ce_mat(first_rs+1,i+1,ant));
        for m=0:first_rs-1
            ce_e=(magc-magdiff*(first_rs-m))*exp(1i*(phasec-phasediff*(first_rs-m)));
            ce_mat(m+1,i+1,ant)=ce_e;           
        end

        % Calculate CEs after last RS
        magdiff=( abs(ce_mat(last_rs+1,i+1,ant))-abs(ce_mat(last_rs+1-6,i+1,ant)) )/6;
        magc=abs(ce_mat(last_rs+1,i+1,ant));
        phasediff=( angle(ce_mat(last_rs+1,i+1,ant))-angle(ce_mat(last_rs+1-6,i+1,ant)) )/6;
        phasec=angle(ce_mat(last_rs+1,i+1,ant));
        for m=1:N_rb_dl*12-last_rs-1
            ce_e=(magc+magdiff*m)*exp(1i*(phasec+phasediff*m));
            ce_mat(last_rs+m+1,i+1,ant)=ce_e;
        end
        
        
        v = 0;
        p = N_ant;
        if     (p==0 && l==0)
            v=0;
        elseif (p==0 && l~=0)
            v=3;
        elseif (p==1 && l==0)
            v=3;
        elseif (p==1 && l~=0)
            v=0;
        end
        shift = mod(v+vshift,6);
        tmp_vec = subfr_mat(:,i+1).*conj(rs_mat(:,i+1,N_ant));

        for q=0: 12*N_rb_dl-1
            if(mod(q,6) == shift && q > 6)
                mag = tmp_vec(q+1-6);
                diff = ( tmp_vec(q+1) - tmp_vec(q+1-6) )/6;
                for n=1:6-1
                    %tmp_vec(1+i+n) = (diff_re + 1i*diff_im);
                    tmp_vec(1+q-6+n) = mag + n * diff;
                    %%tmp_vec(1+i+n) = mag_re + n * diff_re;
                    %tmp_vec(1+i+n) = tmp_vec(1+i+n) + 1i*(mag_im + n * diff_im);
                end
            end
        end

        hold_val = tmp_vec(shift+1);
        diff_val = (tmp_vec(shift+6+1) - tmp_vec(shift+1))/6.0;
        for n = 1:shift-1
            tmp_vec(n+1) = hold_val - (shift-n) * diff_val;
        end

        hold_val = tmp_vec(last_rs+1);
        diff_val = (tmp_vec(last_rs+1) - tmp_vec(last_rs+1-6))/6.0;
        for n = 1:6-1-shift-1
            tmp_vec(last_rs+1+n) = hold_val + n * diff_val;
        end
    
        %disp([ce_mat(:,i+1,ant) tmp_vec]);
        
    end
    

    
    % Interpolation between OFDM symbols.
    % Interpolation in time domain.
    for h=0:length(ce_mat(:,1,ant))-1
        % Interpolation for each row.

        % Interpolate between 0th and 4th symbol
        md=(abs(ce_mat(h+1,5,ant))-abs(ce_mat(h+1,1,ant)))/4;
        ma=abs(ce_mat(h+1,1,ant));
        pd=(angle(ce_mat(h+1,5,ant))-angle(ce_mat(h+1,1,ant)))/4;
        pa=angle(ce_mat(h+1,1,ant));
        for o=1:3
            ce_mat(h+1,o+1,ant)=(ma+md*o)*exp(1i*(pa+pd*o));
        end

        % Interpolate between 4th and 7th symbol
        md=(abs(ce_mat(h+1,8,ant))-abs(ce_mat(h+1,5,ant)))/3;
        ma=abs(ce_mat(h+1,5,ant));
        pd=(angle(ce_mat(h+1,8,ant))-angle(ce_mat(h+1,5,ant)))/3;
        pa=angle(ce_mat(h+1,5,ant));
        for o=1:2
            ce_mat(h+1,o+5,ant)=(ma+md*o)*exp(1i*(pa+pd*o));
        end

        % Interpolate between 7th and 11th symbol
        md=(abs(ce_mat(h+1,12,ant))-abs(ce_mat(h+1,8,ant)))/4;
        ma=abs(ce_mat(h+1,8,ant));
        pd=(angle(ce_mat(h+1,12,ant))-angle(ce_mat(h+1,8,ant)))/4;
        pa=angle(ce_mat(h+1,8,ant));
        for o=1:3
            ce_mat(h+1,o+8,ant)=(ma+md*o)*exp(1i*(pa+pd*o));
        end

        % Interpolate between 11th and 14th symbol
        md=(abs(ce_mat(h+1,15,ant))-abs(ce_mat(h+1,12,ant)))/3;
        ma=abs(ce_mat(h+1,12,ant));
        pd=(angle(ce_mat(h+1,15,ant))-angle(ce_mat(h+1,12,ant)))/3;
        pa=angle(ce_mat(h+1,12,ant));
        for o=1:2
            ce_mat(h+1,o+12,ant)=(ma+md*o)*exp(1i*(pa+pd*o));
        end

    end
end
%disp([subfr_mat(1:10,1) abs(ce_mat(1:10,1)) angle(ce_mat(1:10,1))]);

save_subfr_m=subfr_mat;
% save_subfr_m=save_subfr_m/norm(save_subfr_m);
if N_ant==1
    subfr_mat_r(:,:,1)=subfr_mat; % ./ce_mat(:,:,1);
    subfr_mat_r(:,:,2)=subfr_mat;
else
    subfr_mat_r=zeros(N_rb_dl*12,nsm,2);
    subfr_mat_r(:,:,1)=subfr_mat; % .*conj(ce_mat(:,:,1))./(abs(ce_mat(:,:,1)).^2);
    subfr_mat_r(:,:,2)=subfr_mat; % .*conj(ce_mat(:,:,2))./(abs(ce_mat(:,:,2)).^2);
end









disp('lte_ce_and_correction');



end