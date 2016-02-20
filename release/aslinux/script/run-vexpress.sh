#! /bin/sh

option=0

cd out

if [ $# -eq 1 ] ; then 
	if [ $1 = 'd' ]; then
		option=1
	elif [ $1 = 'n' ]; then
		option=2
	fi
fi
if [ $option -eq 1 ] ; then
	echo "  >> DEBUG by gdb"
	echo "     arm-linux-gnueabihf-gdb"
	echo "      file vmlinux"
	echo "      target remote:1234"
	echo "      b start_kernel"
    qemu-system-arm -kernel rootfs/zImage -dtb rootfs/vexpress-v2p-ca9.dtb -M vexpress-a9 \
    	-append "root=/dev/mmcblk0 console=ttyAMA0 console=tty0" -sd sdcard.ext3 -serial stdio -s -S
elif [ $option -eq 2 ] ; then
	qemu-system-arm -M vexpress-a9 -m 512M -kernel rootfs/zImage -dtb rootfs/vexpress-v2p-ca9.dtb \
		-nographic -append "init=/linuxrc root=/dev/mmcblk0 rw console=ttyAMA0"	\
		-sd sdcard.ext3 -smp 4 -m 512M
else
    qemu-system-arm -M vexpress-a9 -kernel rootfs/zImage -dtb rootfs/vexpress-v2p-ca9.dtb \
    	-append "init=/linuxrc root=/dev/mmcblk0 rw console=ttyAMA0 console=tty0" \
    	-sd sdcard.ext3 -serial stdio -net nic -net user -smp 4 -m 512M

#-usb -usbdevice touchscreen
fi
