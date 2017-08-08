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
