#common compilers
AS  = gcc
CC  = gcc
LD  = gcc
AR  = ar
RM  = rm

#common flags
cflags-y   += -c -g  -O2 -Wall
ldflags-y  += -g  -O2 -Wall	

dir-y += $(src-dir)

VPATH += $(dir-y)
inc-y += $(foreach x,$(dir-y),$(addprefix -I,$(x)))

ldflags-y += -Wl,-Map,$(exe-dir)/$(target-y).map	
	
obj-y += $(patsubst %.c,$(obj-dir)/%.o,$(foreach x,$(dir-y),$(notdir $(wildcard $(addprefix $(x)/*,.c)))))	
obj-y += $(patsubst %.cpp,$(obj-dir)/%.o,$(foreach x,$(dir-y),$(notdir $(wildcard $(addprefix $(x)/*,.cpp)))))	


#common rules
$(obj-dir)/%.o:%.cpp
	@echo
	@echo "  >> CC $(notdir $<)"
	@$(CC) $(cflags-y) $(inc-y) $(def-y) -MM -MF $(patsubst %.o,%.d,$@) -MT $@ $<
	@$(CC) $(cflags-y) $(inc-y) $(def-y) -o $@ $<
	
$(obj-dir)/%.o:%.c
	@echo
	@echo "  >> CC $(notdir $<)"
	@$(CC) $(cflags-y) $(inc-y) $(def-y) -MM -MF $(patsubst %.o,%.d,$@) -MT $@ $<
	@$(CC) $(cflags-y) $(inc-y) $(def-y) -o $@ $<	
	
.PHONY:all clean

$(obj-dir):
	@mkdir -p $(obj-dir)
	
$(exe-dir):
	@mkdir -p $(exe-dir)	

include $(wildcard $(obj-dir)/*.d)

exe:$(obj-dir) $(exe-dir) $(obj-y)
	@echo "  >> LD $(target-y).EXE"
	@$(LD) $(obj-y) $(ldflags-y) -o $(exe-dir)/$(target-y).exe 
	@echo ">>>>>>>>>>>>>>>>>  BUILD $(exe-dir)/$(target-y)  DONE   <<<<<<<<<<<<<<<<<<<<<<"	
	
dll:$(obj-dir) $(exe-dir) $(obj-y)
	@echo "  >> LD $(target-y).DLL"
	@$(CC) -shared $(obj-y) $(ldflags-y) -o $(exe-dir)/$(target-y).dll 
	@echo ">>>>>>>>>>>>>>>>>  BUILD $(exe-dir)/$(target-y)  DONE   <<<<<<<<<<<<<<<<<<<<<<"

lib:$(obj-dir) $(exe-dir) $(obj-y)
	@echo "  >> LD $(target-y).LIB"
	@$(AR) -r $(exe-dir)/lib$(target-y).a $(obj-y)  
	@echo ">>>>>>>>>>>>>>>>>  BUILD $(exe-dir)/$(target-y)  DONE   <<<<<<<<<<<<<<<<<<<<<<"		
clean:
	@rm -fv $(obj-dir)/*
	@rm -fv $(exe-dir)/*
	@rm -fv $(src-dir)/*
