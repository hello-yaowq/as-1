---
layout: post
title: AUTOSAR通讯和诊断
category: real embeded world of automotive
comments: true
---

在整个AUTOSAR架构里，我觉得重点在于通讯和诊断服务，因为我认为VFB虚拟总线是AUTOSAR的核心。AUTOSAR通过抽象层，为不同的通讯方式如CAN/LIN/FlexRay/Ethernet等提供了统一的上层接口。目前国内外主流的通讯媒介是CAN，少数ECU的从机使用LIN来通讯。来一张图先看看AUTOSAR的CAN通讯栈。

![autosar-can-stack.png](/as/images/rewoa/autosar-can-stack.png)
<center> 图1 AUTOSAR CAN STACK </center>

从图上看通讯可划分为3类： COM（Application Purpose），DCM（Diagnostic Purpose）和 NM（Network Management Purpose）。其中有个什么Large Data COM对我来说是个新东西，还没研究过，看名字估计用作大数据消息的传输，简单看了下文档，也确实如此，但在以前做产品的过程中接触的不是很多，所以对其不做评论。

对于CAN通讯，个人认为还是比较简单的，CAN报文属于明文广播传输，在同一个总线上的任何节点都可以收到其他节点发送来的报文，但是，由于总线上消息可能很多，有些是本节点不需要关心的，所以会采用硬件过滤的方式，只接收那些本节点所关心的报文。对于同一个ID的报文，同一总线上只能有一个节点上发送该报文。另外，每个节点该发生么报文和该接受什么报文，一般由整车厂来定义，并将与零部件节点相关的那部分分发给供应商，当然有些整车场会将所有的报文定义都给你，这份定义文档称作CAN通信矩阵(CAN Matrix),其严格定义了每个ID报文的长度(DLC<=8)，一般为８字节,报文里所含消息(signal)的格式和其意义。由于Vector CAN工具的广受欢迎，现在整车厂一般会提供一个Vector DBC格式文件供零部件供应商使用，现在有些零部件供应商已经基本都有一套成熟的工具直接导入该DBC文件直接生成CAN stack的COM部分。


