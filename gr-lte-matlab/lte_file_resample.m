function[ldr]=lte_file_resample(fnum,fftl,samp_rate)

path='/home/demel/Dokumente/Messungen_31_10_2012/';
%path='/home/demel/Dokumente/Messungen/';
prefix='samples_12.5MSps_';
name='NONE';

switch fnum
    case 0
        name = '30dB_aa';
    case 1
        name = '30dB_ab';
    case 2
        name = '30dB_ac';
    case 3
        name = '30dB_ad';
    case 4
        name = '30dB_ae';
    case 5
        name = '30dB_af';    
    case 6
        name = '30dB_ag';
    case 7
        name = '25dB_aa';
    case 8
        name = '25dB_ab';
    case 9
        name = '25dB_ac';
    case 10
        name = '25dB_ad';
    case 11
        name = '25dB_ae';
    case 12
        name = '20dB_aa';    
    case 13
        name = '20dB_ab';       
    case 14
        name = '20dB_ac';
    case 15
        name = '20dB_ad';    
    case 16
        name = '20dB_ae';
    case 17
        name = '15dB_aa';    
    case 18
        name = '15dB_ab';       
    case 19
        name = '15dB_ac';
    case 20
        name = '15dB_ad';    
    case 21
        name = '15dB_ae';           
        
        
        
        
        
    otherwise
        disp([num2str(fnum) ' is not a valid file case']);    
end    

filename=[path prefix name];
disp(filename);

ldr=lte_resample(filename,fftl,samp_rate);



















end