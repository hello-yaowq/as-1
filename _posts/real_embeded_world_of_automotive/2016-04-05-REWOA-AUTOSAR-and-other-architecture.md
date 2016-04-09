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

好吧，上面这一段是我翻译的一段AUTOSAR官方介绍，比较烂。其实也就是说，如果硬件受限，你可以不必使用一个完整的AUTOSAR软件架构，但只要提供符合AUTOSAR 运行时环境RTE就可以了，所以还是相当灵活的。但我个人认为这并不是AUTOSAR的精髓，因为任何一家汽车电子公司在长时间的开发过程中其已经形成了符合其产品特色的软件架构平台，其已经具备了上述特点，并且比AUTOSAR更加的实用和经济。这里不管国外形式如何，我只以我过往经验和经历来说，在国内众车场竞争日益激烈的今天，省成本是他们要考虑的一个重要因子，而AUTOSAR对硬件资源的消耗无疑是巨大的。其实，我觉得道理很简单，这和政府类似，AUTOSAR划分为上下很多层级，每层上又有很多模块，这就和政府机构一样，层级越多，职能划分的越细，那么需要的人越多，你以为这些人会白干活而不给发工资的的吗？但是AUTOSAR又在其架构文档里说了，我知道你们这些零部件整车厂要省成本，没问题，我提出了３个实现等级（Implementation Conformance Classes－ICC）,其中ICC1即只要求提供运行时环境RTE，如下图所示，够简单吧。但经验告诉我，扯犊子，AUTOSAR RTE是头大象，绝非你想象的那么简单，所有的接口再封装其本质已经是资源的消耗。

![autosar-architecture-icc1.png](/as/images/rewoa/autosar-architecture-icc1.png)

那么，一般而言，汽车电子零部件供应商是使用怎样的架构来节约成本的呢？其实思想很重要，在嵌入式里你是怎样看待你要控制的对象和你要采集的输入，目前我所认知和接触到的思维方式有２种，面向数据（DATA）的软件架构和面向输入输出（Input/Output - IO）的软件架构。

面向数据既是说，所有的一切皆是数据，数据有其属性（如布尔型，整数型，数组型等，可读和可写等），例如一个控制器管脚高低电瓶的输入即可抽象为只可读布尔型数据，又如一个控制器模拟量输入ADC可抽象为只可读整数型数据，自然一个管脚的高低电平输出即为可读写布尔型数据，一个可调制脉冲输出即为可读写数组型（周期和占空比）等。那么对应用程序而言，其所关心的既是数据，其行为即转变为对一些数据的读入处理之后输出一些数据，这样应用工程师不需要具体关心一个数据怎么来的怎么出去的，由架构师安排响应人员在底层封装提供这些数据的操作接口即可，一个典型的架构图如下图所示：

```c
|-------------------------------|
|       Application             |
|-------------------------------|
|    Middle Layer Data          |
|-------------------------------|
|    Low Layer Data(Driver)     |
|-------------------------------|
```

对于面向输入输出的软件架构，网上已经是各种接受，各种什么IO大会，其实所有的一切既是输入输出。即应用拿到输入数据，做出处理后，给一个输出，相当好理解。

```c
|----|    |----|    |----|
| I  |    |    |    | O  |
| N  |    | A  |    | U  |
| P  |--> | P  |--> | T  |
| U  |    | P  |    | P  |
| T  |    |    |    | U  |
|    |    |    |    | T  |
|----|    |----|    |----|
```

好吧，不敢太详细的讲解，到此为止，还是去研究AUTOSAR吧。站在单个微控制器ECU的角度来看，的确不觉得的有多大优势，所以重点来了，我很欣赏的AUTOSAR的一个思想，虚拟总线(VFB-Virtual Function Bus),其终极目标是要将车上所有ECU通过VFB虚拟为好像就一个ECU似得，对应用而言，呵呵，我才不管我运行在哪呢，我只要知道我运行在RTE之上就好，如下图所示，正因此，AUTOSAR其是奔着统一和控制整车所有ECU的目的而去的。个人的观点是，汽车为追求高舒适性和更好的驾驶体验，会有越来越多的功能增加，ECU控制器的数量也与日俱增，目前国内低端车ECU的数量一般在10到20个左右，高端车其数量可达到百位数，可见整车是越来越复杂，整车的开发将越来越复杂，所以整车厂提出了AUTOSAR来解决这个问题，以VFB/RTE这样的抽象方式，使应用软件的开发变得简单，甚至说有朝一日，零部件供应商负责提供ECU控制器RTE运行时环境就好，整车厂自己来规划和开发上层应用，并将应用SWC部署到合适的ECU上。我个人觉得面对日益复杂的车身汽车电子，整车厂希望掌握上层应用软件的开发，从而缩短开发周期，提供更好的功能。另外，当每个零部件厂商都使用AUTOSAR软件架构后，整车厂可以轻易的出于各种目的换掉某个ECU。

![VFB](http://www.autosar.org/fileadmin/_processed_/csm_AUTOSAR_TechnicalOverview_80b6ce5e82.jpg)