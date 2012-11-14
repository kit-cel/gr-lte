function[vit]=lte_viterbi_decode(conv_mat)
% function uses matlab functions for viterbi decoding of tail biting
% convolutional coding of BCH.

% Trellis preparations
constlen=7;
codegen=[133 171 165];
trellis=poly2trellis(constlen,codegen);

% Reshape input matrix for input to trellis decoding
conv_vec=reshape(conv_mat',120,1);


vit=vitdec([conv_vec;conv_vec],trellis,constlen,'trunc','unquant');
vit=vit(41:end);






end






