

By default, this project support the machine MPC5634M_MLQB80 only, but event for this a empty template project should be created by codewarrior v2.10 IDE as below filre structure.

```sh
MPC5634M_MLQB80
©¸©¤©¤ Project
    ©À©¤©¤ Project.mcp
    ©À©¤©¤ Project_Data
    ©¦   ©À©¤©¤ CWSettingsWindows.stg
    ©¦   ©À©¤©¤ RAM
    ©¦   ©¦   ©À©¤©¤ ObjectCode
    ©¦   ©¦   ©¸©¤©¤ TargetDataWindows.tdt
    ©¦   ©¸©¤©¤ internal_FLASH
    ©¦       ©¸©¤©¤ TargetDataWindows.tdt
    ©À©¤©¤ bin
    ©¦   ©À©¤©¤ RAM.MAP
    ©¦   ©À©¤©¤ RAM.elf
    ©¦   ©¸©¤©¤ RAM.mot
    ©À©¤©¤ lcf
    ©¦   ©À©¤©¤ MPC5634M_MLQB80.lcf
    ©¦   ©¸©¤©¤ MPC5634M_MLQB80_DEBUG.lcf
    ©À©¤©¤ readme.txt
    ©¸©¤©¤ src
        ©À©¤©¤ Exceptions.c
        ©À©¤©¤ Exceptions.h
        ©À©¤©¤ IntcInterrupts.c
        ©À©¤©¤ IntcInterrupts.h
        ©À©¤©¤ MPC55xx_init.c
        ©À©¤©¤ MPC55xx_init_debug.c
        ©À©¤©¤ MPC5634M_MLQB80.h
        ©À©¤©¤ MPC5634M_MLQB80_HWInit.c
        ©À©¤©¤ MPC5634M_MLQB80_HWInit.h
        ©À©¤©¤ __ppc_eabi_init.c
        ©À©¤©¤ main.c
        ©¸©¤©¤ typedefs.h
```

To support other kind of chips, please take the MPC5634M_MLQB80 as an example and modify the SConscript and Kconfig
