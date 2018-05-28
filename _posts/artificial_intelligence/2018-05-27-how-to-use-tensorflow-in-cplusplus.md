---
layout: post
title: how to use tensorflow in C++
category: artificial intelligence
comments: true
---

Recently, as some of the reason, I have a look at of [Mask RCNN](https://github.com/matterport/Mask_RCNN) which is based on keras. With some research on net, I find some [scripts](https://github.com/ericj974/Mask_RCNN/blob/master/scripts/export_model.py) that could export the keras model file(*.h5) to tensorflow model file(*.pb), and a [script](https://github.com/fastlater/Mask_RCNN/blob/master/infere_from_pb.py) that could load the converted tensorflow model and run it in tersoflow framework but this script need a little modification for the [Mask RCNN 2.0 Relase](https://github.com/fastlater/Mask_RCNN/blob/master/infere_from_pb.py). Now I have create a repository with all the script integrated, here it is [https://github.com/parai/Mask_RCNN](https://github.com/parai/Mask_RCNN).

But this doesn't satisfy me, I now know how to load and run the converted Mask RCNN model in python, but how to load and run it in C++? This is what I really what to know as I am an application engineer, I am not a reseacher about what a machine learning model is and how it works. What I want to know is how to retrain a model and deploy it on devices such as PC or Phone.

But first of all, I want to know how to load and run it in C++.

# 1. Env Prepare

## 1.1 Build out c++ library
```sh
git clone https://github.com/tensorflow/tensorflow
cd tensorflow
./configure
bazel build --config=opt //tensorflow:libtensorflow_cc.so

cd tensorflow/contrib/makefile
chmod +x *.sh
./build_all_linux.sh

# bwlow file is the base tensorflow library for C++ applicaiton
parai@UX303LB:~/workspace/tensorflow/tensorflow/bazel-bin/tensorflow$ ls -l libtensorflow*.so
-r-xr-xr-x 1 parai parai 123880984 5月  27 19:20 libtensorflow_cc.so
```

## 1.2 verify that C++ library

### 1.2.1 create a C++ source file main.cpp

```cpp
#include <tensorflow/core/platform/env.h>
#include <tensorflow/core/public/session.h>

#include <iostream>

using namespace std;
using namespace tensorflow;

int main()
{
    Session* session;
    Status status = NewSession(SessionOptions(), &session);
    if (!status.ok()) {
        cout << status.ToString() << "\n";
        return 1;
    }
    cout << "Session successfully created.\n";
}
```

### 1.2.2 build the sample



```sh
export TFDIR=~/workspace/tensorflow/tensorflow
gcc main.cpp  -I${TFDIR} -I${TFDIR}/bazel-genfiles \
  -I`readlink -f ${TFDIR}/bazel-genfiles`/../../../external/protobuf_archive/src \
  -I${TFDIR}/third_party/eigen3
```


# Reference

[1.loading tensorflow graph with c api](https://medium.com/jim-fleming/loading-a-tensorflow-graph-with-the-c-api-4caaff88463f)
