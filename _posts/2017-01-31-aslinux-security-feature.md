---
layout: post
title: linux security feature overview
category: linux
comments: true
---
# [security](https://wiki.tizen.org/wiki/Security)
  Inherit from Tizen by [AGL](https://www.automotivelinux.org/).

  [cynara](https://wiki.tizen.org/wiki/Security:Cynara):[GIT](https://github.com/Samsung/cynara)

  [smack](https://wiki.tizen.org/wiki/Security/Smack):[TXT](http://lxr.free-electrons.com/source/Documentation/security/Smack.txt?v=3.18)/[GIT](https://github.com/smack-team/smack)/[SRC](http://lxr.free-electrons.com/source/security/smack/?v=3.18)

  [security manager](https://wiki.tizen.org/wiki/Security/Tizen_3.X_Security_Manager):[GIT](https://github.com/Samsung/security-manager)

# dm_verity :: Android & Linux

  Nowdays more and more features of Linux are coming from Android, such as the dm-verity which is some how part of the [secure boot](https://source.android.com/security/verifiedboot/) that gurantee the security of system during run-time. dm-verity helps prevent persistent rootkits that can hold onto root privileges and compromise devices.


  [kernel/drivers/md/dm\_verity.c](http://lxr.free-electrons.com/source/drivers/md/dm-verity.c?v=3.18)

  [android/system/core/fs\_mgr/fs\_mgr\_verity.c](https://android.googlesource.com/platform/system/core/+/android-5.0.0_r2/fs_mgr/fs_mgr_verity.c)

  [kernel/Documentation/device-mapper/verity.txt](https://www.kernel.org/doc/Documentation/device-mapper/verity.txt)

  So for linux, whether the f\s_mgr should be ported to do this job to mount the verity block by a special fs_tab with flags "verity=/dev/mmcblk0p1" for example, or use the linux tool [dmsetup:veritysetup](https://gitlab.com/cryptsetup/cryptsetup/wikis/DMVerity).

