function[m0, m1]=cell_id_g_indices(cell_id_g)
% Returns indices m0 and m1 derived from cell_id_g


qX = floor(cell_id_g/30);
q = floor(( cell_id_g + qX * (qX+1) /2 )/30 );
mX = cell_id_g + q*(q+1)/2;
m0 = mod(mX,31);
m1 = mod( (m0 + floor(mX/31) +1),31);
   
end