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

#include <pthread.h>

#include "autosarplugin.h"

using namespace std;
/* ============================ [ MACROS    ] ====================================================== */

/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
extern "C" void* EcuM_Init(void*);
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
static gboolean timeoutCallback(gpointer data)
{
	AUTOSARPlugin* src = (AUTOSARPlugin*)data;

	src->MainFunction();

	return true;
}
/* ============================ [ FUNCTIONS ] ====================================================== */
extern "C" void StartupHook(void)
{

}
/* library exported function for plugin loader */
extern "C" void create(AbstractRoutingEngine* routingengine, std::map<std::string, std::string> config)
{
	new AUTOSARPlugin(routingengine, config);
}


AUTOSARPlugin::AUTOSARPlugin(AbstractRoutingEngine* re, map<string, string> config) :
	AbstractSource(re, config)
{
	debugOut("setting timeout");

	int delay = 1000;

	if(config.find("delay") != config.end())
	{
		delay = boost::lexical_cast<int>(config["delay"]);
	}

	g_timeout_add(delay, timeoutCallback, this );

	addPropertySupport(VehicleProperty::EngineSpeed, Zone::None);
	addPropertySupport(VehicleProperty::VehicleSpeed, Zone::None);

	pthread_create((pthread_t*)&(thread),NULL,EcuM_Init,NULL);

	DebugOut()<<"AUTOSARPlugin: AS COM STACK ON LINE!"<<endl;
}

AUTOSARPlugin::~AUTOSARPlugin()
{

}

AsyncPropertyReply* AUTOSARPlugin::setProperty(AsyncSetPropertyRequest request)
{
	AsyncPropertyReply *reply = new AsyncPropertyReply(request);
	reply->success = false;

	reply->error = AsyncPropertyReply::InvalidOperation;
	reply->completed(reply);
	return reply;
}

void AUTOSARPlugin::subscribeToPropertyChanges(VehicleProperty::Property property)
{
	mRequests.push_back(property);
}

void AUTOSARPlugin::unsubscribeToPropertyChanges(VehicleProperty::Property property)
{
	if(contains(mRequests,property))
		removeOne(&mRequests, property);
}

void AUTOSARPlugin::MainFunction()
{
	static VehicleProperty::VehicleSpeedType vel;

	vel.setValue(240);
	vel.sequence++;

	routingEngine->updateProperty(&vel, uuid());
}
void AUTOSARPlugin::getRangePropertyAsync(AsyncRangePropertyReply *reply)
{

}

void AUTOSARPlugin::getPropertyAsync(AsyncPropertyReply *reply)
{
	DebugOut()<<"AUTOSARPlugin: getPropertyAsync called for property: "<<reply->property<<endl;
	if(reply->property == VehicleProperty::VehicleSpeed)
	{
		VehicleProperty::VehicleSpeedType temp(120);
		reply->value = &temp;
		reply->success = true;
		reply->completed(reply);
	}
	else if(reply->property == VehicleProperty::EngineSpeed)
	{
		VehicleProperty::EngineSpeedType temp(120);
		reply->value = &temp;
		reply->success = true;
		reply->completed(reply);
	}
	else
	{
		reply->success=false;
		reply->error = AsyncPropertyReply::InvalidOperation;
		reply->completed(reply);
	}
}

PropertyList AUTOSARPlugin::supported()
{
	//DebugOut()<<"AUTOSARPlugin: supported " <<endl;
	return mSupported;
}

int AUTOSARPlugin::supportedOperations()
{
	DebugOut()<<"AUTOSARPlugin: supportedOperations " <<endl;
	return Get | Set | GetRanged;;
}

void AUTOSARPlugin::addPropertySupport(VehicleProperty::Property property, Zone::Type zone)
{
	mSupported.push_back(property);

	Zone::ZoneList zones;

	zones.push_back(zone);

	PropertyInfo info(0, zones);

	propertyInfoMap[property] = info;
}
