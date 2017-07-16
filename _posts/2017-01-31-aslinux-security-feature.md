---
layout: post
title: linux security feature overview
category: linux
comments: true
---
  This page will record all the security related information for fast search and study purpose.

# [Security](https://wiki.tizen.org/wiki/Security)
  Below 3 middleware are inherited from Tizen by [AGL](https://www.automotivelinux.org/).

* [cynara](https://wiki.tizen.org/wiki/Security:Cynara):[GIT](https://github.com/Samsung/cynara)
* [SMACK](https://wiki.tizen.org/wiki/Security/Smack):[GIT](https://github.com/smack-team/smack)
* [security manager](https://wiki.tizen.org/wiki/Security/Tizen_3.X_Security_Manager):[GIT](https://github.com/Samsung/security-manager)

![cynara](https://wiki.tizen.org/w/images/8/8b/CynaraRoadmap_Phase3_UserSupport.png)

# [LSM(Linux Security Modules)](https://en.wikipedia.org/wiki/Linux_Security_Modules):[IBM](https://www.ibm.com/developerworks/cn/linux/l-lxc-security/)/[eLinux](http://elinux.org/Security)/[TXT](http://lxr.free-electrons.com/source/Documentation/security/LSM.txt?v=3.18)

* [SMACK](https://en.wikipedia.org/wiki/Smack_(software)):[SRC](http://lxr.free-electrons.com/source/security/smack/?v=3.18)/[TXT](http://lxr.free-electrons.com/source/Documentation/security/Smack.txt?v=3.18)/[OsChina](https://www.oschina.net/question/591648_247054)
* [SELINUX](https://en.wikipedia.org/wiki/Security-Enhanced_Linux):[SRC](http://lxr.free-electrons.com/source/security/selinux/?v=3.18)/[TXT](http://lxr.free-electrons.com/source/Documentation/security/SELinux.txt?v=3.18)
* [APPARMOR](https://en.wikipedia.org/wiki/AppArmor):[SRC](http://lxr.free-electrons.com/source/security/apparmor/?v=3.18)/[TXT](http://lxr.free-electrons.com/source/Documentation/security/apparmor.txt?v=3.18)
* [TOMOYO](https://en.wikipedia.org/wiki/Tomoyo_Linux):[SRC](http://lxr.free-electrons.com/source/security/tomoyo/?v=3.18)/[TXT](http://lxr.free-electrons.com/source/Documentation/security/tomoyo.txt?v=3.18)
* [YAMA](https://en.wikipedia.org/wiki/Yama_(Linux_security_module)):[SRC](http://lxr.free-electrons.com/source/security/yama/?v=3.18)/[TXT](http://lxr.free-electrons.com/source/Documentation/security/Yama.txt?v=3.18)

![LSM](http://static.oschina.net/uploads/img/201508/18143741_Gvp0.jpg)

# dm_verity :: Android & Linux

  Nowdays more and more features of Linux are coming from Android, such as the dm-verity which is some how part of the [secure boot](https://source.android.com/security/verifiedboot/) that gurantee the security of system during run-time. dm-verity helps prevent persistent rootkits that can hold onto root privileges and compromise devices.

*  [kernel/drivers/md/dm\_verity.c](http://lxr.free-electrons.com/source/drivers/md/dm-verity.c?v=3.18)
*  [android/system/core/fs\_mgr/fs\_mgr\_verity.c](https://android.googlesource.com/platform/system/core/+/android-5.0.0_r2/fs_mgr/fs_mgr_verity.c)
*  [kernel/Documentation/device-mapper/verity.txt](https://www.kernel.org/doc/Documentation/device-mapper/verity.txt)
*  [csdn:Android 中的dm-verity](http://blog.csdn.net/u011280717/article/details/51867673)

  So for linux, whether the fs\_mgr should be ported to do this job to mount the verity block by a special fs_tab with flags "verity=/dev/mmcblk0p1" for example, or use the linux tool [dmsetup:veritysetup](https://gitlab.com/cryptsetup/cryptsetup/wikis/DMVerity).

# [IMA/EVM](https://sourceforge.net/p/linux-ima/wiki/Home/) 

* [src](http://elixir.free-electrons.com/linux/v4.13-rc1/source/security/integrity) 
* [evmctl](https://sourceforge.net/p/linux-ima/ima-evm-utils/ci/master/tree): README is important
