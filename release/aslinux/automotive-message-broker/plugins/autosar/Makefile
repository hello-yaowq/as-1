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
prj-dir = $(subst /release/aslinux/automotive-message-broker/plugins/autosar,,$(CURDIR))

INFRASTRUCTURE = $(prj-dir)/com/as.infrastructure
APPLICATION = $(prj-dir)/com/as.application
ASCONFIG = $(prj-dir)/com/as.tool/config.infrastructure.system
PY34 = python3
LNFS  = $(PY34) $(prj-dir)/release/make/lnfs.py
XCC   = $(PY34) xcc.py
STUDIO = $(PY34) studio.py

src-dir = $(CURDIR)

BSW:	
	@(cd $(src-dir)/config.infrastructure.system;$(XCC) $(src-dir) true)

studio:
	@(cd $(src-dir)/config.infrastructure.system;$(STUDIO) $(src-dir))	

dep:
	@(cd $(src-dir);$(LNFS) $(ASCONFIG))
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/include FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/communication/CanIf TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/communication/PduR TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/communication/Com TRUE)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common/config/CanIf_SpecialPdus.h)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common/amb.arxml autosar.arxml)
	@(cd $(src-dir);make BSW)
