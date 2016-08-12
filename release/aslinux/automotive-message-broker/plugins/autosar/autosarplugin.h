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
#include <map>
#include <memory>
#include <tgmath.h>
#include <libwebsockets.h>
#include <json.h>

#include <ambpluginimpl.h>
/* ============================ [ MACROS    ] ====================================================== */

/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ CLASS     ] ====================================================== */
class AUTOSARPlugin: public AmbPluginImpl {

public:

	/*!
	 * \param re AbstractRoutingEngine
	 * \param config Map of the configuration string values loaded on startup from AMB configuration file
	 * \param parent AmbPlugin instance
	 */
	AUTOSARPlugin(AbstractRoutingEngine* re, const std::map<std::string, std::string>& config, AbstractSource &parent);
	virtual ~AUTOSARPlugin();

	/* from AbstractSink */
public:

	/*! uuid() is a unique identifier of the plugin
	 * @return a guid-style unique identifier
	 *
	 * use python::uuid.uuid5(uuid.NAMESPACE_DNS, 'as.autosar.parai')
	 */
	const std::string uuid() const { return "21e16c41-5d79-5e2c-a088-3e8e68107aa8"; }

	/*! propertyChanged is called when a subscribed to property changes.
	  * @see AbstractRoutingEngine::subscribeToPropertyChanges()
	  * \param value value of the property that changed. this is a temporary pointer that will be destroyed.
	  * Do not destroy it.  If you need to store the value use value.anyValue(), value.value<T>() or
	  * value->copy() to copy.
	  */
	void propertyChanged(AbstractPropertyType* value);

	AsyncPropertyReply* setProperty(const AsyncSetPropertyRequest &request);

	/* from AUTOSAR */
public:



private:

};
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
#endif /* AUTOSARPLUGIN_H_ */
