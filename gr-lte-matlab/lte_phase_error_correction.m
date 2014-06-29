function[out]=lte_phase_error_correction(ref,rx)



reft=ref;
rxt=rx(:,1);
phasevec=zeros(length(reft),1);
phasec=0;
firstidx=0;
for i=1:length(reft)
    if reft(i) ~=0
        phasevec(i)=angle(reft(i))-angle(rxt(i,1));
        phasec=phasevec(i);
        if firstidx==0;
            firstidx=i;
        end
    else
        phasevec(i)=phasec;
    end
end   

for i=1:firstidx
    phasevec(i)=phasevec(firstidx);
end


%rxt=rxt*exp(1i*phase1);
for i=1:length(rxt)
    rx(i,:)=rx(i,:)*exp(1i*phasevec(i));
end


%scatterplot([rxt(:,1)]);
%[ref rxt(:,1) abs(rxt(:,1))]
%disp(['dist un: ' num2str(abs(ref(69)-rx(69)) ) ' dist eq: ' num2str(abs(ref(69)-rxt(69)) )]);



out=rx;
end