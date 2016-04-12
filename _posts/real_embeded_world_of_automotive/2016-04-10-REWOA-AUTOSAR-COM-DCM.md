---
layout: post
title: AUTOSAR通讯和诊断
category: real embeded world of automotive
comments: true
---

在整个AUTOSAR架构里，我觉得重点在于通讯和诊断服务，因为我认为VFB虚拟总线是AUTOSAR的核心。AUTOSAR通过抽象层，为不同的通讯方式如CAN/LIN/FlexRay/Ethernet等提供了统一的上层接口。目前国内外主流的通讯媒介是CAN，少数ECU的从机使用LIN来通讯。来一张图先看看AUTOSAR的CAN通讯栈。

![autosar-can-stack.png](/as/images/rewoa/autosar-can-stack.png)
<center> 图1 AUTOSAR CAN 协议栈 </center>

从图上看通讯可划分为3类： COM（Application Purpose），DCM（Diagnostic Purpose）和 NM（Network Management Purpose）。其中有个什么Large Data COM对我来说是个新东西，还没研究过，看名字估计用作大数据消息的传输，简单看了下文档，也确实如此，但在以前做产品的过程中接触的不是很多，所以对其不做评论。

对于CAN通讯，个人认为还是比较简单的，CAN报文属于明文广播传输，在同一个总线上的任何节点都可以收到其他节点发送来的报文，但是，由于总线上消息可能很多，有些是本节点不需要关心的，所以会采用硬件过滤的方式，只接收那些本节点所关心的报文。对于同一个ID的报文，同一总线上只能有一个节点上发送该报文。另外，每个节点该发生么报文和该接受什么报文，一般由整车厂来定义，并将与零部件节点相关的那部分分发给供应商，当然有些整车场会将所有的报文定义都给你，这份定义文档称作CAN通信矩阵(CAN Matrix),其严格定义了每个ID报文的长度(DLC<=8)，一般为８字节,报文里所含消息(signal)的格式和其意义。由于Vector CAN工具的广受欢迎，现在整车厂一般会提供一个Vector DBC格式文件供零部件供应商使用，现在有些零部件供应商已经基本都有一套成熟的工具直接导入该DBC文件直接生成CAN协议栈的配置文件。

在我的个人仓库AS中，就有２个DBC解析和导入工具，一个是[AS.xlsm](https://github.com/parai/as/blob/master/com/as.tool/config.infrastructure.system/AS.xlsm)微软办公软件Excel,使用VBA宏解析与导入DBC文件，另一个是利用python实现的一个解析工具[ascc](https://github.com/parai/as/tree/master/com/as.tool/py.can.database.access/ascc)，所以有兴趣的人可以基于以上２个工具进行二次开发，开发出和你正在使用的CAN协议栈配置文件生成工具。

这里以一张截图展示下我的工具[AS.xlsm](https://github.com/parai/as/blob/master/com/as.tool/config.infrastructure.system/AS.xlsm)，其可以很方便的导入Vector DBC文件,但目前的功能仅限于此，部支持其他生成AUTOSAR COM配置文件的功能，当初太懒了，没做，还有就是实在没有python来的简单快捷，所以放弃了使用EXCEL来生成AUTOSAR BSW配置文件的想法，但这个工具依然保留着以备不时之需。

![as-can-xlsm.png](/as/images/rewoa/as-can-xlsm.png)
<center> 图２ AUTOSAR CAN Excel Tool </center>

图２为导入Vector CANoe Demmo PowerTrain.dbc之后的样子，每一个CAN报文都有自己的ID,且各不相同，每一个报文可含有多个信号，如ABSData,图３为使用Vector DBC Editor打开该DBC文件后，报文ABSData的一个信号布局图。从而，收到报文ID为201（0xC9）的报文，ECU就知道该报文为ABSData并知道其数据场的前12位表征信号车速（CarSpeed），第15位表征信号齿轮锁（GearLock）等。好吧，也就是说，对于AUTOSAR的COM（Application Purpose）,其是根据整车厂提供的CAN通讯矩阵高度定制的，对同一总线上每个ECU来说其COM模块的配置文件是不一样的。

![as-can-dbc-absdata.png](/as/images/rewoa/as-can-dbc-absdata.png)
<center> 图3 AUTOSAR CAN报文ABSData </center>

说到这里就简单说下，常用的CAN控制器是个什么样子的。一般来说，主流的CAN控制器会有N个接受报文箱（RX MESSAGE BOX）和M个发送报文箱（TX MESSAGE BOX），一般而言，在做CAN的配置时，会按照CAN ID由小到大的顺序来依次配置RX MESSAGE BOX,设置每个RX MESSAGE BOX的过滤器(filter)让其直接收特定CAN ID的报文，如上图2所示，可以配置第1个RX MESSAGE BOX只接收报文Ignition_Info,可配置第2个RX MESSAGE BOX接收ID范围为0x500至0x5FF的NM网络管理报文(一般而言，NM网路管理报文为一个连续地址范围)，可以配置第3个RX MESSAGE BOX只接收报文GearBoxInfo,以此类推。一般而言COM和DCM报文对应一个唯一的RX MESSAGE BOX,只有在报文太多的情况下，会酌情考虑将ID相近的报文使用同一个RX MESSAGE BOX来接受。对于发送报文，与接受相似，但是，一般而言Ｍ要小于Ｎ很多，所以一般情况下会将所有TX MESSAGE BOX链接在一起使用。



