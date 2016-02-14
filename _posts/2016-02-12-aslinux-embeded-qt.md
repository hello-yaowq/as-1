---
layout: post
title: aslinux embeded qt as GUI develop
category: linux
comments: true
---

### [embeded qt](http://doc.qt.io/qt-4.8/qt-embedded-linux.html)

The article [embeded qt](http://doc.qt.io/qt-4.8/qt-embedded-linux.html) seems to be for 4.8.x version only, it can't be used for version 5.x.x. And I still encounter build issues, but I ignore the build errors as I need to fix the run command ts\_calibrate issue "ts_open: No such file or directory" of tslib when it try to open device /dev/input/touchscreen0. So that I need to research how to simulate a touchscreen on qemu vexpress a9, I search it on google, but I didn't find anything that could help me, it realy made me headache. 

But I have found out that the qemu mini2440 do support the QT/LCD simulation, so I would like to try the qemu mini2440 firstly to see whether it works. So I spend some more time to google on the net and set up the enviroment, but when the qemu-system-arm mini2440 boot from the nand.bin, it is OK to boot the kernel but the kernel would panic as the below message.

```sh
Warning: unable to open an initial console.
Kernel panic - not syncing: Attempted to kill init!
```

I feel no good and don't know how to fix the issue. so I think I should stop and start to read the fucking source code of u-boot and kernel
