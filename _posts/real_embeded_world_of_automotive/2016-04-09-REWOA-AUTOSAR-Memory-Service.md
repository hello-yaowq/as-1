---
layout: post
title: AUTOSAR非易失性数据存储服务
category: real embeded world of automotive
comments: true
---

我相信绝大多数的嵌入式系统都需要有一个存储空间来存储一些关键非易失性数据，可选的存储器件那就相当的多了，如TF/SD卡，NAND/NOR FLASH什么的，汽车电子了常用的则为EEPROM和FLASH。

使用EEPROM是一种很常用的方式，一般来说FLASH和EPROM在写数据之前都需要先执行擦出操作，但现在有些EEPROM是不需要擦出操作的，直接发命令写数据就好。但更多的时候，为确保安全在写数据之前，都会执行一个擦写操作，一般对于EEPROM而言，其最小可擦除单元为8到32个字节，而FLASH就会很大,512字节以上。所以这也决定了使用EEPROM更加简单，软件不会很复杂。

例如对于汽车常见需要存储的数据里程信息（总里程Odometer和小计里程Tripmeter）而言，对总里程需要４个字节，对小计里程需要２个字节，２个字节的校验码共８个字节，充分考虑EPROM的使用寿命，可能说固定分配EERPOM的０地址起始处的８个最小可擦除单元供其存储使用，这样软件每次冷启动找到总里程最大值作为当前值，并知道了下一个数据更新的地址，从而轮循使用这８个单元来存储里程信息，软件是何其的简单，也就是说，对EEPROM的使用多为一个数据一个或者多个固定地址的坑。


但是对于FLASH就不那么简单了，最小可擦除单元太大，一个数据一个坑的话，呵呵，基本就不用玩了，因为有些MCU控制器，其内部可能就那么几个FLASH块，并且每个块要被擦的话是整个块被全部擦出，所以像EERPOM那样的那种使用方式已经变得不切实际的了。所以这个时候就要换一种玩法，一般被称作用FLASH来模拟EEPROM,所以在AUTOSAR里就有一个模块叫做Fee(Flash Emulation Eeprom)。这里提一下，有些MCU控制器会告诉你其片内带有EEPROM，同时会注明是用FLASH模拟的，个人觉得可能是MCU控制器实现了一个简单的算法来实现该功能，这里不研究了。

ARCCORE V3.1开源的代码中[Fee](https://github.com/parai/as/blob/master/com/as.infrastructure/memory/Fee/Fee.c),既是AUTOSAR的一个很好实现例子，linus有句话叫做"reading the f**king source code", 不过说实话，第一次读其代码时，我就想说，shit，头疼，所以这里就不讲复杂的代码逻辑，以一张图来描述下其原理吧，如下图所示：

![autosar-fee-mapping.png](/as/images/rewoa/autosar-fee-mapping.png)
