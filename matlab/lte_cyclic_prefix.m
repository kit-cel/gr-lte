function[outvec]=lte_cyclic_prefix(invec,cpl)
outvec=[invec(length(invec)-cpl+1:length(invec))',invec']';
end