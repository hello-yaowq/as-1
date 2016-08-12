/**
 * AS - the open source Automotive Software on https://github.com/parai
 *
 * Copyright (C) 2016  AS <parai@foxmail.com>
 *
 * This source code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by the
 * Free Software Foundation; See <http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */
/* ============================ [ INCLUDES  ] ====================================================== */
#include <boost/assert.hpp>
#include <glib.h>
#include <deque>

#include <vehicleproperty.h>
#include <listplusplus.h>

#include <logger.h>
#include <ambplugin.h>

#include "autosarplugin.h"

/* ============================ [ MACROS    ] ====================================================== */

/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
extern "C" void EcuM_Init(void);
/* ============================ [ DATAS     ] ====================================================== */
static AmbPlugin<AUTOSARPlugin> * plugin = NULL;
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
extern "C" void StartupHook(void)
{

}
/* library exported function for plugin loader */
extern "C" void create(AbstractRoutingEngine* routingengine, std::map<std::string, std::string> config)
{
	plugin = new AmbPlugin<AUTOSARPlugin>(routingengine, config);

	EcuM_Init();
}


AUTOSARPlugin::AUTOSARPlugin(AbstractRoutingEngine* re, const map<string, string>& config, AbstractSource& parent) :
	AmbPluginImpl(re, config, parent)
{

}

AUTOSARPlugin::~AUTOSARPlugin()
{

}

void AUTOSARPlugin::propertyChanged(AbstractPropertyType* value)
{

}

AsyncPropertyReply* AUTOSARPlugin::setProperty(const AsyncSetPropertyRequest &request)
{
	return NULL;
}
