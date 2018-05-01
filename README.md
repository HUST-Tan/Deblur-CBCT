# Statistical Iterative CBCT Reconstruction Based on Neural Network

This is an implementation of [our article]() on matlab, matconvnet. 

The repository includes:

- 3D-Projection：based on [3D Forward and Back-Projection for X-Ray CT Using Separable Footprints](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC2993760/)
- Deblur procedure
- CS Phantom demo

## Preparation

### Prerequisites

- matlab（tested on matlab16b(linux)）
- [matconvnet](http://www.vlfeat.org/matconvnet/)：please reference the official installation instructions to install it

### Get Starts

step1：clone the code

```shell
git clone https://github.com/HUST-Shan/Deblur-CBCT.git
```

step2：compile the c++ extension (for projection)

```matlab
mex cpp/updateu_method_cs.cpp
```

（You will find a compiled file `updatau_method_cs.mexa64` ）

step3：run CS phantom demo

```matlab
iter_cs
```

（Note: 1. You can find the results in result folder.  2. We only do deblur in the middle layer, not on whole 3D phantom.）

## Some Explain

1. If you run this code in windows, please change several "path form"
2. This pre-trained model is coming from [IRCNN](https://github.com/cszn/IRCNN)：the denoise network is a little different to article, however, you can replace the denoise network with "any denoise network or methods"（we will release a trainable model in our python version）
3. This demo only do deblur in one layer, not on whole objection.
4.  We will release a python version soon（Contain training phase.）

## Citation

```
@article{Deblur-CBCT
    Author = {Binbin Chen, Kai Xiang, Zaiwen Gong, Jing Wang*, Shan Tan*},
    Title = {Statistical Iterative CBCT Reconstruction Based on Neural Network},
    Journal = {Transactions on Medical Imaging},
    Year = {2017}
} 
```

