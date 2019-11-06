/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PluginConnectorDummyImpl2.h
 * Author: joaofel-u
 *
 * Created on 23 de outubro de 2019, 07:41
 */

#ifndef PLUGINCONNECTORDUMMYIMPL2_H
#define PLUGINCONNECTORDUMMYIMPL2_H

#include "PluginConnector_if.h"

class PluginConnectorDummyImpl2 : public PluginConnector_if {
public:
    PluginConnectorDummyImpl2();
    PluginConnectorDummyImpl2(const PluginConnectorDummyImpl2& orig);
    virtual ~PluginConnectorDummyImpl2();
public:
    virtual Plugin* check(const std::string dynamicLibraryFilename);
    virtual Plugin* connect(const std::string dynamicLibraryFilename);
    virtual bool disconnect(const std::string dynamicLibraryFilename);
    virtual bool disconnect(Plugin* plugin);
private:

};

#endif /* PLUGINCONNECTORDUMMYIMPL2_H */

