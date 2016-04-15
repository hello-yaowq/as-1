---
layout: post
title: AUTOSAR通讯和诊断
category: real embeded world of automotive
comments: true
---

在整个AUTOSAR架构里，我觉得重点在于通讯和诊断服务，因为我认为VFB虚拟总线是AUTOSAR的核心。AUTOSAR通过抽象层，为不同的通讯方式如CAN/LIN/FlexRay/Ethernet等提供了统一的上层接口。目前国内外主流的通讯媒介是CAN，少数ECU的从机使用LIN来通讯。来一张图先看看AUTOSAR的CAN通讯栈。

![autosar-can-stack.png](/as/images/rewoa/autosar-can-stack.png)
<center> 图1 AUTOSAR CAN 协议栈 </center>

从图上看通讯可划分为3类： 应用报文COM（Application Purpose），诊断报文DCM（Diagnostic Purpose）和 网络管理报文NM（Network Management Purpose）。其中有个什么Large Data COM对我来说是个新东西，还没研究过，看名字估计用作大数据消息的传输，简单看了下文档，也确实如此，但在以前做产品的过程中接触的不是很多，所以对其不做评论。

对于CAN通讯，个人认为还是比较简单的，CAN报文属于明文广播传输，在同一个总线上的任何节点都可以收到其他节点发送来的报文，但是，由于总线上消息可能很多，有些是本节点不需要关心的，所以会采用硬件过滤的方式，只接收那些本节点所关心的报文。对于同一个ID的报文，同一总线上只能有一个节点上发送该报文。另外，每个节点该发送么报文和该接受什么报文，一般由整车厂来定义，并将与零部件节点相关的那部分分发给供应商，当然有些整车场会将所有的报文定义都给你，这份定义文档称作CAN通信矩阵(CAN Matrix),其严格定义了每个ID报文的长度(DLC<=8)，一般为８字节,报文里所含消息(signal)的格式和其意义。由于Vector CAN工具的广受欢迎，现在整车厂一般会提供一个Vector DBC格式文件供零部件供应商使用，现在有些零部件供应商已经基本都有一套成熟的工具直接导入该DBC文件直接生成CAN协议栈的配置文件。

在我的个人仓库AS中，就有２个DBC解析和导入工具，一个是[AS.xlsm](https://github.com/parai/as/blob/master/com/as.tool/config.infrastructure.system/AS.xlsm)微软办公软件Excel,使用VBA宏解析与导入DBC文件，另一个是利用python实现的一个解析工具[ascc](https://github.com/parai/as/tree/master/com/as.tool/py.can.database.access/ascc)，所以有兴趣的人可以基于以上２个工具进行二次开发，开发出和你正在使用的CAN协议栈配置文件生成工具。

这里以一张截图展示下我的工具[AS.xlsm](https://github.com/parai/as/blob/master/com/as.tool/config.infrastructure.system/AS.xlsm)，其可以很方便的导入Vector DBC文件,但目前的功能仅限于此，不支持其他生成AUTOSAR COM配置文件的功能，当初太懒了，没做，还有就是实在没有python来的简单快捷，所以放弃了使用EXCEL来生成AUTOSAR BSW配置文件的想法，但这个工具依然保留着以备不时之需。

![as-can-xlsm.png](/as/images/rewoa/as-can-xlsm.png)
<center> 图２ AUTOSAR CAN Excel Tool </center>

图２为导入Vector CANoe Demmo PowerTrain.dbc之后的样子，每一个CAN报文都有自己的ID,且各不相同，每一个报文可含有多个信号，如ABSData,图３为使用Vector DBC Editor打开该DBC文件后，报文ABSData的一个信号布局图。从而，收到报文ID为201（0xC9）的报文，ECU就知道该报文为ABSData并知道其数据场的前12位表征信号车速（CarSpeed），第15位表征信号齿轮锁（GearLock）等。好吧，也就是说，对于AUTOSAR的COM（Application Purpose）,其是根据整车厂提供的CAN通讯矩阵高度定制的，对同一总线上每个ECU来说其COM模块的配置文件是不一样的。

![as-can-dbc-absdata.png](/as/images/rewoa/as-can-dbc-absdata.png)
<center> 图3 AUTOSAR CAN报文ABSData </center>

说到这里就简单说下，常用的CAN控制器是个什么样子的。一般来说，主流的CAN控制器会有N个接受报文箱（RX MESSAGE BOX）和M个发送报文箱（TX MESSAGE BOX），一般而言，在做CAN的配置时，会按照CAN ID由小到大的顺序来依次配置RX MESSAGE BOX,设置每个RX MESSAGE BOX的过滤器(filter)让其只接收特定CAN ID的报文，如上图2所示，可以配置第1个RX MESSAGE BOX只接收报文Ignition_Info,可配置第2个RX MESSAGE BOX接收ID范围为0x500至0x5FF的NM网络管理报文(一般而言，NM网路管理报文为一个连续地址范围)，可以配置第3个RX MESSAGE BOX只接收报文GearBoxInfo,以此类推。一般而言COM和DCM报文对应一个唯一的RX MESSAGE BOX,只有在报文太多的情况下，会酌情考虑将ID相近的报文使用同一个RX MESSAGE BOX来接受。对于发送报文，与接受相似，但是，一般而言Ｍ要小于Ｎ很多，所以一般情况下会将所有TX MESSAGE BOX链接在一起使用。当然还有一些CAN控制器的的MESSAGE BOX即可用作接收也可用作发送，但配置方法基本和前述一样。

至此，简单介绍了AUTOSAR的CAN应用报文COM，下面再来介绍网络管理NM。第一次接触CAN网络管理NM这个名词时，我认为其作用应该是网络状态监听和网络负载管理，但实际却不是这样（个人观点），从某种意义上讲，我甚至认为网络管理报文可有可无，没有什么特别大的作用，以我过去在VISTEON的经验，网络管理报文的主要作用是用来同步同一CAN总线上各节点的状态，使各节点能够做到同时唤醒，并在各节点都睡眠条件满足的情况下同时休眠。所以，OSEK NM对此提出了２种网络管理方式：直接网络管理和间接网络管理。[AUTOSAR NM](http://www.autosar.org/fileadmin/files/releases/4-2/software-architecture/communication-stack/standard/AUTOSAR_SWS_CANNetworkManagement.pdf)则认为属于直接网络管理。

以我个人经验，直接网络管理报文即有专门的CAN报文用来同步各节点状态，而间接网路管理报文则没有专门的CAN报文从而间接地使用各节点的某个应用报文（或某个应用报文的某个消息）来同步各节点状态，根据OSEK NM文档的描述，一般建议在CAN网络负载较高的情况下，为减轻网络负载，使用这种方式。由于第２种间接网络管理高度定制，所以没有什么大的共性，所以就直接不讲了，还有就是没经验，我也不是很懂啊。

所以就简单介绍下OSEK DIRECT NM和AUTOSAR CAN NM吧，不过说实话，他们的文档讲的真的太详细了，看看就懂了。如图３所示，每个节点会有一个专有的网络管理报文，比如说A节点ID为１（其对应网络管理报文CAN ID为0x501）,B节点ID为2（其对应网络管理报文CAN ID为0x502）,C节点ID为3（其对应网络管理报文CAN ID为0x503）,呵呵，一般来说，同一总线上网络管理报文为同一连续网段，如从0x500到0x5FF,则可支持256网络节点，不过一般不会有那么多，顶多10(国内)多个吧。根据OSEK NM定义，NM报文总是从小ID节点发往较大ID节点，至最大节点ID重新发往最小ID节点。这就好像一个班的学生去上足球课，所有学生七嘴八舌向老师喊报道后（Alive Message），老师让他们按身高由高到低围成一个圈，然后由个最矮的那个将球踢给第２矮的人，然后第２矮的人将球踢给比他高一点的人直到最高个，由最高个在踢给最小个，并且老师说了踢球间隔控制在２s以内，并且要准（不要跟我讲有人身高一样，这里就假设都不一样）,这个就是正常情况下的Ring Message。这个时候，一个磨磨唧唧总迟到的人来了，像老师喊了生报道(Alive Message),请求加入队伍，老师批准了，然后该人迅速找准位置，加入圆圈，开始正常的传球(Ring Message)。然后老师又说当大家都喊累了的时候停止传球，解散休息，当有任何一个人喊不累，即使其他人都喊累，也得都给我继续。所以当大家累了之后，一个接着一个开始喊累了要休息(Ring Message with Sleep Indication)时，球得继续踢，直到最后一个说我也累了大家解散休息吧（Ring Message with Sleep Acknoledge）,这时大家就可以解散了（注意，老师规定只能在你传球的时候喊出累或者不累，不发声就是不累）。当然和OSEK NM机制会有些差别，不过规则基本很像上述足球课的例子，关于limphome什么的，大家自己看文档去脑补吧。

![osek-nm-2-bus-ring-example.png](/as/images/rewoa/osek-nm-2-bus-ring-example.png)
<center> 图３ 2个CAN总线上OSEK NM逻辑环示例 </center>

关于AUTOSAR CAN NM，读其文档，我是感觉要比OSEK NM简单，并且其号称其总线负载更低，个人只认为在总线唤醒的那一刻，AUTOSAR NM比OSEK NM的总线负载率要低，因为AUTOSAR CAN NM不需要喊报道，没有Alive Message。AUTOSAR CAN NM只有周期性消息，只要该节点不想休眠，其就可以周期性的发送网络管理报文，为了降低负载，AUTOSAR NM使用一种特殊的方式来达到此目的，即每个节点的有一个小周期(reduced period)和一个正常周期（normal period）,小周期每个节点不相同，但正常周期所有节点一样，并且小周期一定要小于正常周期。

![autosar-cannm-reduce-bus-load.png](/as/images/rewoa/autosar-cannm-reduce-bus-load.png)
<center> 图４　AUTOSAR CAN NM 示例 </center>

如图４所示，节点正常周期为７０ms，节点１小周期为４０ms，节点２小周期为５０ms，节点３小周期为６０ms。根据AUTOSAR CAN NM的规则，节点发送网络管理报文后，下一次发送网络管理报文的时间为正常周期结束之后；节点收到其他节点发送的报文后，节点需将下一次发送网络管理报文的时间设置为小周期结束之后。则如图４所示节点１发送网路管理报文，则节点１在７０ms后才会第２次发送网络管理报文，由于收到报文，节点２将在５０ms之后发送报文，节点３将在６０ms后发送报文。所以当然节点２在５０ｍｓ超时后率先发送出报文，这时节点２将在７０ｍｓ后才会发送报文，由于收到报文，节点１将在４０ms之后发送报文，节点３将在６０ms后发送报文。所以当然节点１在４０ｍｓ超时后再次发送出报文。从而如果节点１／２／３都不休眠，则总线上将始终是节点１和节点２分别以４０ｍｓ和５０ｍｓ的周期发送网络管理报文，而节点３因为小周期太大将没有机会发送网络管理报文。所以说AUTOSAR CAN NM只有小周期最小的那两个节点有机会发送网络管理报文，其他节点只能默默监听这网络状态。所以可能有人会问，我了个去，那我怎么知道那个节点还在不在呢，那我只能告诉你，监听他的周期性应用报文吧。

接着就剩下诊断报文了，说到此，虽然诊断很简单，但是两个标准协议必须了解，[ｉｓｏ１５７６５](https://en.wikipedia.org/wiki/ISO_15765-2)（ＣＡＮ传输协议）和[ｉｓｏ１４２２９](https://en.wikipedia.org/wiki/Unified_Diagnostic_Services)（统一诊断服务）。其中，ｉｓｏ１５７６５定义描述了通过ＣＡＮ报文如何传输多余８字节（但小于等于４０９５字节）的数据。这个就不想介绍了，看看ｗｉｋｉ的介绍基本就能明白是什么意思，还是相当简单的。虽然数据长度可以大于８个字节，一条服务请求是通过多包传输的，但其总的数据格式是预定义的。想简单了解ＵＤＳ诊断服务，可参见后续我的[ｂｏｏｔｌｏａｄｅｒ](http://parai.github.io/as/real%20embeded%20world%20of%20automotive/2016/04/15/REWOA-AUTOSAR-BOOTLOADER.html)的介绍文档。


