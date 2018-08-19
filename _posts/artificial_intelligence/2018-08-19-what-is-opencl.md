---
layout: post
title: what is openCL
category: artificial intelligence
comments: true
---

# 1. A Hello World Demo
Get the source code from here [openCL Hello World demo](https://blog.csdn.net/jaccen2012/article/details/78810546), build with msys2 with below commands:

```sh
g++ main.cpp -I"%CUDA_PATH%\include" "%CUDA_PATH%\lib\x64\OpenCL.lib"
```


From this we could know that with openCL computing jobs can be assigned to other device(GPU/DSP/NPU, etc).

