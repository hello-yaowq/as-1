---
layout: post
title: AUTOSAR RTE
category: real embeded world of automotive
comments: true
---

俗话说，居安思危，虽然说通过以前的一些努力，的确学习了很多东西，但当从汽车电子行业转入半导体行业，我也认识到山外有山，人外有人。而目前的我感觉很低沉，就感觉和现在的股市一样，上攻乏力，下踏不稳，想攻入Ｌｉｎｕｘ，奈何山太大，不知道从何处开始攀岩，辗转反侧，多日不能安稳睡觉（不知道是不是这个原因），后来想想术业有专攻，我既然在工作中学习和使用ＡＵＴＯＳＡＲ都４年了，何不继续深入，虽然掌握ＡＵＴＯＳＡＲ　ＢＳＷ，但对ＲＴＥ还是知之甚少，何不更加深入一步，在了解和掌握[ＡＵＴＯＳＲ　ＲＴＥ](http://www.autosar.org/fileadmin/files/releases/4-2/software-architecture/rte/standard/AUTOSAR_SWS_RTE.pdf)之后，何去何从在做决定也不迟。

读过ＲＴＥ文档数遍之后，我知道，这是个硬茬，其需要工具的配合，而ＡＲＣＣＯＲＥ未曾提供ＲＴＥ工具的试用版，所以只能靠自己了。其实以我之见，ＲＴＥ无非是符合特定要求的ＢＳＷ的一层封装，应该不是很难，但ＲＴＥ文档太抽象，我也从未见过完整的ＲＴＥ是个什么样子的。好在，日本[ｔｏｐｐｅｒｓ开源了其ＲＴＥ工具](https://www.toppers.jp/a-rtegen-download.html)，还开放了ＲＴＥ开发环境，所以正好可以拿过来学习一番，奈何其文档和代码注释全是日文，又得依赖翻译软件了。这里我同时下载了[a-rtegen-1.3.0](https://www.toppers.jp/download.cgi/a-rtegen-1.3.0.tar.gz)和[a-rtedev eclipse kepler](https://www.toppers.jp/download.cgi/a-rtegen-dev.zip).

好吧学习之路又开始了。下载了上述２个包之后，还需要下载[atk2-sc3\_nios2\_dev\_gcc](https://www.toppers.jp/download.cgi/atk2-sc3_nios2_dev_gcc-20160324.tar.gz)。解压之后将包a-rtegen和atk2-sc3放入同一目录下，然后运行如下命令,有一步操作失败了，虽败犹荣，毕竟我是在ｕｂｕｎｔｕ上运行的该命令，而命令uncrustify.exe是ｗｉｎｄｏｗｓ格式，当然失败了。但同样可以看到生成了很多Ｃ文件。

```sh
parai@UX303LB:~/workspace/toppers/a-rtegen/sample/sc3/HelloAutosar$ git commit -m " add AUTOSAR 4.0.3 schema"
[master 8450dbe]  add AUTOSAR 4.0.3 schema
 9 files changed, 256671 insertions(+), 2 deletions(-)
 create mode 100644 bin/schema/AUTOSAR_4-0-3.css
 create mode 100644 bin/schema/AUTOSAR_4-0-3.xsd
 create mode 100644 bin/schema/AUTOSAR_4-0-3_COMPACT.xsd
 create mode 100644 bin/schema/AUTOSAR_4-0-3_STRICT.xsd
 create mode 100644 bin/schema/AUTOSAR_4-0-3_STRICT_COMPACT.xsd
 create mode 100644 bin/schema/autosar.soc
 create mode 100644 bin/schema/readme.txt
 create mode 100644 bin/schema/xml.xsd
parai@UX303LB:~/workspace/toppers/a-rtegen/sample/sc3/HelloAutosar$ ls
configure.sh  HelloAutosar.yaml
parai@UX303LB:~/workspace/toppers/a-rtegen/sample/sc3/HelloAutosar$ ./configure.sh 
configure: Generating Makefile from ../../../../atk2-sc3/sample/Makefile.
Executable file of the configurator (cfg) is not found.
Generated ./HelloAutosar.arxml
Checking input AUTOSAR XMLs...
Generating ./Rte_InternalDataTypes.arxml ...
There are some insufficient configurations for RTE.
Generating AUTOSAR XMLs to complement the insufficient configurations... 
Generating ./Rte_GeneratedEcuc.arxml ...
Generation done.
Generated ./Rte_GeneratedEcuc.yaml
Generated ./HelloAutosar.arxml
Checking input AUTOSAR XMLs...
Generating ./Rte_InternalDataTypes.arxml ...
Generating RTE...
Generation done.
parai@UX303LB:~/workspace/toppers/a-rtegen/sample/sc3/HelloAutosar$ ls
configure.sh        Rte_Bsw_Api.h  Rte_Common.h             Rte_Hook.h                         Rte_Partition_EcucPartition_Rte.h   Rte_Partition_EcucPartition_SWC2.h  Rte_SWC2_Type.h
HelloAutosar.arxml  Rte.c          Rte_GeneratedEcuc.arxml  Rte_InternalDataTypes.arxml        Rte_Partition_EcucPartition_SWC1.c  Rte_SWC1.h                          Rte_Type.h
HelloAutosar.yaml   Rte_Cbk.h      Rte_GeneratedEcuc.yaml   Rte_Main.h                         Rte_Partition_EcucPartition_SWC1.h  Rte_SWC1_Type.h                     Rte_Util.h
Makefile            Rte_Cfg.h      Rte.h                    Rte_Partition_EcucPartition_Rte.c  Rte_Partition_EcucPartition_SWC2.c  Rte_SWC2.h
```
