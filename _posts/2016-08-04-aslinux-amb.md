---
layout: post
title: aslinux automotive message broker
category: linux
comments: true
---


## [AMB](https://github.com/otcshare/automotive-message-broker) is part of [AGL](https://www.automotivelinux.org/)

Automotive Message Broker is a vehicle network abstraction system. It brokers information from the vehicle to applications. It provides application with a rich API for accessing vehicle data.

I want to study it on the host machine Ubuntu, and I think it was possible as it was a midware.

[2016-08-20]: by several days work after the work, I succesfully done the source plugin with AUTOSAR COM stack for AMB.

check [the plugin source code](https://github.com/parai/as/tree/master/release/aslinux/automotive-message-broker/plugins/autosar) for AMB for more detailed information about how the sample AUTOSAR COM stack source plugin works.

the build the source code as the below command:

``` sh
git clone https://github.com/parai/as.git
cd as/as/release/aslinux/automotive-message-broker/plugins/autosar
sudo make amb work_dir=~/workspace
make canutils work_dir=~/workspace
``` 

do the test:

``` sh
parai@UX303LB:~$ ambd &
[1] 943
parai@UX303LB:~$ 97853.115        | Automotive Message Broker
97853.115        | Version: 0.14.0 Series: 0.14 (74A final)
parai.STDOUT     :: can set on-line!

parai@UX303LB:~$ 
parai@UX303LB:~$ cansend can0 100#002200334455667788 && amb-get VehicleSpeed && amb-get EngineSpeed
This application is deprecated.  It will disappear in the future.  Use ambctl
{
  "SpeedSequence": 12352, 
  "Speed": 34, 	# hex value is 0x0022
  "Zone": 0, 
  "Time": dbus.Double(97866.924, variant_level=2)
}
This application is deprecated.  It will disappear in the future.  Use ambctl
{
  "SpeedSequence": 12403, 
  "Speed": 51, 	# hex value is 0x33
  "Zone": 0, 
  "Time": dbus.Double(97866.99, variant_level=2)
}
```

so the next step is to research how to access the AMB property in QT5 application, let's start again.