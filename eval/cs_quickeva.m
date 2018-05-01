%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% 获得"特地区域"的PSNR和SSIM值
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
clear 
clc

load 'data/CS_350X350X1_40.mat'
Iold=img(210:320,34:144);

load 'data/CS_350X350X16_init_5e3.mat'
IFDK=u(210:320,34:144,end/2);

% load 'result/result_cs_deblur/deblur_15.mat'
load 'result/result_cs_v2/cs_iter_30.mat'
D = deblur(210:320,34:144,end/2)/40;

% figure, imshow(Iold, []);
% figure, imshow(IFDK, []);
% figure, imshow(D, []);

IndOfEva(Iold, IFDK, IFDK)
IndOfEva(Iold, IFDK, D)
    
    
    




