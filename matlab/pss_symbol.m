function[pss]=pss_symbol(cell_id_n)
% Returns vector of length 72 with a zadoff_chu sequence according to the
% generation rules
% PSS Symbol occupies 72 carriers.
% 5 first and 5 last indices are ZEROS
% DC carrier value is excluded/ignored
% cell_id = 0,1,2
% cell_id_group: one out of 168 possibilities
pss = zeros(72,1);
zc = zadoff_chu_split(cell_id_n);
pss(6:67)=zc;
end