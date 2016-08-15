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
#ifndef AUTOSARPLUGIN_H_
#define AUTOSARPLUGIN_H_
/* ============================ [ INCLUDES  ] ====================================================== */
#include <abstractsource.h>
#include <string>
/* ============================ [ MACROS    ] ====================================================== */
using namespace std;
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ CLASS     ] ====================================================== */
class AUTOSARPlugin: public AbstractSource {

public:

	AUTOSARPlugin(): AbstractSource(nullptr, map<string, string>()) {}
	AUTOSARPlugin(AbstractRoutingEngine* re, map<string, string> config);
	virtual ~AUTOSARPlugin();

	/* from AbstractSink */
public:

	/*! uuid() is a unique identifier of the plugin
	 * @return a guid-style unique identifier
	 *
	 * use python::uuid.uuid5(uuid.NAMESPACE_DNS, 'as.autosar.parai')
	 */
	const string uuid() { return "21e16c41-5d79-5e2c-a088-3e8e68107aa8"; }

	void getPropertyAsync(AsyncPropertyReply *reply);
	void getRangePropertyAsync(AsyncRangePropertyReply *reply);
	AsyncPropertyReply * setProperty(AsyncSetPropertyRequest request);
	void subscribeToPropertyChanges(VehicleProperty::Property property);
	void unsubscribeToPropertyChanges(VehicleProperty::Property property);
	PropertyList supported();

	int supportedOperations();

	void supportedChanged(const PropertyList &) {}

	PropertyInfo getPropertyInfo(const VehicleProperty::Property & property)
	{
		if(propertyInfoMap.find(property) != propertyInfoMap.end())
			return propertyInfoMap[property];

		return PropertyInfo::invalid();
	}
	/* from AUTOSAR */
public:
	void MainFunction(void);

private:
	void* thread;
	PropertyList mSupported;
	PropertyList mRequests;
	std::map<VehicleProperty::Property, PropertyInfo> propertyInfoMap;
	std::map<Zone::Type, bool> acStatus;

private:


private:
	void addPropertySupport(VehicleProperty::Property property, Zone::Type zone);
};
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
#endif /* AUTOSARPLUGIN_H_ */
