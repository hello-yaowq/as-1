---
layout: post
title: 真实的嵌入式汽车电子 -- 2　初识Ｃ语言
category: real embeded world of automotive
comments: true
---

不知道现在的局面使怎样的，我只知道那时我学Ｃ语言使用的是ＶＣ６．０开发环境，那时我还以为世界上就所有的电脑都一样的，就一个操作系统ｗｉｎｄｏｗｓ，但后来我接触了ｌｉｎｕｘ，之后了解了ＭｉｎＧＷ项目，那之后，我就再也没有用过ＶＣ６．０了。现在，我喜欢在ｕｂｕｎｔｕ上写代码，编译调试和仿真，偶尔也会切换到ｗｉｎｄｏｗｓ环境，但只会用ＭｉｎＧＷ工具连，集成开发环境我选ｅｃｌｉｐｓｅ。

对于一个想在嵌入式行业里好好混下去的人，我认为仅了解Ｃ语言使不够的，对汇编机器指令不说精通，一定要有些理解，不然只知其一，不知其二，也不可能对Ｃ语言各关键子能有各好的理解，如ｓｔａｔｉｃ。

我想Ｃ变量作用域在开始之初肯定还是比较折磨人的，什么局部变量，局部静态变量，全局变量等，我了个去，反正当初我是傻傻分不清楚。例如，如下代码：

```c
#include <stdio.h>
int a=0;
int main(int argc,char* argv[])
{
	int a = a + 1;
	if(a==1)
	{
		int a = a + 1;
		printf("1: a is %d\n",a);
		a += 3;
		printf("2: a is %d\n",a);
	}
	printf("3: a is %d\n",a);
	return 0;
}
```

将上述代码保存为ｍａｉｎ.c，然后用ｇｃｃ编译，并运行，你就会惊奇的发现，我操，怎么会是这个结果呢？

```sh
parai@UX303LB:~/workspace/tmp$ gcc main.c 
parai@UX303LB:~/workspace/tmp$ ls
a.out  main.c
parai@UX303LB:~/workspace/tmp$ ./a.out 
1: a is 1
2: a is 4
3: a is 1
```

我本以为第一个ｐｒｉｎｔｆ会输出２，结果确是１，好吧，我倒现在还是没有搞明白作用域，反正这个不是重点，重点是，我想说的是，编码一定要有规范，例如，上面的代码如果出现在你的代码中，恭喜你，你的老板一定会骂你，写的什么玩意。

说到编码规范，那就不得不说说汽车电子通用规范[ＭＩＳＲＡＣ](http://caxapa.ru/thumbs/468328/misra-c-2004.pdf),像上述的Ｃ代码在ＭＩＳＲＡＣ规范里是明令禁止的，因为太复杂，一般人根本看不懂ａ到底是哪个ａ，所以我们不用去研究这些不必要的语言规范，像什么逗号表达式什么的，完全没什么大的用处。用我四年的嵌入式经验总结下，在嵌入式行业，会基本的Ｃ语言语法ｉｆ　ｅｌｓｅ　ｗｈｉｌｅ　ｆｏｒ　ｓｗｉｔｃｈ　ｃａｓｅ等已然足够，不必花心思话研究指针啊，数组啊什么的，因为除非你的工作是去做算法优化，不然根本用不上。在嵌入式行业，只要能看懂数据手册，会配寄存器基本够混的了，有口饭吃，已经不成问题了。

但是如果能稍微了解下汇编语言，那是极好的，能看懂汇编，将其和Ｃ语言相结合，那你基本可以屹立不倒了，使用命令“gcc -S main.c  -o main.s”可将Ｃ语言编译成汇编语言。

```asm
	.file	"main.c"
	.globl	a
	.bss
	.align 4
	.type	a, @object
	.size	a, 4
a:
	.zero	4		/* 在ｂｓｓ段分配一个全集变量ａ　*/
	.section	.rodata
.LC0:
	.string	"1: a is %d\n"	/* 在ｒｏｄａｔａ只读数据段存储一个字符串１　*/
.LC1:
	.string	"2: a is %d\n"	/* 在ｒｏｄａｔａ只读数据段存储一个字符串２　*/
.LC2:
	.string	"3: a is %d\n"	/* 在ｒｏｄａｔａ只读数据段存储一个字符串３　*/
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp			/* 保存堆栈指针当前位置　*/
	.cfi_def_cfa_register 6
	subq	$32, %rsp			/* 开辟３２字节空间给ｍａｉｎ函数局部变量　*/
	movl	%edi, -20(%rbp)
	movq	%rsi, -32(%rbp)
	addl	$1, -8(%rbp)		/* -8(%rbp) 即第一个局部变量ａ，对他没有没有做任何初始化就直接加１，所以说ａ的值是个随机值 */
	cmpl	$1, -8(%rbp)		/* 刚好，这个随机值是０，所以ｉｆ条件为ｔｒｕｅ，　ｉｆ(a==1) */
	jne	.L2
	addl	$1, -4(%rbp)		/* 看吧，第二个局部变量ａ为-4(%rbp)，　又是一次没有初始化就直接加１了，看吧，能看懂汇编，什么都清楚了　*/
	movl	-4(%rbp), %eax		/* eax 寄存器的值为第二个局部变量a　*/
	movl	%eax, %esi
	movl	$.LC0, %edi			/* 好吧，这里是为了向ｐｒｉｎｔｆ函数传入参数，第一个字符串１　*/
	movl	$0, %eax
	call	printf
	addl	$3, -4(%rbp)		/* 第二个局部变量ａ加３　*/
	movl	-4(%rbp), %eax
	movl	%eax, %esi
	movl	$.LC1, %edi
	movl	$0, %eax
	call	printf
.L2:
	movl	-8(%rbp), %eax		/* ｅａｘ为第一个局部变量ａ的值 */
	movl	%eax, %esi			/* 所以一切一目了然，ｍａｉｎ函数自始至终没有访问全局变量，今天我终于再次　*/
	movl	$.LC2, %edi			/* 学习了Ｃ语言的变量作用域　*/
	movl	$0, %eax
	call	printf
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 4.8.4-2ubuntu1~14.04.1) 4.8.4"
	.section	.note.GNU-stack,"",@progbits
```

所以Ｃ语言还是一门和机器汇编语言紧密结合的语言，精通Ｃ，会一点汇编，走遍天下都不怕了，接下来分析下ＲＴＯＳ的任务上下文的切换和保存恢复。




