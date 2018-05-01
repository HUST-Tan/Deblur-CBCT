function IndOfEva(Iold,IFDK, I)
p=PSNR(Iold,I,max(Iold(:)));
s=ISNR(Iold,IFDK,I);
K = [0.01 0.03]; 
window = fspecial('gaussian', 11, 1.5);
L=0.025;
[m,map]=ssim_index(Iold,I,K,window,L);
[p, s, m]
end

