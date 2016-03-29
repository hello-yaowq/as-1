---
layout: post
title: AUTOSAR/OSEK OS 与其他常规RTOS的区别
category: real embeded world of automotive
comments: true
---

每一个RTOS都将会有其独特的设计之处，例如ucOS，那个8x8的任务优先级就续表，我想必然惊讶了很多人，那是多么巧妙的一个快速查找最高就绪优先级的方法，FreeRTOS和ucOS的该方法比起来，那真是落后好几条大街。好吧，这里不比较ucOS和FreeRTOS，而是比较OSEK OS和FreeRTOS等的区别。

看过AUTOSAR OS文档的人都会知道，因为OSEK OS的设计思想太好了，所以AUTOSAR就直接借过来用了，也就是说AUTOSAR OS是基于OSEK OS的，而后在其上增加了些特性和需求，如调度表（Schedule Table），内存保护（Memory Protection）以及应用（Application）的概念。本文主要讲解OSKE OS和其他OS的区别。

首先，从OS支持的功能上看，常规OS如FreeRTOS等支持任务（Task）/报警器（Alarm or Timer）/事件（Event）/消息队列（Message）/信号量（Semaphore）/互斥所（Mutex），可见常规OS所支持的功能还是很丰富的。再看OSEK OS，其支持任务（Task）/报警器（Alarm）/资源（Resource）/事件（Event），其中资源和常规任务的互斥锁类似，但资源引入的一个机制，设置资源天花板优先级来解决了优先级翻转和死锁的问题，可以参见OSEK OS 2.2.3文档8.4.1章节，如下是我的翻译(这年头不会英语真的是没法在程序圈混啊)：

![priority_inversion.png](/as/images/rewoa/priority_inversion.png)

![deadlock.png](/as/images/rewoa/deadlock.png)

如上图所示，任务T1获取了信号量S1，之后陷入等待状态，等待某个事件的发生。这时低优先级就绪任务T2将获得CPU使用权，开始执行，其将获取信号量S2，之后，任务T1等待的事件发生了，任务T1将重新获得CPU使用权，并尝试获取信号量S2，其将重新陷入等待状态，因为这时信号量S2已经被任务T2获取了。这时任务T2恢复执行，如果其尝试获取信号量S1，这是任务T1也会陷入等待状态，这时任务T1和任务T2都因等待一个已经被对方所获取的信号量而陷入死锁状态。

这里突然想到了曾经和一个朋友在一起吃饭时，他向我讲述了一件事情，说他去面试时，面试官问了他一个问题，在OS不提供死锁避免机制的前提下，在开发应用程序时如何避免死锁，当我听到这个问题时，我也瞬间懵了，很多时候，我们都在研究什么是死锁，开发RTOS时，该提供哪些机制去避免死锁，却真未曾思考过在开发应用程序时，如何去避免死锁的问题。其实问题是很容易解决的，那就是所有任务都按照一定的顺序去获取资源。如上图所示，若果任务T1和任务T2都按照先获取信号量S1再获取信号量S2的顺序执行，那么这个死锁将不复存在。这是一种很好的在应用程序端避免死锁的方法。另外个人猜测，微软提供API WaitForMultipleObjects 就是出于此目的。但是现实的应用需求可能是复杂的，有些时候，应用程序的逻辑复杂度上去了，导致了不能够保一定按一定顺序去获取资源，那这个时候，就只能够依靠OS来提供机制避免死锁的发生，如ucOS/FreeRTOS的资源等待超时的方法，但此方法却毕竟能力有限，还是依赖应用程序来判断是否超时来决定执行何种操作。但这也不能说ucOS/FreeRTOS不好，更多的是因为ucOS/FreeRTOS不是一个纯静态配置的操作系统，其支持任务/信号量等的动态创建，从而其不能像OSEK OS那样采取设置资源天花板优先级的方法来解决问题，但也折中的提供了改变当前任务优先级的API（如FreeRTOS API vTaskPrioritySet）来实现死锁的避免。

