function[N_id_2,ni,pss_pos]=lte_pss_sync(invec,in_pos,fftl)
% Syncronization based on PSS || arg=invec,pos,fftl,cpl
% Return: N_id_2 : cell ID number
%         ni     : integer offset
%         pss_pos: position of PSS symbol
%         pss_err: time offset between coarse sync and fine sync

tic;
% Prepare variables
N_id_2=-1;
ni=-1;
pos=in_pos;
cpl=floor(144*(fftl/2048));
cpl0=floor(160*(fftl/2048));
cpldiff=floor(16*(fftl/2048));
syml=fftl+cpl;
syml0=fftl+cpl0;
slot_symbs=7*fftl+6*cpl+cpl0;
% disp(['lte_pss_sync->syml: ' num2str(syml) ' cpl0: ' num2str(cpl0) ' cpl: ' num2str(cpl) ' cpldiff: ' num2str(cpldiff)]);

% Generate Zadoff_Chu sequences with DC subcarrier
chu0=lte_pss_zc(0);
chu0=[chu0(1:31);0;chu0(32:end)];
chu1=lte_pss_zc(1);
chu1=[chu1(1:31);0;chu1(32:end)];
chu2=lte_pss_zc(2);
chu2=[chu2(1:31);0;chu2(32:end)];



% Find integer offset and cell ID number
if pos < syml
    disp(['pss_sync pos = ' num2str(pos) ' incremented = ' num2str(pos+syml)]);
    pos=pos+syml;
end

count=0;
max_corr=0;
max_pos=zeros(3,1);
corrvec=zeros(1400,1);
expos=0;

disp('lte_pss_sync: find PSS and N_id_2');
for h=0:200 % (length(invec)/slot_symbs) % -25
    %disp('lte_pss_sync: find PSS and N_id_2 outer loop');
    for j=0:6
        idx=pos+h*slot_symbs+j*syml;
        
        if mod(count,7)==0
            expos=expos+syml0;
            %disp(['lte_pss_sync->additional correction ' num2str(idx) ' ' num2str(expos) ' error: ' num2str(expos-idx)]);
        else
            expos=expos+syml;
        end
        count=count+1;

        if idx >= length(invec)
            idx=length(invec);
        end
        ft=fft(invec(idx-fftl+1:idx) ,fftl);
        %ft1=fft(invec(h-fftl+1+10*slot_symbs:h+10*slot_symbs) ,fftl);
        t=[ft(fftl-30:fftl);ft(1:32)];
        %t1=[ft1(fftl-30:fftl);ft1(1:32)];
        %t=(t+t1)/2;
        cpos=zeros(3,1);
        c0x=xcorr(t,chu0);
        [c0 cpos(1)]=max(c0x);
        c1x=xcorr(t,chu1);
        [c1 cpos(2)]=max(c1x);
        corrvec(count)=c1; % vector for test visualization
        c2x=xcorr(t,chu2);
        [c2 cpos(3)]=max(c2x);
        c=[c0 c1 c2];
        if abs(max(c))>abs(max_corr)
            [m i]=max(c);
            N_id_2=i-1;
            max_corr=m;
            ni=cpos(i)-63;
            switch N_id_2
                case 0
                    plot(abs(c0x));
                case 1
                    plot(abs(c1x));   
                case 2
                    plot(abs(c2x));   
            end
            %plot(abs(c0x));
            %plot(abs(ft));
            title(['lte PSS sync ' num2str(N_id_2)])
            

            cmnorm=m/(sqrt(sum(abs(t)))*sqrt(sum(abs(chu0))));
            %disp(['lte_pss_sync->max found ' num2str(idx) ' ' num2str(expos) ' error: ' num2str(expos-idx) ' N_id: ' num2str(N_id_2)]);
            disp(['lte_pss_sync-->coarse ' num2str(idx) ' ' num2str(abs(cmnorm)) ' ' num2str(N_id_2)]);
            %fttest=fft(invec(15360-fftl+1:15360) ,fftl);

            %ttest=[fttest(fftl-30:fftl);fttest(1:32)];
            %phase=angle(ttest)-angle(chu1);
            %disp([ttest chu1 phase]);
            %max_pos(3)=max_pos(2);
            %max_pos(2)=max_pos(1);
            max_pos(1)=idx;
        end
        
    end
end

maxmax = 0;
relpos = 0;
for m=max_pos(1)-5:max_pos(1)+5
    ft=fft(invec(m-fftl+1:m) ,fftl);
    t=[ft(fftl-30:fftl);ft(1:32)];

    c1x=xcorr(t,chu1);
    c1=max(c1x);
    disp(['val = ' num2str(c1) ' phase = ' num2str(angle(c1)) ' mag = ' num2str(abs(c1)) ' pos = ' num2str(m)]);
    if abs(maxmax) < abs(c1)
        maxmax = c1;
        relpos = m-max_pos(1);
    end
   
end
disp(['max mag = ' num2str(abs(maxmax)) ' relpos = ' num2str(relpos)]);

%plot(abs(corrvec));

%plot(abs(fft(invec(max_pos(1)-fftl+1:max_pos(1) ) ,fftl)));

toc;

% Prepare Zadoff-Chu sequence for further time sync.
% vector chu_n holds chu sequence according to detected cell ID number.
chu_n=chu0;
if N_id_2==0
    chu_n=chu0;
elseif N_id_2==1
    chu_n=chu1;
elseif N_id_2==2
    chu_n=chu2;
end




% Prepare vectors with chosen Chu sequence.
PSS=[zeros(5,1);chu_n(1:end);zeros(5,1)]; % Tested, assumed to be correct
fvec=zeros(fftl,1);
fvec_p=[PSS(37:end);fvec(1:fftl-length(PSS) );PSS(1:36)];
%fvec  =[PSS(37:end);fvec(1:fftl-length(PSS) );PSS(1:36)]; % copied from cp_sync_test



% Integer offset not yet removed. Therefore -ni 
fvec=circshift(fvec_p,ni); 

chu_t=ifft(fvec ,fftl);
%tvec =ifft(fveci,fftl); % first use of fveci

chu_t=chu_t/norm(chu_t);
%tvec =tvec /norm(tvec );

tic;
save_max_pos = max_pos(1);
% First plot!
%subplot(2,2,1:2);
%plot(abs(xcorr(chu_t,mytest)));
disp('lte_pss_sync: find fine POS ');
mpos=zeros(10,1);
%pos_err=0;
for i=0:3
    % Averaging over 3 PSS symbols
    chu_s1=invec(max_pos(1)-fftl+1:max_pos(1) );
    fine_corr=xcorr(chu_s1,chu_t);
    if length(invec) > max_pos(1)+10*slot_symbs
        chu_s2=invec(max_pos(1)-fftl+1+10*slot_symbs:max_pos(1)+10*slot_symbs );
        fine_corr=(fine_corr+xcorr(chu_s2,chu_t))/2;
    end
    if length(invec) > max_pos(1)+20*slot_symbs
        chu_s3=invec(max_pos(1)-fftl+1+20*slot_symbs:max_pos(1)+20*slot_symbs );
        fine_corr=(fine_corr+xcorr(chu_s3,chu_t))/2;
    end
    % fine_corr=xcorr(chu_s1,chu_t)+xcorr(chu_s2,chu_t)+xcorr(chu_s3,chu_t);
    % fine_corr=fine_corr/3;
    %fine_corr=fine_corr/norm(fine_corr);
    
    
    %plot(abs(fine_corr));
    disp(['lte_pss_sync-->fine_corr: ' num2str(max(abs(fine_corr)))]);

    % Get maximum value and calculate fine time offset
    [~,mpos(i+1)]=max(abs(fine_corr));
    %disp(['fine_corr max: ' num2str(val)]);
    %disp(['fine_corr pos: ' num2str(mpos(i+1)) ' -> ' num2str(mpos(i+1)-2048)]);
    
    if mpos(i+1)==fftl
        break;
    else
        disp(['lte_pss_sync->before: ' num2str(max_pos')]);
        max_pos=max_pos+(mpos(i+1)-fftl);
        disp(['lte_pss_sync-> after: ' num2str(max_pos')]);
        %pos_err=pos_err+(mpos(i+1)-2048);
        %disp(pos_err);
        %pos=pos-pos_err;
    end
end

nidx=max_pos(1);
% Plot of the hopefully exact PSS symbol
% ft=fft(invec(nidx-fftl+1:nidx) ,fftl);
% plot(abs(ft));
% title('lte pss sync');


disp(['PSS fine_pos = ' num2str(max_pos(1)) ' coarse_pos = ' num2str(save_max_pos) ' diff = ' num2str(max_pos(1)-save_max_pos)]);

tmp = save_max_pos;%max_pos(1);
coarse_start=zeros(1,7);
while(tmp > 0)
    tmp = tmp - syml;
end
for n=1:7
    coarse_start(n) = tmp + (n*syml);
end
%pss_pos=coarse_start;
pss_pos=max_pos(1); % -2; % -2, because early sync only results in a phase shift. otherwise ISI expected.

if N_id_2 ==-1
    disp('lte_pss_sync->ERROR could not sync PSS!');
end

toc;
end