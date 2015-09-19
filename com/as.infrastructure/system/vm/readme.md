Thinking in virtio for autosar

AUTOSAR VIRTIO

Thinking in virtio for AUTOSAR
============

# *Prelog*
> A lot of times, I am thinking about software and how to be a good programer. But it turns out that I am stupid, I am not the guy who are smart than others. So I have a decision to learn harder, use the perfect open-source software developed by others to create a my self owened Automotive Software, also, I will make it source open.
> As now I think virtualization is perfect way to develop our application when hardware is not ready, and the most important point is that it is much easier to debug and reduce the time of software develop phase. So I am alwasys trying to find a way to do the purpose better and better. I have tried simulate CAN network by socket, simulate OS by host OS thread and event simulate DIO LCD..etc through GTK or QT, as my limited ability, I can't make it runs well, I am always thinking...
> Now I have came up an idea that build the ECU simulated software to dynamic link library(*.dll/*.so), and then load by host Qt daemon simulation manager software, I have tried it both in windows and linux, so it has been proven out perfect.
> So question came how to do the data exchange between the ECUs and Qt daemon, the most easiest and perfect way is through callouts and callbacks, but recently I am studying virtio, I think it is good and I am attracted by its idea. As some how my work is to do simualtion of AUTOSAR software environment(VFB-Virtual Function Bus), so I think virtio is a good way, I can lean it aslo and do the work at the same time.
> Then on I go deep into linux virtio source code, by it turns out very difficult to understand as I have no base knowledge of Linux Driver System, I can't know how it works. By reading articles searched from google, it seems that I have gotten some points. So I begins.
> Now on, it is already a month, but no big progressing, so I think I should write something to analyse the virtio and then move on.

# *TODO*
> 1. Investigate how the virtio&remoteproc works on ECU side, this is host virtio.
> 2. Investigate how the virtio&remoteproc works on QT side, this is guest virtio.
> 3. There are variant numbers of ECUs as host, but only one QT guest, the guest should take care of the communicaton between hosts, for example CAN.


