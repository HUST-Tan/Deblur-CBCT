%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%创建人：孙楠博
%日期：2014/6/27
%修改人：
%日期：
%功能：计算一幅图像的PSNR
%      输入：Iold原始无噪声图像 I用算法去噪后的图像 MAX图像最大的可能灰度值 I
%      输出：v图像的PSNR值
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function v=PSNR(Iold,I,MAX)
[h w]=size(I);
mse=sum(sum((Iold-I).^2))/(h*w);
psnr=20*log10(MAX/sqrt(mse));
v=[mse psnr];
end