---
layout: post
title: AUTOSAR BOOTLOADER
category: real embeded world of automotive
comments: true
---

在汽车电子，现在ｂｏｏｔｌｏａｄｅｒ基本已成汽车ＥＣＵ必备，其负责应用程序的启动和通过ＣＡＮ总线来更新应用程序和数据（如ＮＶＭ）等。

一般而言，ｂｏｏｔｌｏａｄｅｒ通过ＣＡＮ总线来更新应用程序，有些也会通过ＬＩＮ或者ＵＡＲＴ来更新程序，不管通过同种方式，都可以基于[ｉｓｏ１４２２９](https://en.wikipedia.org/wiki/Unified_Diagnostic_Services)来实现，也就是说，ｉｓｏ１４２２９　ＵＤＳ协议算是一种上层应用协议，可用在多种通讯总线之上，而[ｉｓｏ１５７６５](https://en.wikipedia.org/wiki/ISO_15765-2)是基于ＣＡＮ总线的ＵＤＳ协议服务，其定义了如何通过ＣＡＮ总线来传输大数据。

这里只讨论基于ＣＡＮ总线的ｂｏｏｔｌｏａｄｅｒ，说简单点，ｂｏｏｔｌｏａｄｅｒ的主要功能是负责应用程序的启动和程序的更新，而更新过程由一系列的ＵＤＳ服务组合起来实现的，主要由以下８条服务组成。OK,说到这里顺便提一下，目前已经有一个很成熟的ｂｏｏｔｌｏａｄｅｒ规范叫[ＨＩＳ](http://portal.automotive-his.de)，其基于ＡＵＴＯＳＡＲ，对一个ｂｏｏｔｌｏａｄｅｒ该有的元素都做了很详细的定义，有兴趣的可以去了解下，看看他们的文档，不过最近不知道为什么，其官网访问不了，估计又被屏蔽了吧，哎，在党国不会翻墙，ｇｏｏｇｌｅ会永远使用不了，百度还是个渣，郁闷。

##１　Ｓｅｓｓｉｏｎ　Ｃｏｎｔｒｏｌ（会话控制　０ｘ１０）

##1.1 Request message definition

<table>
  <thead>
    <tr>
      <th>索引</th>
      <th>参数名称</th>
      <th>１６进制值</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>#1</td>
      <td>Diagnostic Session Control Request Serivce ID</td>
      <td>10</td>
    </tr>
    <tr>
      <td>#2</td>
      <td>sub-function=[diagnostic session type]</td>
      <td>00-FF</td>
    </tr>
  </tbody>
</table>

##1.2 Request message sub-function parameter $Level (LEV_) definition

<table>
  <thead>
    <tr>
      <th>１６进制值(6位)</th>
      <th>描述</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>０１</td>
      <td>默认会话层(Ｄｅｆａｕｌｔ　Ｓｅｓｓｉｏｎ)</td>
    </tr>
    <tr>
      <td>０２</td>
      <td>编程会话层(Ｐｒｏｇｒａｍｉｎｇ　Ｓｅｓｓｉｏｎ)<br>　这个就是ｂｏｏｔｌｏａｄｅｒ主要要实现的一个会话层，在该会话层下要实现一系列的诊断服务，从而形成一个为升级应用程序提供服务的服务器，ＰＣ或手持客户机即可通过合法访问该会话层下的服务来实现应用程序的更新。<br></td>
    </tr>
    <tr>
      <td>０３</td>
      <td>扩展会话层(Ｅｘｔｅｎｄｅｄ　Ｓｅｓｓｉｏｎ)</td>
    </tr>
  </tbody>
</table>

#1.3 Positive response message

<table>
  <thead>
    <tr>
      <th>索引</th>
      <th>参数名称</th>
      <th>１６进制值</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>#1</td>
      <td>Diagnostic Session Control Response Serivce ID</td>
      <td>50</td>
    </tr>
    <tr>
      <td>#2</td>
      <td>diagnostic session type</td>
      <td>00-7F</td>
    </tr>
    <tr>
      <td>#3 .. #n</td>
      <td>sessionParameterRecord[] #1 = [data1 .. datan]</td>
      <td>00-FF</td>
    </tr>
  </tbody>
</table>

一般而言，当ＥＣＵ上电后，首先执行的是其类似于BIOS的一段程序，该程序一般由芯片厂商设计并不可改写，其会加载后续客户应用程序，对于汽车级ＥＣＵ，其一般为ｂｏｏｔｌｏａｄｅｒ，ｂｏｏｔｌｏａｄｅｒ首先会检查是否有更新程序请求，如果没有，其将检查应用程序的有效性，如果有效，其将启动应用程序。在应用程序运行过程中，如果有客户机通过合法途径（如通过了安全访问）请求进入编程会话层，则应用程序会写一个标记，根据ＨＩＳ规范，该标记会写到ＥＥＰＲＯＭ里面，这样在应用程序执行复位操作之后，ｂｏｏｔｌｏａｄｅｒ会去ＥＥＰＲＯＭ里面读取该标记，即知有更新程序请求，从而停留在ｂｏｏｔｌｏａｄｅｒ，启动编程服务器（Ｓｅｒｖｅｒ），提供一系列的应用程序更新相关的诊断服务给客户机（Ｃｌｉｅｎｔ）来实现程序的升级操作。但是，使用ＥＥＰＲＯＭ来保存升级请求的方式，是开销比较大的，并且也使应用程序的启动时间变长，毕竟要去通过ＳＰＩ／ＩＩＣ等总线去ＥＥＰＲＯＭ读取数据，即使是内部ＥＥＰＲＯＭ数据，也会是比较慢的。所以一般会使用ＭＣＵ内部在不掉电情况下，既使软件复位其值也不会丢失的内存或者寄存器来存储应用程序更新请求标记，相当的简单快捷。另外，在ｂｏｏｔｌｏａｄｅｒ启动过程中，如果检测到应用程序无效，其也会停留在ｂｏｏｔｌｏａｄｅｒ，启动服务器，等待客户机的连接并更新程序。

##２　Ｓｅｃｕｒｉｔｙ　Ａｃｃｅｓｓ（安全访问　０ｘ２７）

好吧，我又犯懒癌了，用ＭＡＲＫＤＯＷＮ的ＨＴＭＬ方式来写表格是一件好麻烦的事情，所以就不在描述消息的格式了。网上搜索ｉｓｏ１４２２９下载其文档看吧。对于ｂｏｏｔｌｏａｄｅｒ安全访问是必须的，从而保证应用程序不被非法客户机给破坏了。安全访问一般分为２步，第一步请求种子（Ｒｅｑｕｅｓｔ　Ｓｅｅｄ）和发送钥匙(Ｓｅｎｄ　Ｋｅｙ)。一般，请求的种子为４字节，是由Ｓｅｒｖｅｒ根据某种特殊算法随机生成，每次访问都会不一样，以防止恶意客户机找到规律给破解了。客户机收到种子之后，经过双方约定的算法来解密，算出钥匙，然后发送给服务器，如果钥匙有效则解锁成功，和此次解锁相关的服务就可以访问了。

目前，一般而言每个会话层就一个安全等级。但ＡＵＴＯＳＡＲ　ＤＣＭ的描述不限于此，同一会话层下，可以有好多个安全等级，举例来说某些服务需要使用安全等级１的算法来解锁，又有其他一些服务需要安全等级２来访问，等等，这里要说每个等级没有高低之分，他们都是相等权重的，但等级０意味着不需要解锁，因为这是默认服务器的安全等级，该安全等级下的服务可以随意访问。

对于安全等级ｎ，其访问数据格式如下：

* 客户机请求种子：　[２７，２ｎ＋１］

* 服务器返回种子：　[６７，２ｎ＋１，ＸＸ，ＸＸ，ＸＸ，ＸＸ］

* 客户机发送钥匙：　[２７，２ｎ＋２，ＹＹ，ＹＹ，ＹＹ，ＹＹ］

* 服务器解锁成功：　[６７，２ｎ＋２］

##３　Ｒｏｕｔｉｎｅ　Ｃｏｎｔｒｏｌ（过程控制　０ｘ３１）

在ｂｏｏｔｌｏａｄｅｒ里过程控制３１服务一般用于应用程序和非易失性数据ＮＶＭ存储空的擦出操作。每一个Ｒｏｕｔｉｎｅ服务都有其唯一的标识符ＩＤ，其请求数据格式如下表所示：
<table>
  <thead>
    <tr>
      <th>索引</th>
      <th>参数名称</th>
      <th>１６进制值</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>#1</td>
      <td>RoutineControl Request Service Id</td>
      <td>３１</td>
    </tr>
    <tr>
      <td>#2</td>
      <td>sub-function = [ routineControlType: 01-startRoutine;02-stopRoutine;03-requestRoutineResults ]</td>
      <td>00-FF</td>
    </tr>
    <tr>
      <td>#3 #4</td>
      <td>routineIdentifier [] = [byte#1 (MSB) byte#2 ]</td>
      <td>00-FF</td>
    </tr>
    <tr>
      <td>#5 .. #n</td>
      <td>routineControlOptionRecord [] = [OptionRecord#1 .. OptionRecord#m ]</td>
      <td>00-FF</td>
    </tr>
  </tbody>
</table>

例如在我ＡＳ仓库里实现的[ｂｏｏｔｌｏａｄｅｒ](https://github.com/parai/as/blob/master/com/as.tool/lua/script/flashloader.lua)， 标识符０ｘＦＦ０１用来请求应用程序存储空间Ｆｌａｓｈ和非易失性数据ＮＶＭ存储空间ＥＥＰＲＯＭ的擦出操作，其格式如下：

<table>
  <thead>
    <tr>
      <th>索引</th>
      <th>参数名称</th>
      <th>１６进制值</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>#1</td>
      <td>RoutineControl Request Service Id</td>
      <td>３１</td>
    </tr>
    <tr>
      <td>#2</td>
      <td>sub-function = [ routineControlType: 01-startRoutine，我在实现时只支持ｓｔａｒｔ，即得到肯定回复时，必然擦除成功 ]</td>
      <td>00-FF</td>
    </tr>
    <tr>
      <td>#3 #4</td>
      <td>routineIdentifier [] = [FF 01 ]</td>
      <td>00-FF</td>
    </tr>
    <tr>
      <td>#5 #6 #7 #8</td>
      <td>address [ byte#1 (MSB) byte#2 byte#3 byte#4 ]</td>
      <td>00-FF</td>
    </tr>
    <tr>
      <td>#9 #10 #11 #12</td>
      <td>length [ byte#1 (MSB) byte#2 byte#3 byte#4 ]</td>
      <td>00-FF</td>
    </tr>
    <tr>
      <td>#13</td>
      <td> 内存标识：　０ｘＦＦ－Ｆｌａｓｈ；０ｘＥＥ－ＥＥＰＲＯＭ</td>
      <td>00-FF</td>
    </tr>
  </tbody>
</table>

##４　Ｒｅｑｕｅｓｔ　Ｄｏｗｎｌｏａｄ（请求下载　０ｘ３４）

在ｂｏｏｔｌｏａｄｅｒ里请求下载服务用于告诉服务器，客户机即将下载一段程序或者数据到客户机，对于我在ＡＳ仓库的一个实现，其请求数据格式如下：
<table>
  <thead>
    <tr>
      <th>索引</th>
      <th>参数名称</th>
      <th>１６进制值</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>#1</td>
      <td>RequestDownload Request Service Id</td>
      <td>３４</td>
    </tr>
    <tr>
      <td>#2</td>
      <td>dataFormatIdentifier：我使用０ｘ００，记没有任何加密以及压缩</td>
      <td>00-FF</td>
    </tr>
    <tr>
      <td>#3</td>
      <td>addressAndLengthFormatIdentifier：我使用０ｘ４４，即地址和长度信息都为４个字节</td>
      <td>00-FF</td>
    </tr>
    <tr>
      <td>#４ #５ #６ #７</td>
      <td>address [ byte#1 (MSB) byte#2 byte#3 byte#4 ]</td>
      <td>00-FF</td>
    </tr>
    <tr>
      <td>#8 #9 #10 #11</td>
      <td>length [ byte#1 (MSB) byte#2 byte#3 byte#4 ]</td>
      <td>00-FF</td>
    </tr>
    <tr>
      <td>#12</td>
      <td> 内存标识：　０ｘＦＦ－Ｆｌａｓｈ；０ｘＥＥ－ＥＥＰＲＯＭ；０ｘＦＤ-Flash Driver</td>
      <td>00-FF</td>
    </tr>
  </tbody>
</table>

如上表中第１２个字节参数内存标识是我添加的，由于ＡＲＣＣＯＲＥ　ｖ３．１源代码并没有实现３４／３５／３６／３７服务，所以我在其代码的基础上增加了这些服务，为使代码简单但并复用其ＤＣＭ　ｍｅｍｏｒｙ管理机制，我选择使用内存标识的方法来区分不同内存快，而不是使用地址空间来区分，虽然和标准ｉｓｏ１４２２９有些许的不一致，但也无妨，我这里只为基于ＡＵＴＯＳＡＲ实现一个简单的示例ｂｏｏｔｌｏａｄｅｒ，可用我开发的[ｅａｓｙＳＡＲ图形配置工具](https://github.com/parai/as/blob/master/com/as.tool/config.infrastructure.system/studio.py)打开文件[boot/common/autosar.arxml](https://github.com/parai/as/blob/master/com/as.infrastructure/boot/common/autosar.arxml)来了解所有关于ｂｏｏｔｌｏａｄｅｒ诊断的配置信息。

##５　Ｒｅｑｕｅｓｔ　Ｕｐｌｏａｄ（请求上载　０ｘ３５）
该服务和下载服务差不多，唯一的不同是其是为了请求从服务器Ｓｅｒｖｅｒ端读取应用程序或者非易失性数据ＮＶＭ到客户机，对于我在ＡＳ仓库的一个实现，其请求数据格式如下，基本和下载服务一样：
<table>
  <thead>
    <tr>
      <th>索引</th>
      <th>参数名称</th>
      <th>１６进制值</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>#1</td>
      <td>RequestUpload Request Service Id</td>
      <td>３5</td>
    </tr>
    <tr>
      <td>#2</td>
      <td>dataFormatIdentifier：我使用０ｘ００，记没有任何加密以及压缩</td>
      <td>00-FF</td>
    </tr>
    <tr>
      <td>#3</td>
      <td>addressAndLengthFormatIdentifier：我使用０ｘ４４，即地址和长度信息都为４个字节</td>
      <td>00-FF</td>
    </tr>
    <tr>
      <td>#４ #５ #６ #７</td>
      <td>address [ byte#1 (MSB) byte#2 byte#3 byte#4 ]</td>
      <td>00-FF</td>
    </tr>
    <tr>
      <td>#8 #9 #10 #11</td>
      <td>length [ byte#1 (MSB) byte#2 byte#3 byte#4 ]</td>
      <td>00-FF</td>
    </tr>
    <tr>
      <td>#12</td>
      <td> 内存标识：　０ｘＦＦ－Ｆｌａｓｈ；０ｘＥＥ－ＥＥＰＲＯＭ；０ｘＦＤ-Flash Driver</td>
      <td>00-FF</td>
    </tr>
  </tbody>
</table>

##６　Ｔｒａｎｓｆｅｒ　Ｄａｔａ（传输数据　０ｘ３６）

数据传输服务则为实现应用程序数据由客户机到服务器或者由服务器到客户机的传输过程，取决于客户机请求的是何种传输模式，下载（３４）模式还是上载（３５）模式，如果是下载，其客户机请求服务格式如下：
<table>
  <thead>
    <tr>
      <th>索引</th>
      <th>参数名称</th>
      <th>１６进制值</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>#1</td>
      <td>TransferData Request Service Id</td>
      <td>３6</td>
    </tr>
    <tr>
      <td>#2</td>
      <td>blockSequenceCounter</td>
      <td>00-FF</td>
    </tr>
    <tr>
      <td>#3</td>
      <td>reserved</td>
      <td>00</td>
    </tr>
    <tr>
      <td>#４</td>
      <td>内存标识：　０ｘＦＦ－Ｆｌａｓｈ；０ｘＥＥ－ＥＥＰＲＯＭ；０ｘＦＤ-Flash Driver</td>
      <td>00-FF</td>
    </tr>
    <tr>
      <td>#5 ... #n </td>
      <td>data [ byte#1 ... byte#m ]</td>
      <td>00-FF</td>
    </tr>
  </tbody>
</table>

由上表所示，为了简化ｂｏｏｔｌｏａｄｅｒ程序并保证数据ｄａｔａ的起始地址能够４字节对齐，所以我增加了参数＃３和＃４。当服务器正确响应请求并将数据写入相应内存之后，服务器会给出如下响应：
<table>
  <thead>
    <tr>
      <th>索引</th>
      <th>参数名称</th>
      <th>１６进制值</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>#1</td>
      <td>TransferData Response Service Id</td>
      <td>76</td>
    </tr>
    <tr>
      <td>#2</td>
      <td>blockSequenceCounter</td>
      <td>00-FF</td>
    </tr>
  </tbody>
</table>

如果是上载数据，其客户机请求格式如下表所示：
<table>
  <thead>
    <tr>
      <th>索引</th>
      <th>参数名称</th>
      <th>１６进制值</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>#1</td>
      <td>TransferData Request Service Id</td>
      <td>３6</td>
    </tr>
    <tr>
      <td>#2</td>
      <td>blockSequenceCounter</td>
      <td>00-FF</td>
    </tr>
    <tr>
      <td>#3</td>
      <td>reserved</td>
      <td>00</td>
    </tr>
    <tr>
      <td>#４</td>
      <td>内存标识：　０ｘＦＦ－Ｆｌａｓｈ；０ｘＥＥ－ＥＥＰＲＯＭ；０ｘＦＤ-Flash Driver</td>
      <td>00-FF</td>
    </tr>
  </tbody>
</table>
当服务器正确响应请求并读取相应数据之后，服务器会给出如下响应：
<table>
  <thead>
    <tr>
      <th>索引</th>
      <th>参数名称</th>
      <th>１６进制值</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>#1</td>
      <td>TransferData Response Service Id</td>
      <td>76</td>
    </tr>
    <tr>
      <td>#2</td>
      <td>blockSequenceCounter</td>
      <td>00-FF</td>
    </tr>
    <tr>
      <td>#３ ... #n </td>
      <td>data [ byte#1 ... byte#m ]</td>
      <td>00-FF</td>
    </tr>
  </tbody>
</table>

好吧，个人认为还是相当简单的，基本就是一问一答，按照固定的模式将下载的数据写入存储空间或者将将相应数据读取出来返回给客户机。

##７　Ｒｅｑｕｅｓｔ　Ｔｒａｎｓｆｅｒ　Ｅｘｉｔ（请求传输结束　０ｘ３７）

见名知意，请求一个传输过程的结束。即是说一个完整的过程分为三步，第一步是请求上载或者下载，第二步不断的数据传输直至结束，第三步请求传输结束。

##８　Ｅｃｕ　Ｒｅｓｅｔ（ＥＣＵ复位　０ｘ１１）

呵呵，这个更简单了，请求ＥＣＵ复位，当应用程序更新完完毕时，重启以启动更新后的应用程序。


Ｏｋａｙ，以上介绍了一些ｂｏｏｔｌｏａｄｅｒ关键性的诊断服务，当然还会有一些其他服务如什么写Ｆｉｎｇｅｒ　Ｐｒｉｎｔ啊，应用程序完整性校验啊什么的，但我就不管了，只要掌握思想原理，随你客户需求怎么提，Ｉ　ｄｏｎ＇ｔ　ｃａｒｅ，一切都只是时间和工作量的问题。下面就简单将上述服务串起来，讲述一个完整的ｂｏｏｔｌｏａｄｅｒ应用程序升级过程。

１　当前应用程序正在执行，客户机经过安全访问之后，取得了进入ｐｒｏｇｒａｍ　ｓｅｓｓｉｏｎ的权限，然后请求进入ｐｒｏｇｒａｍ　ｓｅｓｓｉｏｎ，之后应用程序写一个程序更新请求标识到ＮＶＭ或者软件复位其值不会丢失的内存ＲＡＭ或者寄存器中，并执行软件复位（可利用开门狗来复位）。

２　ｂｏｏｔｌｏａｄｅｒ开始执行，检测到程序更新请求标识，启动Ｓｅｒｖｅｒ，等待客户机后续响应。

３　客户机通过安全访问Ｓｅｃｕｒｉｔｙ　Ａｃｃｅｓｓ，获取其他被保护服务的访问权限，解锁成功。

４　客户机请求下载Ｆｌａｓｈ　Ｄｒｉｖｅｒ到可执行ＲＡＭ空间。这里说明下，出于安全行考虑，一般ｂｏｏｔｌｏａｄｅｒ的Ｆｌａｓｈ驱动都会通过动态下载的方式。当然，有些ｂｏｏｔｌｏａｄｅｒ会选择内联Ｆｌａｓｈ　Ｄｒｉｖｅｒ，这样，这一步操作就可以省略了。内联Ｆｌａｓｈ　Ｄｒｉｖｅｒ的隐患是当程序一旦跑飞，并执行了Ｆｌａｓｈ　Ｄｒｉｖｅｒ的ＡＰＩ可能会破坏应用程序和ｂｏｏｔｌｏａｄｅｒ使设备变砖。

５　客户机请求擦除应用程序存储空间以来更新程序。

６　客户机下载应用程序。

７　客户机执行ＥＣＵ　Ｒｅｓｅｔ，升级完成。

看吧，还是很简单的。

