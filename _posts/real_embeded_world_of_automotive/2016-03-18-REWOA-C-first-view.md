---
layout: post
title: 真实的嵌入式汽车电子 -- １　初识Ｃ语言
category: real embeded world of automotive
comments: true
---

记得初识Ｃ语言，那应是初入大学的大一，青春年少，那时只知道玩，根本就没有任何的心思想要好好学习。我一个机械设计专业的学门编程语言有个什么用，可后来，没有想到，Ｃ语言却成了我吃饭的技能。我对Ｃ语言的第一印象就是一堆拉丁字母在一起不知道怎么组合下，什么if for while完全没概念，只知道最后期末开始完全靠作弊才通过的，身为武大学子，再在这里又给武大抹黑了。悔不当初啊，那时要是知道我今后将以码代码为生，说什么也要好好学习，不至于毕业后这么辛苦，需要自己边工作边学习。但是话又说回来，编程本身是一件简单的事情，编程语言也绝非你想象的那么复杂，但难就难再你学好很多方面的专业知识才有可能做好你的工作。例如Ｃ语言，其无非就是由那么些个有限的拉丁关键字去组成各种不同的逻辑表达式，组成一个个函数，继而成为一个个程序。

OK,继续回到我的大学时代，可能说从一开始我对编程还是不感冒的，反正就是不会，那时的梦想还是做一个乖乖的机械设计工程师，在大一上学期挂了一门课之后，我就知道了，我去，不能在玩了，该好好学习了，可能在大一下学期吧，记不太清了，买了电脑，安装了ＡＵＴＯＣＡＤ／ＳｏｌｉｄＷｏｒｋｓ／ＰｒｏＥ等专业机械软件，还有Ｍａｔｌａｂ等分析类软件，从此踏上机械软件学习折腾之旅，工程图／３Ｄ建模当年我也是都干过的。正是在这过程中，我发现了我对编程有着相当的兴趣，在大四时彻底明白，我喜欢编程，并很擅长。

我记得我的第一个像样程序应该是ｌｉｓｐ语言，ＡＵＴＯＣＡＤ的插件语言，当时我是我们班唯一一个用该ｌｉｓｐ语言自动画出齿轮的人，那种优越和成就感油然而生,这里还是附上代码以来纪念我的青春，之后在大学的多门课程的里，不同程度的接触了一些编程相关的知识，在优化设计这么课上更是见识到Ｃ语言对机械设计的作用，能帮助工程师做各种计算，然后对编程着了迷，开始学习各种编程知识。在大四时更是学习ｕｃＯＳ，邵贝贝翻译的那本书算是打开了我嵌入式世界之门，对微内核算是有了个透彻的理解，之后又迅速的学习了ＦｒｅｅＲＴＯＳ和一个国内大牛开发的ｒｔＴｈｒｅａｄ。之后于渊的一本《自己动手写操作系统》让我对操作系统有了更深入的认识，之后还读过ｌｉｎｕｘ０．１１代码一段时间，可惜没能坚持下去，因为要开始找工作了。

``` lisp
(defun gear()
 (setq pt (getpoint"\nEnter Basepoint：")
       x0 (car pt)
       y0 (cadr pt)
 )
 (setq m (getreal "\nEnter diameter m:")
       z (getint "\nEnter diameter z:")
       b (getint "\nEnter diameter b:")
 )
       
 (setq r  (/ ( * m z) 2.0) ;;;分度圆半径
       ra (/ (* (+ z 2.0) m) 2.0);;;齿顶圆半径
       rf (/ (* (- (- z 2.0) 0.5) m) 2.0) ;;;齿根圆半径
       rb (* (/ ( * m z) 2.0) 0.93969262) ;;;基圆半径
       d0 (/ rf 3)
       r0 (/ d0 2.0)
  )
 (command "limits" (list 0 0) (list (+ x0 (* 2 ra) b 50) (+ y0 ra 60)))
 (command "zoom" "a")
 (command "layer" "s" "0" "");;;细虚线
 (command "circle" (list x0 y0) r)
 (command "circle" (list x0 y0) rb)
 (command "layer" "s" "1" "");;;粗实线
 (command "circle" (list x0 y0) rf)
 (command "circle" (list x0 y0) ra)
 (command "circle" (list x0 y0) r0)
 (command "line" (list (+ x0 ra 50) (+ y0 r0)) (list (+  x0 ra 50) (+ y0 rf)) (list (+ x0 ra 50 b) (+ y0 rf)) (list (+ x0 ra 50 b) (+ y0 r0)) "c")
 (command "layer" "s" "3" "");;;剖面
 (command "hatch" "u" "45" "5" "n" "w" (list (+ x0 ra 50 b 10) y0) (list (+ x0 ra 10) (+ y0 ra)) "")
 (command "layer" "s" "1" "")
 (command "line" (list (+ x0 ra 50) (- y0 ra)) (list (+  x0 ra 50) (+ y0 ra)) (list (+ x0 ra 50 b) (+ y0 ra)) (list (+ x0 ra 50 b) (- y0 ra)) "c")
 (command "layer" "s" "0" "")
 (command "line" (list (+ x0 50 ra) (- y0 rf)) (list (+ x0 50 ra b) (- y0 rf)) "")
 (command "line" (list (+ x0 50 ra) (- y0 r0)) (list (+ x0 50 ra b) (- y0 r0)) "")
 (command "layer" "s" "4" "");;;中心线
 (command "line" (list (- x0 ra 10) y0) (list (+ x0 ra 10) y0) "")
 (command "line" (list x0 (- y0 ra 10)) (list x0 (+ y0 ra 10)) "")
 (command "line" (list (+ x0 ra 40) y0) (list (+ x0 ra 50 b 10) y0) "")
 (command "line" (list (+ x0 ra 50 (/ b 2)) (- y0 ra 10)) (list (+ x0 ra 50 (/ b 2)) (+ y0 ra 10)) "")
 (command "layer" "s" "2" "");;;标注层
 (setq df (* rf 2.0)
       da (* ra 2.0)
 )
 (setq da (rtos da)
       df (rtos df)
       d0 (rtos d0)
 )
 (setq da (strcat "%%c" da)
       df (strcat "%%c" df)
       d0 (strcat "%%c" d0)
 )

 (command "dim")
 (command "dimtxt" "5")
 (command "hor" (list (- x0 rf) y0) (list (+ x0 rf) y0) (list x0 (+ y0 ra 10)) df)
 (command "hor" (list (- x0 ra) y0) (list (+ x0 ra) y0) (list x0 (+ y0 ra 20)) da)
 (command "hor" (list (- x0 r0) y0) (list (+ x0 r0) y0) (list x0 (+ y0 r0 10)) d0)
 (command "hor" (list (+ x0 ra 50) y0) (list (+ x0 ra 50 b) y0) (list (+ x0 ra 50 (/ b 2)) (+ y0 ra 10)) b)  
 (command "exit")

  )
```

找工作这件事绝对是我的噩梦，一个理论上算是纯机械专业的人跑去找一份软件行业的工作，别开玩笑了，所以屡屡碰壁，以至于后来都想放弃了，但去找纯机械相关的工作却也屡遭鄙视，只顾着玩软件，专业知识基本为零，我了个去，那叫个心灰意冷啊，那是我最痛苦的时候。无赖，我最终放弃了去高大上企业的想法，然后去了个都是小企业的招聘会，机缘巧合，就在那里我找到了普华i-soft,然后普华也要了我，可是工资极低，又在上海，但是我接受了，我直觉告诉我汽车电子或许还是个不错的行业。后来的经历都告诉我，上天还是眷顾我的。
