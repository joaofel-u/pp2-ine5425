/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PluginConnectorDummyImpl2.cpp
 * Author: joaofel-u
 *
 * Created on 23 de outubro de 2019, 07:43
 */

#include "PluginConnectorDummyImpl2.h"

// Model Components
#include "Assign.h"
#include "Create.h"
#include "Decide.h"
#include "Delay.h"
#include "Dispose.h"
#include "Enter.h"
#include "Hold.h"
#include "Leave.h"
#include "Record.h"
#include "Remove.h"
#include "Route.h"
#include "Seize.h"
#include "Separate.h"
#include "Signal.h"
#include "Release.h"

// Model elements
#include "Attribute.h"
#include "Counter.h"
#include "EntityType.h"
#include "Queue.h"
#include "Resource.h"
#include "Variable.h"
#include "ProbDistrib.h"
#include "EntityGroup.h"
#include "Station.h"
#include "Formula.h"
#include "ODE.h"

#include "Util.h"

PluginConnectorDummyImpl2::PluginConnectorDummyImpl2() {
}

PluginConnectorDummyImpl2::PluginConnectorDummyImpl2(const PluginConnectorDummyImpl2& orig) {
}

PluginConnectorDummyImpl2::~PluginConnectorDummyImpl2() {
}

Plugin* PluginConnectorDummyImpl2::check(const std::string dynamicLibraryFilename) {
    return nullptr; /**@ TODO:To implement */
}

Plugin* PluginConnectorDummyImpl2::connect(const std::string dynamicLibraryFilename) {
    std::string fn = getFileName(dynamicLibraryFilename);
    StaticGetPluginInformation GetInfo = nullptr;
    Plugin* pluginResult = nullptr;
    /* Model elements. */
    if (fn == "attribute.so")
	GetInfo = &Attribute::GetPluginInformation;
    else if (fn == "counter.so")
	GetInfo = &Counter::GetPluginInformation;
    else if (fn == "entitytype.so")
	GetInfo = &EntityType::GetPluginInformation;
    else if (fn == "queue.so")
	GetInfo = &Queue::GetPluginInformation;
    else if (fn == "resource.so")
	GetInfo = &Resource::GetPluginInformation;
    else if (fn == "statisticscollector.so")
	GetInfo = &StatisticsCollector::GetPluginInformation;
    else if (fn == "variable.so")
	GetInfo = &Variable::GetPluginInformation;
    else if (fn == "entitygroup.so")
	GetInfo = &EntityGroup::GetPluginInformation;
    else if (fn == "station.so")
	GetInfo = &Station::GetPluginInformation;
    else if (fn == "formula.so")
	GetInfo = &Formula::GetPluginInformation;
    else if (fn == "ode.so")
	GetInfo = &ODE::GetPluginInformation;
    /* Model components. */
    else if (fn == "assign.so")
	GetInfo = &Assign::GetPluginInformation;
    else if (fn == "create.so")
	GetInfo = &Create::GetPluginInformation;
    else if (fn == "decide.so")
	GetInfo = &Decide::GetPluginInformation;
    else if (fn == "delay.so")
	GetInfo = &Delay::GetPluginInformation;
    else if (fn == "dispose.so")
	GetInfo = &Dispose::GetPluginInformation;
    else if (fn == "enter.so")
	GetInfo = &Enter::GetPluginInformation;
    else if (fn == "hold.so")
	GetInfo = &Hold::GetPluginInformation;
    else if (fn == "leave.so")
	GetInfo = &Leave::GetPluginInformation;
    else if (fn == "record.so")
	GetInfo = &Record::GetPluginInformation;
    else if (fn == "remove.so")
	GetInfo = &Remove::GetPluginInformation;
    else if (fn == "release.so")
	GetInfo = &Release::GetPluginInformation;
    else if (fn == "route.so")
	GetInfo = &Route::GetPluginInformation;
    else if (fn == "seize.so")
	GetInfo = &Seize::GetPluginInformation;
    else if (fn == "separate.so")
	GetInfo = &Separate::GetPluginInformation;
    else if (fn == "signal.so")
	GetInfo = &Signal::GetPluginInformation;
    //else if (fn=="")

    if (GetInfo != nullptr) {
	pluginResult = new Plugin(GetInfo);
    }
    return pluginResult;
}

bool PluginConnectorDummyImpl2::disconnect(const std::string dynamicLibraryFilename) {
    return true;
}

bool PluginConnectorDummyImpl2::disconnect(Plugin* plugin) {
    return true;
}

