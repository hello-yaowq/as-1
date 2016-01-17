#/**
# * AS - the open source Automotive Software on https://github.com/parai
# *
# * Copyright (C) 2015  AS <parai@foxmail.com>
# *
# * This source code is free software; you can redistribute it and/or modify it
# * under the terms of the GNU General Public License version 2 as published by the
# * Free Software Foundation; See <http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt>.
# *
# * This program is distributed in the hope that it will be useful, but
# * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# * for more details.
# */
#common compilers
AS  = $(COMPILER_DIR)/bin/$(COMPILER_PREFIX)gcc
CC  = $(COMPILER_DIR)/bin/$(COMPILER_PREFIX)gcc
LD  = $(COMPILER_DIR)/bin/$(COMPILER_PREFIX)ld
AR  = $(COMPILER_DIR)/bin/$(COMPILER_PREFIX)ar
CS  = $(COMPILER_DIR)/bin/$(COMPILER_PREFIX)objdump
S19 = $(COMPILER_DIR)/bin/$(COMPILER_PREFIX)objcopy -O srec --srec-forceS3 --srec-len 32
BIN = $(COMPILER_DIR)/bin/$(COMPILER_PREFIX)objcopy -O binary

RM  = rm

#common flags
asflags-y += -mcpu=arm920t
cflags-y  += -mcpu=arm920t -std=gnu99
cflags-y  += -mstructure-size-boundary=8 -ffreestanding
cflags-y  += -W -Wall

ldflags-y += -Map $(exe-dir)/$(target-y).map

ifeq ($(debug),true)
cflags-y += -g -O0
asflags-y += -g -O0
else
cflags-y += -O2
asflags-y += -O2
endif
#remove unused code and data to save ROM/RAM usage
cflags-y += -ffunction-sections -fdata-sections
ldflags-y += --gc-sections
# supress printf_chk memcpy_chk and so on
cflags-y += -U_FORTIFY_SOURCE

ldflags-y += -static -T $(link-script)
dir-y += $(src-dir)

VPATH += $(dir-y)
inc-y += $(foreach x,$(dir-y),$(addprefix -I,$(x)))	
	
obj-y += $(patsubst %.c,$(obj-dir)/%.o,$(foreach x,$(dir-y),$(notdir $(wildcard $(addprefix $(x)/*,.c)))))		
obj-y += $(patsubst %.S,$(obj-dir)/%.o,$(foreach x,$(dir-y),$(notdir $(wildcard $(addprefix $(x)/*,.S)))))		
ofj-y += $(patsubst %.of,$(src-dir)/%.h,$(foreach x,$(dir-y),$(notdir $(wildcard $(addprefix $(x)/*,.of)))))
#common rules	

# used to generate member offset in a struct 
$(src-dir)/%.h:%.of
	@echo
	@echo "  >> CC $(notdir $<)"
	@cp -v $< $(patsubst %.h,%.c,$@)
	@$(CC) -S $(patsubst %.h,%.c,$@) -o $@h $(cflags-y) $(inc-y) $(def-y)
	@sed -n '/#define/p' $@h > $@
	@rm $@h $(patsubst %.h,%.c,$@)
	
$(obj-dir)/%.o:%.S
	@echo
	@echo "  >> AS $(notdir $<)"	
	@$(AS) $(asflags-y) $(def-y) $(inc-y) -o $@ -c $<
	
$(obj-dir)/%.o:%.c
	@echo
	@echo "  >> CC $(notdir $<)"
	@gcc -c $(inc-y) $(def-y) -MM -MF $(patsubst %.o,%.d,$@) -MT $@ $<	
	@$(CC) $(cflags-y) $(inc-y) $(def-y) -o $@ -c $<	
	
	@$(CS) -D $@ > $@.s
	
ifeq ($(host), Linux)
include $(wildcard $(obj-dir)/*.d)
else
-include $(obj-dir)/as.dep
endif
	
.PHONY:all clean

$(obj-dir):
	@mkdir -p $(obj-dir)
	
$(exe-dir):
	@mkdir -p $(exe-dir)	

include $(wildcard $(obj-dir)/*.d)

exe:$(obj-dir) $(exe-dir) $(ofj-y) $(obj-y) 
	@echo "  >> LD $(target-y).exe"
	@$(LD) $(obj-y) $(ldflags-y) -o $(exe-dir)/$(target-y).exe 
	@$(S19) $(exe-dir)/$(target-y).exe  $(exe-dir)/$(target-y).s19
	@$(BIN) $(exe-dir)/$(target-y).exe  $(exe-dir)/$(target-y).bin
	@echo ">>>>>>>>>>>>>>>>>  BUILD $(exe-dir)/$(target-y)  DONE   <<<<<<<<<<<<<<<<<<<<<<"	
	
dll:$(obj-dir) $(exe-dir) $(obj-y)
	@echo "  >> LD $(target-y).DLL"
	@$(CC) -shared $(obj-y) $(ldflags-y) -o $(exe-dir)/$(target-y).dll 
	@echo ">>>>>>>>>>>>>>>>>  BUILD $(exe-dir)/$(target-y)  DONE   <<<<<<<<<<<<<<<<<<<<<<"

lib:$(obj-dir) $(exe-dir) $(obj-y)
	@echo "  >> LD $(target-y).LIB"
	@$(AR) -r $(exe-dir)/lib$(target-y).a $(obj-y)  
	@echo ">>>>>>>>>>>>>>>>>  BUILD $(exe-dir)/$(target-y)  DONE   <<<<<<<<<<<<<<<<<<<<<<"		

clean-obj:
	@rm -fv $(obj-dir)/*
	@rm -fv $(exe-dir)/*
	
clean-obj-src:clean-obj
	@rm -fv $(src-dir)/*

