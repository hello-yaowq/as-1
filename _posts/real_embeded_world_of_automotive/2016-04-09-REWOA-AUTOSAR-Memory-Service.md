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

ARCCORE V3.1开源的代码中[Fee](https://github.com/parai/as/blob/master/com/as.infrastructure/memory/Fee/Fee.c),既是AUTOSAR FEE的一个很好实现例子，linus有句话叫做"reading the f**king source code", 不过说实话，第一次读其代码时，我就想说，shit，头疼，所以这里就不讲复杂的代码逻辑，以一张图来描述下其原理吧，如下图所示：

![autosar-fee-mapping.png](/as/images/rewoa/autosar-fee-mapping.png)
<center> 图1 AUTOSAR FEE 原理图 </center>

上图中展示了如何使用２个FLASH块来模拟EEPROM的过程，使用过程中可以看出，总会有一个FLASH块空闲未使用。当然使用３个及其以上的FLASH块来模拟EEPROM的过程其实是相似的。在系统刚准备就绪时，２个FLASH块肯定都是空的，没有任何数据，这个时候软件初始化BANK0,在其底部写上一个特殊的魔数（MAGIC NUMBER）用以标识BANK0的使用开始，因为是空的所以理所当然软件很容易知道下一个要写入的数据块的ID域底部地址和DATA域上部地址。通常而言数据块的ID域至少包含数据编码号和数据地址信息，其大小和结构固定，但数据块的DATA域大小和结构都不固定。图1 Group1展示了在依次更新写入数据块0/数据块1/数据块2和又写入一次数据0之后的样子，可见其是依照先后顺序为每个要更新的数据块动态的分配存储空间的，系统冷启动时，又可依照数据块ID域找到最新的有效数据，还是相当巧妙的方法的，从两头向中间靠拢，虽然每个数据块的数据域大小可能不一样，但由于动态分配，一点也不浪费空间。图1 Group2展现了当ID域和DATA域合拢而在该BANK0内没有多余空间分配给数据块3时，则软体将首先转移所有数据块最新数据至BANK1,然后在写入数据块3,之后如图1 Group3所示BANK0将被擦出供下一次当BANK1内存耗尽时使用。当然，DATA域中可存入校验码(CRC或者checksum)来保证数据完整性。

OK,这里介绍下，在ARCCORE FEE实现时，其魔数为0x2345babe,如下图图３所示，其最后８个字节前半部分即为魔数，后面是个什么呢，看代码：

```c
typedef uint32 BankMagicType;
typedef uint8 FlsBankCounterType;
typedef struct {
	BankMagicType		Magic;
	FlsBankCounterType  BankCounter;
	FlsBankCounterType  InvBankCounter;
} FlsBankControlDataType;
```

则可知该BANK的BankCounter为１，至此一个有效的魔数大概是什么样子我们就知道了，虽然这只是ARCCORE的实现，换做其他厂商可能是另外的的样子，但万变不离其宗。

这个时候就需要了解下，一个标识域ID是个什么样子了，继续看ARCCORE代码：

```c
typedef struct {
	uint16				BlockDataLength; // == 0 if invalidated
	uint16				InvBlockDataLength;
	Fls_AddressType		BlockDataAddress;
} FlsBlockCtrlDataType;		/* 数据块的长度和存储地址信息 */
typedef union {
	FlsBlockCtrlDataType	Data;
	uint8					Byte[BLOCK_CTRL_DATA_PAGE_SIZE];
} FlsBlockCtrlDataPageType;
typedef uint32 BlockMagicType;
typedef struct {
	BlockMagicType		Magic;/* 值为0xebbababe */
	uint16				BlockNo;
	uint16				InvBlockNo;
} FlsBlockCtrlIdType;	/* 数据块的编码号信息 */
typedef union {
	FlsBlockCtrlIdType	Data;
	uint8				Byte[BLOCK_CTRL_ID_PAGE_SIZE];
} FlsBlockCtrlIdPageType;
typedef struct {
	FlsBlockCtrlDataPageType	DataPage;
	FlsBlockCtrlIdPageType		IdPage;
} FlsBlockControlType;
```

代码有点绕，其时标识域ID包含２部分信息，１为数据块的编码号信息（BlockNo），２为数据块的长度和存储地址信息，如下图３所示偏移地址0x3ff0处８字节数据即为数据块的编码号信息，其BlockNo为９。偏移地址0x3fe0的后８字节数据即为数据块的长度和存储地址信息，可知长度为40字节，地址为0,在看图２，你将很清楚的知道该数据块的数据信息，以此类推，可以很容易的查找和遍历所有数据，并可知部分历史数据信息。

![autosar-fee-data-part.png](/as/images/rewoa/autosar-fee-test-data-part.png)
<center> 图2 AUTOSAR FEE FLASH 镜像文件数据域DATA </center>
![autosar-fee-id-part.png](/as/images/rewoa/autosar-fee-test-id-part.png)
<center> 图3 AUTOSAR FEE FLASH 镜像文件标识域ID </center>