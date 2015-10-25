
LM3S6965
==
	get the related resource files from the below 2 links.
	http://www.ti.com/product/LM3S6965/toolssoftware
	http://www.ti.com/tool/pdl-lm3s-exe

	qemu-arm-system do support the machine lm3s6965evb, so use this qemu machine to do simulation of real hardware, and one of the serial port to do simulation of CAN.

	Let's start.

# Command:

	qemu-system-arm  -cpu cortex-m3 -nographic -monitor null -serial null -semihosting -kernel main.elf -machine lm3s6965evb
	
	It is also possible to attach gdb to qemu (you'll need two windows):

    qemu-system-arm -cpu cortex-m3 -nographic -monitor null -serial null -semihosting -kernel main.elf -gdb tcp::51234 -S arm-none-eabi-gdb main.elf 

	"51234" is simply a tcp port number chosen randomly. Within gdb execute the following commands:

	(gdb) target remote localhost:51234
	(gdb) load
	(gdb) break main
	(gdb) continue