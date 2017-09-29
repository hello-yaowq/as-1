
# ASKAR

## prelog

I have designed [GaInOS](https://github.com/parai/GaInOS) and [GaInOS-TK](https://github.com/parai/gainos-tk) since 2012, but all the development has been stopt for a long time till now, somehow it's a regret to me but I think I have no chooice as I feel a pain that I have too much to learn and I think that the design is not good.

And now as I have read a lot of RTOS such as ITRON/ucos/freertos/freeosek/trampoline/toppers/rtthtrad/..., okay really many, so I want to restart the design of a kind of RTOS for automotive MCU.

* BUT NOTE THAT I WILL BORROW IDEAs FROM OTHERS.

So somehow you can say it's a copy, is it legal? I am not sure, I just want to develop a good RTOS for automotive MCU.

Then I want to give a new name to this RTOS, how about ASKAR(Automotive oSeK AutosaR)? It pronounce good, is't it?

## design

### schedule policy

* 1. ucos 8x8 ready map
* 2. freertos ready priority list
* 3. trampoline heap entry with bubble up/down

The above policy 1/2 is the most common policy that used by most of other RTOS with just a little bit difference. The policy 3 is really a rare case I have never seen before on any other RTOS, but it was really a good design.
