---
layout: post
title: AUTOSAR TCPIP
category: real embeded world of automotive
comments: true
---

okay, first of all, we should know the basic frame format of an Ethernet Frame, the [PDF file from baidu](http://wenku.baidu.com/view/d6639ebaf121dd36a32d8249.html?re=view) do introuduce it very clearly. And as it was complex, it was not possible for me to implement it, so I found out the ARCCORE 4.2 implementation of TCPIP stack based on [LWIP](http://savannah.nongnu.org/projects/lwip/).

As I think that all type of devices which has the attribute that exchange data with a fixed format as the same as CAN can be easily simulated for the purpose to study the protocol based on that device, so I integrated the code I got from [PWC-Demonstrator repositioy](https://github.com/z2akhtar/PWC-Demonstrator.git) into ascore which can be simulated running on host machine Ubuntu, and now it was done, and it works good. The below picture shows how it works.

![aslwip_on_ubunut.png](/as/images/rewoa/aslwip_on_ubunut.png)

So as the picture shows above, the ascore and asserver are applications on host machine ubuntu, the ascore OS is based on posix thread(yes, it was [toppers_osek](https://www.toppers.jp/osek-os.html) that I ported it running on posix thread as the same strategy that [FreeRTOS](http://www.freertos.org/FreeRTOS-simulator-for-Linux.html) does). For the ethernet device, I simulated it on socket(I have also simulated a CAN device on socket, and it works good,too). 

By this kind of simulation of device strategy, I can focus on the study of the protocal things, get rid of the bing up hardware devices, it really saves me a lot of precious time.

Back to the picture above, the asserver is a socket server that will receive the Ethernet Frame from the instance client of ascore 0/1/2(in fact, the number of instance ascore is not limited, it is up to the performance of host machine Ubuntu), and then forward the received frame to other instance client of ascore, so that a network is build, and the ascore 0/1/2 each will be a node.

So further on, I need to research the LWIP stack and implemenet different ascore applications so that they can communicate with each other.
