---
layout: post
title: AUTOSAR/OSEK OS 与其他常规RTOS的区别
category: real embeded world of automotive
comments: true
---

每一个RTOS都将会有其独特的设计之处，例如ucOS，那个8x8的任务优先级就续表，我想必然惊讶了很多人，那是多么巧妙的一个快速查找最高就绪优先级的方法，FreeRTOS和ucOS的该方法比起来，那真是落后好几条大街。好吧，这里不比较ucOS和FreeRTOS，而是比较OSEK OS和FreeRTOS等的区别。

看过AUTOSAR OS文档的人都会知道，因为OSEK OS的设计思想太好了，所以AUTOSAR就直接借过来用了，也就是说AUTOSAR OS是基于OSEK OS的，而后在其上增加了些特性和需求，如调度表（Schedule Table），内存保护（Memory Protection）以及应用（Application）的概念。本文主要讲解OSKE OS和其他OS的区别。

首先，从OS支持的功能上看，常规OS如FreeRTOS等支持任务（Task）/报警器（Alarm or Timer）/事件（Event）/消息队列（Message）/信号量（Semaphore）/互斥所（Mutex），可见常规OS所支持的功能还是很丰富的。再看OSEK OS，其支持任务（Task）/报警器（Alarm）/资源（Resource）/事件（Event），其中资源和常规任务的互斥锁类似，但资源引入的一个机制，设置资源天花板优先级来解决了优先级翻转和死锁的问题，可以参见OSEK OS 2.2.3文档8.4.1章节，如下是我的翻译：

![priority_inversion.png](/as/images/rewoa/priority_inversion.png)

![deadlock.png](/as/images/rewoa/deadlock.png)