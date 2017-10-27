---
layout: post
title: The Architecture of ASKAR
category: AUTOSAR
comments: true
---

# 1.ASKAR([Automotive oSeK AutosaR](https://github.com/parai/as/tree/master/com/as.infrastructure/system/kernel/askar)) 

ASKAR is firstly a [OSEK/VDX OS223](http://trampolinebin.rts-software.org/os223.pdf) confirmed RTOS, planed to be support AUTOSAR OS standard laterly if ASKAR really becomes famous. This page will illustrate the design concept of ASKAR.

# 2.Task Scheduler of ASKAR

Scheduler is the most important core component of a RTOS, it decides the schedule policy. The common schedule policy for a RTOS is generally priority based, always serves the high priority task firstly, such as ucOS_II which just allow only one task per priority, and for those RTOS that allow multiply tasks per priority, such as freertos, an additionaly schedule policy is that apply time budget based round-robin policy for tasks with same priority.

And the OSEK/VDX OS223 standard defined a another kind of priority based scheduler, for only one task per priority, the scheduler acting the same with the common RTOS such as ucOS_II, but for multiply tasks per priority, the behavior is different with other common RTOS such as freertos, generally it can be illustrate as the below Figure 2.2. And note that OSEK OS can be divided into 4 conformance class show as Figure 2.1.

![OSEK conformance class](/as/images/rewoa/osek_os_cc_level.png)

* Figure 2.1 OSEK OS Conformance Class

![OSEK scheduler](/as/images/rewoa/osek_os_scheduler_fifo.png)

* Figure 2.2 OSEK OS Scheduler

Seeing the Figure 2.2, most out of common way to implement it is just as what you see that use a priority base FIFO queues which is just I originally implemented in [GaInOS](https://github.com/parai/GaInOS). But really, there is another way that I will tell in chapter 2.1, this is not my idea, I borrow it from trampoline](https://github.com/TrampolineRTOS/trampoline) and re-implement it in ASKAR.

## 2.1 Bubble up/down method based ready queue 

# 3.Alarm & Counter of ASKAR

# 4.Event of ASKAR

# 5.Resource of ASKAR

# 6.Summary
