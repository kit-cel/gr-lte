function[r]=lte_ref_sig_gen(ns,l,N_id,Ncp)
% formerly known as ref_sig_generation
% ns   == slot number within a radio frame
% l    == OFDM symbol number within a slot
% N_id == cell ID of the cell
% Ncp  == represents the cyclic prefix type 1 == normal ; 0 == extended
% The pseudo-random sequence generator shall be initialised
% at the start of each OFDM symbol.

Nrbmax=110; % Maximum number of ressource blocks in the system.
% Get reference signals
cinit=(2^10)*(7*(ns+1)+l+1)*(2*N_id+1)+2*N_id+Ncp;
% if ns==1
%     disp(['lte_ref_sig_gen->cinit: ' num2str(cinit) ' N_id: ' num2str(N_id) ' ns: ' num2str(ns) ' l: ' num2str(l) ' Ncp: ' num2str(Ncp)]);
% end
c=lte_pn_generator(cinit,4*Nrbmax);
r=zeros(2*Nrbmax,1);
for m=0:2*Nrbmax-1
    r(m+1)=(1-2*c(2*m+1))/sqrt(2) + 1i* (1-2*c(2*m+1+1))/sqrt(2); %complex( (1-2*c(2*m+1))/sqrt(2) , (1-2*c(2*m+1+1))/sqrt(2) ); 
end


%r=r*20; % higher values for test purposes
% Reference signal generation DONE!
end