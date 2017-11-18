# on C disk do the following command:

$(inc-dir)/utility:
	@mkdir -p $@
$(inc-dir):
	@mkdir -p $@
$(src-dir):
	@mkdir -p $@

$(ASCORE)/SgDesign/SgRes/SgRes.h:
	@(cd SgDesign;$(SG) $(sgapp)/Sg.xml)	

SG:
	@(cd SgDesign;$(SG) $(sgapp)/Sg.xml)

OS:
	@(cd $(src-dir)/config.infrastructure.system;$(XCC) $(src-dir) false)	
ifeq ($(rtos),freeosek)
# as the bug of FreeOSEK generator
	@(mv -v srcme/parai/workspace/as/release/ascore/src/* $(src-dir))
	@(rm -fvr srcme/parai/workspace/as/release/ascore/src)	
endif

BSW:
	@(cd $(src-dir)/config.infrastructure.system;$(XCC) $(src-dir) true)

studio:
	@(cd $(src-dir)/config.infrastructure.system;$(STUDIO) $(src-dir))	

dep-as-virtual:
	@($(LNFS) $(VIRTUAL) virtual)

$(download):
	@mkdir -p $@

$(download)/lwip:
ifeq ($(tcpip),lwip)
	@(cd $(download);git clone git://git.savannah.nongnu.org/lwip.git;cd lwip;git checkout STABLE-1_4_1)
endif

$(download)/lwip-contrib:
ifeq ($(tcpip),lwip)
	@(cd $(download);git clone git://git.savannah.nongnu.org/lwip/lwip-contrib.git;cd lwip-contrib;git checkout STABLE-1_4_1)
endif

aslwip: $(download)/lwip $(download)/lwip-contrib

$(download)/rt-thread:
	@(cd $(download); git clone https://github.com/RT-Thread/rt-thread.git)

$(download)/WpdPack_4_1_2.zip:
	@(cd $(download);wget https://www.winpcap.org/install/bin/WpdPack_4_1_2.zip)

$(download)/WpdPack:
	@(cd $(download); unzip $(download)/WpdPack_4_1_2.zip)

ifeq ($(host), Linux)
dep-wincap=
else
dep-wincap=$(download)/WpdPack
endif

$(download)/qemu/hw/char/libpyas.a:
	(cd $(prj-dir)/release/aslua; make 81; make 82 forceclib=yes)

$(download)/qemu: $(download)/qemu/hw/char/libpyas.a $(dep-wincap)
	@(cd $(download); git clone https://github.com/qemu/qemu.git; \
		cd qemu; git submodule update --init dtc ; \
		cd hw/char; $(LNFS) $(prj-dir)/com/as.tool/qemu/hw/char TRUE; \
		cp $(prj-dir)/release/aslua/out/libpyas.a .; \
		cat Makefile >> Makefile.objs)

asqemu:$(download)/qemu
	@(cd $(download)/qemu; ./configure; make)

$(src-dir)/pci.download.done:
ifeq ($(usepci),yes)
	@(cd $(src-dir);wget https://raw.githubusercontent.com/torvalds/linux/v4.8/include/uapi/linux/pci.h -O pci.h)
	@(cd $(src-dir);wget https://raw.githubusercontent.com/torvalds/linux/v4.8/include/uapi/linux/pci_regs.h -O pci_regs.h)
	@(cd $(src-dir);wget https://raw.githubusercontent.com/torvalds/linux/v4.8/include/linux/pci_ids.h -O pci_ids.h)
	@(cd $(src-dir);sed -i "20c #include \"pci_regs.h\"" pci.h)
endif
	@touch $@

$(download)/contiki:
ifeq (${rtos},contiki)
	@(cd $(download);git clone https://github.com/contiki-os/contiki.git;\
		cd contiki;git checkout 3.x;	\
		cd core/sys;	\
		sed -e "274c /*static*/ PT_THREAD(process_thread_##name(struct pt *process_pt, \\\\" process.h > process.h2;	\
		mv process.h2 process.h)
endif

ascontiki:$(download)/contiki

$(LUA)/device/websock/out/libaws.a:
	@(cd $(LUA)/device/websock; make all)

aslibaws:$(LUA)/device/websock/out/libaws.a

$(INFRASTRUCTURE)/system/fs/out/libff13-$(board).a:
ifeq ($(EMAIL),parai@foxmail.com)
else
	@(cd $(INFRASTRUCTURE)/system/fs; make all target=ff13 board=$(board))
endif

$(INFRASTRUCTURE)/system/fs/out/liblwext4-$(board).a:
ifeq ($(EMAIL),parai@foxmail.com)
else
	@(cd $(INFRASTRUCTURE)/system/fs; make all target=lwext4 board=$(board))
endif

aslibfatfs:$(INFRASTRUCTURE)/system/fs/out/libff13-$(board).a $(INFRASTRUCTURE)/system/fs/out/liblwext4-$(board).a
	