function iter_cs()

clear all
clc
addpath('cpp')
mkdir('result/result_cs')
file = fopen( 'result/results_cs.txt', 'wt' );

load('data/CS_350X350X16_init_5e3.mat')
load('data/CS_origin.mat')

w=0;
pauseTime = 1;
column_num=zeros(nv,Nprj,'int32');  
column_data=zeros(nv,Nprj,'single');
iternum = 30;

sigma_range = 0.01;
noise_range = 20;
deblur = zeros(350, 350, 16);

k_sigma = linspace(0.6157, sigma_range, iternum);
k_noise = linspace(30, noise_range, iternum);

% Beta_range = linspace(1e5, 5e6, 30);
beta=7.5e4;

% param for deblur
innernum = 10;
coeff=40;
num = 0;

best_psnr = 0;
img = coeff*single(img);

for kk=1:iternum
    tic
    avg_psnr = 0;
    noise = k_noise(kk);
    ksigma = k_sigma(kk);
    kernel = fspecial('gaussian', 9, ksigma);
    deblur(:,:,8)=Cal_Deblur(coeff*u(:, :, 8), kernel, noise, 49, 13, innernum);
    [PSNR_Cur,SSIM_Cur] = Cal_PSNRSSIM(255*img,255*deblur(:,:,8),0,0);
    fprintf(file, ['deblur psnr ', num2str(kk), ' : ', num2str(PSNR_Cur, '%2.2f'), 'db\n']);
    if PSNR_Cur>best_psnr
        best_psnr = PSNR_Cur;
    end
    disp(['iter ', num2str(kk), '  Layer ', num2str(8), '  PSNR :', num2str(PSNR_Cur,'%2.2f'),'dB']);
    imshow(cat(2,im2uint8(coeff*u(:, :, 8)),im2uint8(deblur(:,:,8)), im2uint8(img)));
    drawnow;
    title(['Iter: ', num2str(kk),  '    Layer ', num2str(8), '    ',num2str(PSNR_Cur,'%2.2f'),'dB'],'FontSize',12)
    pause(pauseTime)
    c=double(kernel);
    g=single(convn(deblur/coeff, c,'same'));
    updateu_method_cs(u,beta,w,int32([nx ny nz]),int32([nuT nvT]),NprjT,SDD,SAD,[dx dy dz],usT,vsT,var_1,r,s,angleT,column_num,column_data,g);
    [PSNR_Cur,SSIM_Cur] = Cal_PSNRSSIM(255*img,255*u(:,:,8)*coeff,0,0);
    fprintf(file, ['recons psnr ', num2str(kk), ' : ', num2str(PSNR_Cur, '%2.2f'), 'db\n']);
    disp(['iter ', num2str(kk), '  Reconstruct PSNR :', num2str(PSNR_Cur,'%2.2f'),'dB']);
    fprintf(file, '---------------------------------------------------------------------------------------\n');
end
deblur(:,:,8)=Cal_Deblur(u(:, :, 8)*coeff, kernel, noise, 49, 13, innernum);
[PSNR_Cur,SSIM_Cur] = Cal_PSNRSSIM(255*img,255*deblur(:,:,8),0,0);
if PSNR_Cur>best_psnr
    best_psnr = PSNR_Cur;
end
fprintf(file, ['Sigma:', num2str(sigma_range),  '   Noise:', num2str(noise_range),'  final psnr: ', num2str(PSNR_Cur, '%2.2f'), 'db\n']);
fprintf(file, '---------------------------------------------------------------------------------------\n');
save(sprintf('result/result_cs/cs_5e3'),'u','deblur','r');
disp(['save success!'])

fclose(file);

end

        
        