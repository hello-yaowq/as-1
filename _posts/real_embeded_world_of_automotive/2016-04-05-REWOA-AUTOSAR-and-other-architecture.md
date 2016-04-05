---
layout: post
title: 软件架构AUTOSAR和其他架构的比较
category: real embeded world of automotive
comments: true
---

AUTOSAR已经发展到4.2版本，而目前我们能够从网上找到的比较完整的AUTOSAR开源版本是[ARCCORE v3.1](http://www.arccore.com/products/arctic-core/arctic-core-for-autosar-v31)，但也仅限Ｃ源代码部分开源，其BSW和RTE配置工具却并不开源。好吧，透露下，其实从github开源项目[MOPED](https://github.com/sics-sse/moped)你是可以找到ARCCORE V4.0,一开始时，其实我已经将该V4.0的代码并入我的个人仓库，但后来注意到其license并非纯粹GPL，所以果断退回到v3.1的版本，其实比较过代码之后发现，其实V3.1和V4.0没有特别大的区别，学习V3.1的代码看着AUTOSAR 4.2的版本文档，是没有任何问题的，基本差不多。

这里顺便说下关于GPL通用许可证的问题。刚开始接触GPLv2时，我觉得这个东西太好了，开源代码随便用，绝大多数还是免费的（注意开源不一定意味着免费），以后开发中不知道要省多少事情呢。可后来工作却慢慢发现，事情并非如此，很多商用专利软件很不喜欢GPL,因为使用GPL代码之后意味着你的那静态或者动态链接的部分代码也需要开源并同样以GPL协议发布，这是很多商业公司不能接受的，所以商业公司不喜欢GPL,所以会禁止GPL开源代码的使用，商业公司喜欢像BSD这样的许可证。

好吧，回到正题，文档[AUTOSAR\_EXP\_LayeredSoftwareArchitecture.pdf](http://www.autosar.org/fileadmin/files/releases/4-2/software-architecture/general/auxiliary/AUTOSAR_EXP_LayeredSoftwareArchitecture.pdf)是一个很完整的AUTOSAR软件架构介绍文档，对于上层应用而言，其所看到的是运行时环境（RTE），其提供了一切应用接口和任务调度,其一个完整架构如下图所示。

![autosar-architecture-icc3.png](/as/images/rewoa/autosar-architecture-icc3.png)

但时至今日我对AUTOSAR的了解也仅限于AUTOSAR RTE以下的部分基本软件模块（BSW），如操作系统OS，数据存储服务NvM和通讯服务COM。其一个更抽象的架构图如下图所示：

![autosar-ecuc](http://www.autosar.org/fileadmin/_processed_/csm_AUTOSAR_ECU_softw_arch_b_6269666540.jpg)

如上图所示，[AUTOSAR软件架构](http://www.autosar.org/about/technical-overview/)是为实现软件功能的模块化（modularity）／可扩展性（scalability）／可移植性（transferability）和可复用性（re-usability）而设计并为汽车电子提供了一个不同层级模块基于标准接口的通用的基础软件架构。AUTOSAR允许通过优化配置（如分区和资源使用），如果有必要通过本地优化来使硬件设备达到运行时需求，克服硬件设备的限制条件。

好吧，上面这一段是我翻译的一段AUTOSAR官方介绍，比较烂。其实也就是说，如果硬件受限，你可以不必使用一个完整的AUTOSAR软件架构，但只要提供符合AUTOSAR 运行时环境RTE就可以了，所以还是相当灵活的。

![VFB](http://www.autosar.org/fileadmin/_processed_/csm_AUTOSAR_TechnicalOverview_80b6ce5e82.jpg)