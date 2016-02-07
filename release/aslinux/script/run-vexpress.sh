#! /bin/sh

debug=0

cd out

if [ $# -eq 1 ] ; then 
	if [ $1 = 'd' ]; then
		debug=1
	fi
fi
if [ $debug -eq 1 ] ; then
	echo "  >> DEBUG by gdb"
	echo "     arm-linux-gnueabihf-gdb"
	echo "      file vmlinux"
	echo "      target remote:1234"
	echo "      b start_kernel"
    #qemu-system-arm -kernel u-boot -M vexpress-a9 -append "root=/dev/mmcblk0 console=ttyAMA0 console=tty0" -sd sdcard.ext3 -serial stdio -s -S
    qemu-system-arm -kernel rootfs/zImage -dtb rootfs/vexpress-v2p-ca9.dtb -M vexpress-a9 -append "root=/dev/mmcblk0 console=ttyAMA0 console=tty0" -sd sdcard.ext3 -serial stdio -s -S
else
    #qemu-system-arm -kernel rootfs/u-boot -M vexpress-a9 -append "root=/dev/mmcblk0 console=ttyAMA0 console=tty0" -sd sdcard.ext3 -serial stdio
    qemu-system-arm -kernel rootfs/zImage -dtb rootfs/vexpress-v2p-ca9.dtb -M vexpress-a9 -append "root=/dev/mmcblk0 rw console=ttyAMA0 console=tty0" -sd sdcard.ext3 -serial stdio
fi
