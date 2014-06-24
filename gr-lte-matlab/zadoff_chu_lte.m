% Zadoff Chu sequence generator for 63 elements. For LTE DC carrier must be
% set to zero. Corresponds to Element 32.
function[a]=zadoff_chu_lte(cell_id)
Nzc=63;
u=0;
if cell_id==0
    u=25;
end
if cell_id==1
    u=29;
end
if cell_id==2
    u=34;
end
a = zeros(Nzc,1);
for m=0:Nzc-1
    %phase = mod( (pi * u * m * (m+1) ) / (Nzc) ,pi*2); 
    phase = (pi * u * m * (m+1) ) / (Nzc);   
    a(m+1)=exp(complex(0,-1)*phase);
end


end
