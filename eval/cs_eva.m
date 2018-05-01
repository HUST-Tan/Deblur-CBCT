%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% 获得"特地区域"的PSNR和SSIM值
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
clear 
clc



load 'data/CS_350X350X1_40.mat'
Iold=img;
% Iold=img(55:125,55:125);   % 菱形
% Iold=img(170:210,10:140);   % 条形码

load 'data/CS_350X350X16_init_5e3.mat'
IFDK=u(:,:,end/2);
% IFDK=u(55:125,55:125,end/2);
% IFDK=u(170:210,10:140,end/2);

% load 'result/result_cs_deblur/deblur_15.mat'
load 'result/result_cs/cs_0001_0001.mat'
D = deblur(:,:,end/2)/40;
% D = deblur(55:125,55:125,end/2)/40;
% D = deblur(170:210,10:140,end/2)/40;

% figure, imshow(Iold, []);
% figure, imshow(IFDK, []);
% figure, imshow(D, []);

IndOfEva(Iold, IFDK, D)
    
    
    




