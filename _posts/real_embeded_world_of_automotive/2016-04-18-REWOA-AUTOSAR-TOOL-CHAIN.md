---
layout: post
title: AUTOSAR工具链
category: real embeded world of automotive
comments: true
---

俗话说的好，工欲善其事，必先利其器。对于码农来说，最基本的工具既是文本编辑器（如ｖｉｍ／Ｅｍａｃｓ等），高级点的如集成开发环境ＩＤＥ（ｅｃｌｉｐｓｅ／ｖｉｓｕａｌ　ｓｔｕｄｉｏ）。因为听说过这样一句话，用ｖｉｍ的都是大牛，用ｅｍａｃｓ的都是大神，所以我选择使用了ｅｍａｃｓ一段时间，后来实在不适应没有鼠标的操作方式，放弃了，后来转用ｅｃｌｉｐｓｅ　ＩＤＥ，个人认为还是ｅｃｌｉｐｓｅ功能强大，且界面友好易于上手。

所以我的个人以学习为目的的仓库[AS](https://github.com/parai/as)基本使用ｅｃｌｉｐｓｅ来开发，包括现在写博客也用ｅｃｌｉｐｓｅ。

但是我今天不介绍ｅｃｌｉｐｓｅ，我要说的是ＡＵＴＯＳＡＲ这个软件架构的工具链。浏览[ａｒｃｃｏｒｅ](http://www.arccore.com/)官网你可以发现，其有很多产品出售，主要有三类，ＡＵＴＯＳＡＲ　ＣＯＲＥ(C代码), ＳＴＵＤＩＯ（主要是ＢＳＷ和ＲＴＥ配置生成工具）以及ｂｏｏｔｌｏａｄｅｒ（Ｃ代码），当你点开其产品ｓｔｕｄｉｏ目录，你会发现又会细分为好多类，但是这里我所掌握的只有ＢＳＷ的配置生成工具，目前我对ＲＴＥ也只是一知半解，需要进一步的学习。

这里再把话题扯开，我于２０１２年７月开始在普华工作，９月分的样子萌生了自己开发一个符合ＯＳＥＫ　ＯＳ规范的操作系统，那个时候对ｐｙｔｈｏｎ只是有所了解，所以选择使用ｐｙＱｔ创建了我这个自己开发的ＯＳ的配置工具，取名[Gainos Studio](https://github.com/parai/GaInOS_Studio),我开发的ｏｓ名称叫[Ｇａｉｎｏｓ](https://github.com/parai/GaInOS)，寓意Ｎｏ　Ｐａｉｎ　Ｎｏ　Ｇａｉｎ。仔细回想下，我在普华的第一年，的确是痛并快乐着，每天奋战到半夜１２点，不断的学习ＯＳＥＫ和ＡＵＴＯＳＡＲ，个人觉得，没有那第一年的艰苦奋斗，就没有我现在的悠然自得。

正是从Ｇａｉｎｏｓ　Ｓｔｕｄｉｏ开始，ｐｙｔｈｏｎ踏着小碎步一点一点走入我的世界，我已经彻底折服于这门强大的高级语言，并深深觉得ｐｙｔｈｏｎ的思想只提供一种最好的解决问题的方法是多么高明，这点和ｐｅｒｌ截然不同。并且，虽然为脚本语言，即时解释执行，ｐｙｔｈｏｎ也变得越来越快，这使得ｐｙｔｈｏｎ越来越适合做数值分析和作为快速工具来使用。引力波的发现可以说ｐｙｔｈｏｎ是具有很大功能的，我觉得这样的例子太多了。由于ｐｙｔｈｏｎ语言的广受欢迎，ｐｙｔｈｏｎ的各种ｐａｃｋａｇｅ包也越来越多，基本你所需要的各种功能都能找到对应的ｐａｃｋａｇｅ包。

这里举几个小例子来证明，ｐｙｔｈｏｎ是如何被我利用来辅助高效地完成个人汽车电子软件开发工作的。

####１ [正则表达式](http://www.cnblogs.com/huxi/archive/2010/07/04/1771073.html)——由Ｖｅｃｔｏｒ ＣＡＮ　ＤＢＣ文件到ＯＳＥＫ　ＯＩＬ文件的自动转换
工作中，曾经接到了这样一份工作，将一个由客户提供的有很多ＣＡＮ消息的ＤＢＣ文件手工转换为ＯＩＬ（ＯＳＥＫ　Ｉｍｐｌｅｍｅｎｔａｔｉｏｎ　Ｌａｎｇｕａｇｅ）文件，当时就傻掉了，ｓｈｉｔ，这是多么无聊的一份工作啊，但不管怎样，活得干啊，谁让我是小兵呢。习惯性的用文本编辑器ｎｏｔｅｐａｄ＋＋打开了那个让我头疼的ＤＢＣ文件，看了一下之后，然后用Ｖｅｃｔｏｒ　ＤＢＣ　Ｅｄｉｔｏｒ打开，比较下之后瞬间明白了，ＤＢＣ本身是具有固定格式的文本文件，如果使用ｐｙｔｈｏｎ正则表达式来解析该ＤＢＣ文件然后自动生成ＯＩＬ文件，那是何其的简单，所以前前后后也就花了２天左右的时间，工具出来了，用这个工具向公司提了个ＣＩ（Ｃｏｎｔｉｎｉｏｕｓ　Ｉｍｐｒｏｖｅｍｅｎｔ）竟然还拿奖了，可惜奖金很少就１００块。现在，该工具的解析部分被我升级了，使用ｐｌｙ库来解析，更加的简单和灵活，参见[ａｓｃｃ](https://github.com/parai/as/tree/master/com/as.tool/py.can.database.access/ascc),
一时无聊，还基于Ｅｘｃｅｌ　ＶＢＡ实现了一个解析工具[AS.xlsm](https://github.com/parai/as/blob/master/com/as.tool/config.infrastructure.system/AS.xlsm)，如下图所示：

![as-can-xlsm.png](/as/images/rewoa/as-can-xlsm.png)
<center> 图１ AUTOSAR CAN Excel Tool </center>

###２ [ｘｌｒｄ　Ｅｘｃｅｌ解析](https://pypi.python.org/pypi/xlrd/)
之所以我会掌握了解ｐｙｔｈｏｎ库[ｘｌｒｄ](https://pypi.python.org/pypi/xlrd/)的一个重要原因也是因为ＣＡＮ通讯矩阵Ｍａｔｒｉｘ，因为有一个客户竟然不提供ＣＡＮ　ＤＢＣ文件，而是一个如图１所示的Ｅｘｃｅｌ文件，在我接受该项目之前，我惊奇的发现，前人又是眼看手写配置文件ＯＩＬ，我又傻掉了，能不能不要这么无聊。之后，我发现工作中有很多这样的相同的事情，Ｅｘｃｅｌ常常作为一个软件某些配置信息的输入文件，如ＮＶＭ数据配置等，而这些配置信息对应的Ｃ代码通常是具有某种共性的，所以如果使用ｘｌｒｄ去解析ｅｘｃｅｌ文件，并生成相应Ｃ代码是很简单的事情，并节省工时同时又可避免人为手写代码因为不细心而导致的错误，何乐而不为呢,并且在Ｅｘｃｅｌ数据升级之后，只要执行ｐｙｔｈｏｎ解析生成工具就所有工作完成，比眼看手写的方式不知道快多少倍。如下代码片段是解析图１所示Ｅｘｃｅｌ　AS.xlsm的代码片段,从中可以看出使用ｘｌｒｄ解析一个ｅｘｃｅｌ并用[ｐｙｔｈｏｎ字典](https://www.linuxzen.com/python-you-ya-de-cao-zuo-zi-dian.html)来返回ｅｘｃｅｌ数据是何其的简单，你能使用其他语言写出如此精简的代码吗，我个人认为是没有的，所以我想说使用ｐｙｔｈｏｎ作为数据分析工具在合适不过了,我认为当你了解ｐｙｔｈｏｎ之后，会给你的工作带来另外一种态度，另外一种思维方式，另外我认为今后的汽车电子软件开发将高度依赖于工具，如ＡＲＣＣＯＲＥ的一系列工具（基于[ｅｃｌｉｐｓｅ　ａｒｔｏｐ](http://www.artop.org/)），但ｐｙｔｈｏｎ的强大用来做些ｅｃｌｉｐｓｅ不擅长的事情，如数据分析，各种小工具等，是具有很强的生命力的，经济实惠。

```python
import xlrd

def parse_as_xls_com(filename):
    scom = {}
    book = xlrd.open_workbook(filename)
    sheet = book.sheet_by_name('COM')
    for row in range(5,sheet.nrows):
        signal = []
        for col in range(1,sheet.ncols):
            signal.append(str(sheet.cell(row,col).value))
        name = signal[0]
        try:
            scom[name].append(signal)
        except KeyError:
            scom[name]=[signal]
    return scom

if(__name__ == '__main__'):
    for name,signals in parse_as_xls_com('AS.xlsm').items():
        print('%s = {'%(name))
        for sig in signals:
            print('\t%s'%(sig))
        print('}')
```

####３ [ｍａｔｈｐｌｏｔｌｉｂ](http://matplotlib.org/) 
这是我最为欣赏和认为最有价值的[一个ｐｙｔｈｏｎ软件包](http://matplotlib.org/)，有了这个包，[ｍａｔｌａｂ](http://cn.mathworks.com/?requestedDomain=www.mathworks.com)能干的事情基本都能干了，而且入门门槛比ｍａｔｌａｂ还要低很多。对于像我这样菜鸟般的软件工程师来说，工作中常见的需要数值分析的问题，我都可以用ｐｙｔｈｏｎ　ｍａｔｈｐｌｏｔｌｉｂ来完成，甚至我还会用其来做简单建模来辅助软件设计。但是我个人一直没搞懂怎么安装此软件包，因为其依赖于其他很多软件包，如ｐｙＱｔ，[ｎｕｍｐｙ](http://www.numpy.org/)等，所以后来我也就不折腾了，干脆使用[ｐｙｔｈｏｎ（ｘ，ｙ）](http://python-xy.github.io/)，该ｐｙｔｈｏｎ软件安装包包含了几乎所有常用软件包，用于科学计算和数值分析，但是该安装包仅面向ｗｉｎｄｏｗｓ操作系统，对于我这样使用ｕｂｕｎｔｕ　ｌｉｎｕｘ操作系统的，则不得不使用虚拟机运行个ＸＰ然后使用ｐｙｔｈｏｎｘｙ，如下图所示，展示了ｐｙｔｈｏｎｘｙ的基本架构和功能模块，可见其强大。

![pythonxy](http://python-xy.github.io/images/pythonxy_2117.png)
<center> 图２　ｐｙｔｈｏｎ（ｘ，ｙ） </center>

如下图所示，则为使用ｍａｔｈｐｌｏｔ可以画出的图表，可知其功能真的很强大。
![mathplotlib](http://matplotlib.org/_static/logo_sidebar_horiz.png)
<center> 图３　matplotlib　ｅｘａｍｐｌｅ </center>

如下代码片段则为我通常使用ｐｙｔｈｏｎ　ｍａｔｈｐｌｏｔｌｉｂ的一种方式，对我而言，只要会这种方式基本足矣，因为我基本上和复杂数学问题无缘了，嵌入式基础软件行业还是一个比较简单的行业，工作比较繁琐，但不是很难。

```python
from pylab import *

X = [-4,-3,-2,-1,0,1,2,3,4]
Y0 = [-4,-3,-2,-1,0,1,2,3,4]
Y1 = [4,3,2,1,0,-1,-2,-3,-4]

plt.plot(X,Y0,color="blue", linewidth=1.0, linestyle="-",label='y=x')
plt.plot(X,Y1,color="green", linewidth=1.0, linestyle="-",label='y=-x')

X = np.linspace(-np.pi, np.pi, 256,endpoint=True)
C,S = np.cos(X), np.sin(X)

plt.plot(X, C, color="red", linewidth=1.0, linestyle="-",label='y=cos(x)')
plt.plot(X, S, color="black", linewidth=1.0, linestyle="-",label='y=sin(x)')

grid()

plt.title('Example of mathplotlib')
plt.legend()

plt.show()
```

以上介绍了这么多ｐｙｔｈｏｎ的东西，其实实际上和ＡＵＴＯＳＡＲ工具链没什么关系，应该介绍[ａｒｔｏｐ](http://www.artop.org/)才对的吗。其实最开始的时候，我也想基于ａｒｔｏｐ做二次开发，开发ＡＵＴＯＳＡＲ　ＢＳＷ和ＲＴＥ的配置工具，但是，我只有我一个人，ａｒｔｏｐ那样一个基于ｅｃｌｉｐｓｅ的庞然大物让我不知道从哪下口，实在是啃不动啊，转念想想，ｐｙｔｈｏｎ这么简单，干嘛不用ｐｙｔｈｏｎ来干呢。但是，这个过程也是及其艰苦的，在经过好几个版本的迭代，才有了现在从理论上来说已经还算完美的ＡＵＴＯＳＡＲ　ＢＳＷ配置工具[ｅａｓｙＳＡＲ](https://github.com/parai/as/tree/master/com/as.tool/config.infrastructure.system),这里不得不说的一点是ＡＲＣＣＯＲＥ　ＡＵＴＯＳＡＲ　Ｖ３．１　ＳＴＵＤＩＯ的一个月试用给了我很多帮助，其实从如下ｅａｓｙＳＡＲ工具界面上看，其实已经基本上差不多，基本上可以算是一个模仿，但此ｅａｓｙＳＡＲ配置工具目前不支持配置数据有效性的校验，只能在生成Ｃ配置文件的时候去校验配置的正确性，但由于一个人精力有限，这个生成前的数据检验我也没做。

![config.infrastructure.system](/as/images/config.infrastructure.system.png)
<center> 图４　ｅａｓｙＳＡＲ　ｓｔｕｄｉｏ </center>

在ｅａｓｙＳＡＲ的前一个版本[gainos studio](https://github.com/parai/gainos-tk/tree/master/tool/gainos-studio)的时候，每一个ＢＳＷ模块的ＵＩ都是用ＱＴ　Ｃｒｅａｔｏｒ手工一步一步画出来的，每个ＢＳＷ的ＵＩ都有一个ＵＩ类，来对每个控件的点击操作做出合理的响应,其如下图５所示，每增加一个ＢＳＷ配置模块的工作量相当之大，没有２天时间根本做不出来，但是新的工具ｅａａｓｙＳＡＲ就不一样了，因为其界面是根据我自定义的一个[ａｒｘｍｌ](https://github.com/parai/as/blob/master/com/as.tool/config.infrastructure.system/arxml/easySAR.arxml)格式的文件自动生成的（曾经试想完全符合ＡＵＴＯＳＡＲ　ＡＲＸＭＬ格式，太复杂就放弃了），配置后的数据和ｇａｉｎｏｓ　ｓｔｕｄｉｏ一样都保存为ａｒｘｍｌ格式，但是注意了，ｅａｓｙＳＡＲ的保存操作也是按照自定义的ａｒｘｍｌ格式来保存的，有兴趣的可以之间查看保存后的[配置文件](https://github.com/parai/as/blob/master/com/as.application/common/autosar.arxml)和[定义文件](https://github.com/parai/as/blob/master/com/as.tool/config.infrastructure.system/arxml/easySAR.arxml)的区别。但是每个模块的生成工具和ｇａｉｎｏｓ　ｓｔｕｄｉｏ一样，需要定制，也就是说ｅａｓｙＳＡＲ省去了２个操作即ＧＵＩ创建和配置数据保存的自定义操作，只要你定义好你的ＢＳＷ属性描述文件，ＧＵＩ自动生成，配置数据自动保存，你只要个性化定制生成工具就好了，所以我认为这已经很简单了，所以取名ｅａｓｙＳＡＲ。虽然实现的或许不是那么的好，但核心代码总共加起来不超过１０００行，自己有兴趣就去读代码看其是怎么实现的吧，我个人认为目前为止，这已经是极致了，我想不出其他的更好的方法来定制每个ＢＳＷ的配置工具。

![gainos_studio](/as/images/rewoa/gainos-tk-studio.png)
<center> 图５　ｇａｉｎｏｓ　ｓｔｕｄｉｏ </center>

