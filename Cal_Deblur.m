function [u] = Cal_Deblur(blur_image, kernel, noise, sigma1, sigma2, iternum)
addpath('utilities')
folderModel  = 'models';
useGPU = false;
% ---init---
k = kernel;
sigma = noise/255;
totalIter = iternum;      
lamda       = (sigma^2)/3;

modelSigma1 = sigma1;       
modelSigma2 = sigma2;       
modelSigmaS = logspace(log10(modelSigma1),log10(modelSigma2),totalIter);

rho = sigma^2/((modelSigma1/255)^2);
ns = min(25,max(ceil(modelSigmaS/2),1));
ns = [ns(1)-1,ns];
disp(['---------Image Debluring-----']);
y = blur_image;
[w, h, c] = size(y);
V = psf2otf(k,[w,h]);
denominator = abs(V).^2;
upperleft   = conj(V).*fft2(y);
load(fullfile(folderModel,'modelgray.mat'));
z = single(y);

% ---iter op---
tic;
for itern = 1:totalIter
    % ---step 1---
    rho = lamda*255^2/(modelSigmaS(itern)^2);
    z = real(ifft2((upperleft + rho*fft2(z))./(denominator + rho)));
    % change net coef---
    if ns(itern+1)~=ns(itern)
        [net] = loadmodel(modelSigmaS(itern),CNNdenoiser);
        net = vl_simplenn_tidy(net);
        if useGPU
            net = vl_simplenn_move(net, 'gpu');
        end
    end
    % ---step 2---
    res = vl_simplenn(net, z,[],[],'conserveMemory',true,'mode','test');
    residual = res(end).x;
    z = z - residual;
end
toc;

u = z;
