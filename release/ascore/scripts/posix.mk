# android termux command system
termux ?= no
ifeq ($(rtos),contiki)
tcpip=contiki
else
ifeq ($(host),Linux)
tcpip=lwip
else
tcpip=none
endif
endif
ifeq ($(rtos),smallos)
tcpip=none
endif
ifeq ($(rtos),ucosii)
tcpip=none
endif
ifeq ($(rtos),trampoline)
tcpip=none
endif

ifeq ($(termux),yes)
tcpip=none
#sgapp=none
def-y += -D__TERMUX__
endif

ifeq ($(host),Linux)
def-y += -D__LINUX__
# my computer is 64 bit 
def-y += -D__X86_64__
ifeq ($(rtos),trampoline)
inc-y += -I$(src-dir)/viper
def-y += -DWITH_DEBUG=YES
endif
else
def-y += -D__WINDOWS__
def-y +=  -DDYNAMIC_XLDRIVER_DLL -DPOINTER_32=
endif
# slect GUI openVG/GTK 
ifeq ($(host),Linux)
ldflags-y +=-lrt
ldflags-y +=-ldl -lreadline
else
ldflags-y +=-lwinmm -lwsock32
ldflags-y += $(src-dir)/PCANBasic.lib
endif
ifeq ($(sgapp),none)
else
cflags-y  += `pkg-config --cflags gtk+-3.0`
ldflags-y += `pkg-config --cflags gtk+-3.0` \
			 `pkg-config --libs gtk+-3.0 glib-2.0 gthread-2.0`
endif
ldflags-y += -lpthread
ldflags-y += -lstdc++ -lm

def-y += -D_LITTLE_ENDIAN
# basic
def-y += -DUSE_KERNEL -DUSE_PORT -DUSE_MCU -DUSE_DIO -DUSE_ECUM -DUSE_SCHM -DUSE_DET
# network of CAN
def-y += -DUSE_CAN -DUSE_CANIF -DUSE_PRUR -DUSE_COM -DUSE_COMM -DUSE_CANTP -DUSE_CANNM	\
		 -DUSE_DCM -DUSE_CANNM -DUSE_CANSM -DUSE_PDUR -DUSE_NM -DUSE_OSEKNM -DUSE_XCP
# inter processor communication		 
#def-y += -DUSE_IPC -DUSE_VIRTQ -DUSE_RPMSG
ifeq ($(tcpip),lwip)
def-y += -DUSE_SOAD
def-y += -DUSE_LWIP
endif
ifeq ($(tcpip),contiki)
def-y += -DUSE_CONTIKI
def-y += -DCONTIKI=1 -DCONTIKI_TARGET_MINIMAL_NET=1
endif
# memory service
def-y += -DUSE_FLS -DUSE_EEP -DUSE_EA -DUSE_FEE -DUSE_NVM -DUSE_MEMIF
dllflags-y += -Wl,--version-script=$(src-dir)/asvirtual.version
def-y += -DUSE_STMO
def-y += -D__AS_CAN_BUS__ -DAUTOSAR_SIMULATOR
ifeq ($(sgapp),none)
else
def-y += -DUSE_GUI
ifeq ($(termux),yes)
else
def-y += -DUSE_LCD
endif
ifeq ($(gui),GTK)			 
def-y += -DGUI_USE_GTK			 
else	 # openVG
def-y += -DGUI_USE_OPENVG	
def-y += -DOPENVG_STATIC_LIBRARY -D_WIN32 -DEGL_STATIC_LIBRARY
inc-y += -I$(inc-dir)/EGL
inc-y += -I$(inc-dir)/VG
endif
endif
ifeq ($(tcpip),lwip)
inc-y += -I$(src-dir)/lwip/src/include
inc-y += -I$(src-dir)/lwip/src/include/ipv4
endif
def-y += -DAUTOSTART_ENABLE

dir-y += $(src-dir)/swc/telltale

dir-y += $(download)/json-c
def-y += -DUSE_JSONC
ifeq ($(host),Linux)
else
def-y += -D_MSC_VER
endif

# for mingw need to install gnutls from ftp://ftp.gnu.org/gnu/gnutls/w32/gnutls-3.0.22-w32.zip
dir-y += $(download)/libmicrohttpd-0.9.55/src/microhttpd
inc-y += -I$(download)/libmicrohttpd-0.9.55/src/include
ifeq ($(host),Linux)
inc-y += -I$(download)/libmicrohttpd-0.9.55/
else
inc-y += -I$(download)/libmicrohttpd-0.9.55/w32/common
endif
def-y += -DUSE_MICROHTTPD -D__forceinline=inline
ldflags-y += -lgnutls

ifeq ($(compiler),posix-gcc)
#cflags-y += -Werror
COMPILER_DIR = 
include ../make/posix.mk
endif

$(download)/contiki:
ifeq (${rtos},contiki)
	@(cd $(download);git clone https://github.com/contiki-os/contiki.git;\
		cd contiki;git checkout 3.x;	\
		cd core/sys;	\
		sed -e "274c /*static*/ PT_THREAD(process_thread_##name(struct pt *process_pt, \\\\" process.h > process.h2;	\
		mv process.h2 process.h)
endif

ascontiki:$(download)/contiki

$(download)/json-c:
	@(cd $(download);git clone https://github.com/json-c/json-c.git;)
ifeq ($(host),Linux)
	@(cd $(download)/json-c;sh ./autogen.sh; ./configure; make)
else
	@(cd $(download)/json-c;cp config.h.win32 cp config.h;cp json_config.h.win32 cp json_config.h)
endif

asjson-c:$(download)/json-c

$(download)/libmicrohttpd-0.9.55.tar.gz:
	@(cd $(download);wget ftp://ftp.gnu.org/gnu/libmicrohttpd/libmicrohttpd-0.9.55.tar.gz)

$(download)/libmicrohttpd-0.9.55:$(download)/libmicrohttpd-0.9.55.tar.gz
	@(cd $(download);tar xf libmicrohttpd-0.9.55.tar.gz)

aslibmicrohttpd:$(download)/libmicrohttpd-0.9.55
ifeq ($(host),Linux)
	@(cd $(download)/libmicrohttpd-0.9.55; sh ./configure)
endif
	@(cd $(download)/libmicrohttpd-0.9.55;	\
		sed -i "28c #ifdef HTTPS_SUPPORT" src/microhttpd/connection_https.c; \
		sed -i "200c #endif /* HTTPS_SUPPORT */" src/microhttpd/connection_https.c; \
		rm src/microhttpd/test_*.c -v)

dep-posix: $(download) aslwip ascontiki dep-as-virtual asjson-c
ifeq ($(sgapp),none)
else
	@(make SG)
endif
	@(mkdir -p $(inc-dir)/arch)
	@(cd $(src-dir);$(LNFS) $(ASCONFIG))
	@(cd $(src-dir);$(LNFS) $(VIRTUAL)/source/asvirtual.version)
	@(cd $(src-dir);$(LNFS) $(ASCORE)/app FALSE)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common FALSE)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common/config FALSE)
#	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common/test FALSE)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common/rte TRUE)
	@(cd $(src-dir);mkdir -p swc; cd swc; $(LNFS) $(APPLICATION)/swc/telltale)
	@(cd $(src-dir);$(PY34) swc/telltale/swc.py)
	@(cd $(src-dir);sed -e "5c <OsRef name='$(rtos)'/>" infrastructure.xml > $(board)_infrastructure.xml)
	@(cd $(src-dir); rm infrastructure.xml)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board) TRUE)
	@(cd $(src-dir);$(LNFS) $(ASCORE)/SgDesign/SgRes TRUE)
	@(cd $(src-dir);$(LNFS) $(ASCORE)/SgDesign/$(sgapp)/src TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.h Os.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.c Os.c)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/include FALSE)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/include/sys)
ifeq ($(tcpip),lwip)
	@(cd $(src-dir);rm -f lwip_timers.c timers.c)
	@(cd $(src-dir);$(LNFS) $(download)/lwip)
	@(cd $(src-dir);$(LNFS) $(download)/lwip/src/core TRUE)
	@(cd $(src-dir);rm icmp6.c inet6.c ip6_addr.c ip6.c)
	@(cd $(src-dir);$(LNFS) $(download)/lwip/src/netif TRUE)
	@(cd $(src-dir);$(LNFS) $(download)/lwip/src/api TRUE)
	@(cd $(src-dir);rm ethernetif.c;mv timers.c lwip_timers.c)
	@(cd $(inc-dir)/arch;$(LNFS) $(INFRASTRUCTURE)/arch/posix/lwip/arch TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/posix/lwip TRUE)
endif
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/posix/mcal TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/EcuM TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/diagnostic TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/communication TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/Crc TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/memory TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/libraries TRUE)
#	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/vm TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/SchM TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/gui/Sg.h Sg.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/gui/SgDraw.h SgDraw.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/gui/Sg.c Sg.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/gui/SgDraw.c SgDraw.c)		
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/clib/cirq_buffer.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/clib/mbox.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/clib/asheap.c)
ifeq ($(rtos),smallos)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/small FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/small/portable/posix FALSE)
endif
ifeq ($(rtos),posix)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/posix TRUE)
endif
ifeq ($(rtos),contiki)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/contiki/core TRUE)
	@(cd $(src-dir);$(LNFS) $(download)/contiki/core FALSE)
	@(cd $(src-dir);$(LNFS) $(download)/contiki/core/lib FALSE)
	@(cd $(src-dir);$(LNFS) $(download)/contiki/core/sys TRUE)
	@(cd $(src-dir);$(LNFS) $(download)/contiki/core/net FALSE)
	@(cd $(src-dir);$(LNFS) $(download)/contiki/core/net/mac TRUE)
	@(cd $(src-dir);$(LNFS) $(download)/contiki/core/net/rime TRUE)
	@(cd $(src-dir);$(LNFS) $(download)/contiki/core/net/rpl TRUE)
	@(cd $(src-dir);$(LNFS) $(download)/contiki/core/cfs)
	@(cd $(src-dir);$(LNFS) $(download)/contiki/core/sys)
	@(cd $(src-dir);$(LNFS) $(download)/contiki/core/lib)
	@(cd $(src-dir);$(LNFS) $(download)/contiki/core/net)
	@(cd $(src-dir);$(LNFS) $(download)/contiki/core/net/rpl)
	@(cd $(src-dir);$(LNFS) $(download)/contiki/core/dev)
	@(cd $(src-dir);$(LNFS) $(download)/contiki/cpu/native TRUE)
	@(cd $(src-dir);$(LNFS) $(download)/contiki/platform/minimal-net TRUE)
	@(cd $(src-dir); rm contiki-main.c ctk-*.c rime-udp.c wpcap.c wpcap-drv.c slipdev.c sensors.c libconio.c)
endif
ifeq ($(rtos),atk2-sc4)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers-atk/atk2-sc4/include TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers-atk/atk2-sc4/kernel TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers-atk/atk2-sc4/portable/posix FALSE)
endif
ifeq ($(rtos),freeosek)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freeosek/inc TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freeosek/src FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freeosek/gen/inc FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freeosek/gen/src FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freeosek/OpenGEN)
ifeq ($(host),Linux)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freeosek/src/posix TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freeosek/gen/inc/posix FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freeosek/gen/src/posix FALSE)
else	
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freeosek/src/win TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freeosek/gen/inc/win FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freeosek/gen/src/win FALSE)
endif
endif
ifeq ($(rtos),freertos)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freertos/source FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freertos/source/include FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freertos/source/portable/MemMang FALSE)
ifeq ($(host),Linux)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freertos/source/portable/GCC/Posix FALSE)
else
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freertos/source/portable/MSVC-MingW FALSE)
endif
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freertos/config TRUE)
	@(cd $(src-dir);rm -f heap_1.c heap_2.c heap_4.c heap_5.c)
endif
ifeq ($(rtos),ucosii)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/ucos_ii/core ucosii)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/ucos_ii/core/ucos_ii.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/ucos_ii/core/os_dbg_r.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/small/os_i.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/ucos_ii FALSE)
ifeq ($(host),Linux)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/ucos_ii/portable/posix TRUE)
else
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/ucos_ii/portable/win32 FALSE)
endif
endif
ifeq ($(rtos),toppers_osek)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/include TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/kernel TRUE)
ifeq ($(host),Linux)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/portable/posix TRUE)
else	
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/portable/mingw TRUE)
endif	
endif
ifeq ($(rtos),trampoline)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/trampoline/os TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/trampoline/debug TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/trampoline/autosar TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/trampoline/machines/posix FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/trampoline/machines/posix/viper)
	@(cd $(src-dir);rm tpl_os_stm_kernel.c)
	pgrep viper|xargs -i kill -9 {}
	@(cd $(src-dir)/viper; make; cp viper $(CURDIR) -v)
endif
ifeq ($(gui),openVG)	
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/posix/openvg/vg FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/posix/openvg/vg/win32 FALSE)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/system/gui/openvg/include/VG)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/system/gui/openvg/include/EGL)
endif
	@(cd $(src-dir);$(LNFS) $(LUA)/can TRUE)
	@(cd $(src-dir);$(LNFS) $(prj-dir)/com/as.tool/cancasexl.access/vxlapi.h)
	@(cd $(src-dir);$(LNFS) $(prj-dir)/com/as.tool/cancasexl.access/xlLoadlib.cpp xlLoadlib.c)
ifeq ($(host),Linux)
else
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/clib/strtok_r.c)
	@(cd $(src-dir);cp C:/Users/parai.wang/Downloads/peak-can/PCAN-Basic\ API/Win32/VC_LIB/PCANBasic.lib PCANBasic.lib)
endif
	@(cd $(src-dir);rm -f Dem.c J1939Tp* Lin*)
ifeq ($(tcpip),contiki)
	@(cd $(src-dir);rm -f lwip.xml)
endif
	@(make BSW)
	@(echo "  >> prepare link for posix done")

