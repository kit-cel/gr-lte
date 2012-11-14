function[invec]=lte_ref_sig_mapping(invec,ns,l,N_id,Ncp)
% function gets a vector and maps reference signals to it according to the
% current symbol.


reftest=invec;

%ns=0; % is the slot number within a radio frame
%Ncp=1; 
fftl=length(invec);
p=0; % is the antenna port
Nrbdlmin = 6; % are the smallest
Nrbdlmax = 110; % and largest downlink bandwidth
occupied_tones=1200*fftl/2048; % used subcarriers
N_rb_dl = 50; %occupied_tones/12; % actual downlink bandwidth


load('/home/demel/exchange/subfr00_rs.mat');
load('/home/demel/exchange/subfr04_rs.mat');
load('/home/demel/exchange/subfr07_rs.mat');
load('/home/demel/exchange/subfr011_rs.mat');
load('/home/demel/exchange/rs_vector.mat');



symb=lte_rs_subc_mapping(N_rb_dl,ns,l,N_id,Ncp,0);

if ns==0 && l==0
    myt1=find(symb);
    myt2=find(modvectest1);
    istest=isequal(myt1,myt2);
    disp(['lte_ref_sig_mapping: ' num2str(istest)]);
end



          

%disp(symb(300:320));
refvec=[0;symb(length(symb)/2+1:end);zeros(length(invec)-length(symb)-1,1);symb(1:length(symb)/2)];
% refvec=[symb(length(symb)/2+1:end);zeros(fftl-1-length(symb),1);symb(1:length(symb)/2)];


for i=1:length(refvec)
    if refvec(i)~=0
        invec(i)=refvec(i);
        %invec(i)=-2;
    end
end


if ns==1 && l==0
    disp([fftl ns l length(refvec) length(invec) length(symb)]);
    %disp(['lte_ref_sig_mapping: ' num2str(isequal(fvec,refvecgen))]);
    %disp([refvec(1:40)  reftest(1:40)  invec(1:40)]);
end
 

end