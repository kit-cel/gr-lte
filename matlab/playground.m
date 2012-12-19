fftl = 2048;
cpl = 144*fftl/2048;
cpl0 = 160*fftl/2048;
slotl = 7*fftl+6*cpl+cpl0;

frame_start = 0;
slot_start = mod(frame_start,slotl);
symbol_start = mod(slot_start+fftl+cpl0,fftl+cpl);

last_symb = 0;

next_symb = 0;

for i=0:5*slotl
    if frame_start == mod(i,10*slotl)
        disp(['frame_start' num2str(i) ' last ' num2str(last_symb)]);
        last_symb = i;
        next_symb = i+fftl+cpl0;
    elseif slot_start == mod(i,slotl)
        disp(['slot_start' num2str(i) ' last ' num2str(last_symb)]);
        last_symb = i;
        next_symb = i+fftl+cpl0;
    elseif next_symb == i
        disp(['symb_start ' num2str(i) ' lastdiff ' num2str(i-last_symb)]);
        last_symb = i;
        next_symb = i+fftl+cpl;
    end
end

