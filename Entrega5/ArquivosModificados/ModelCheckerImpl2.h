/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ModelCheckerImpl2.h
 * Author: joaofel-u
 *
 * Created on 23 de outubro de 2019, 08:11
 */

#ifndef MODELCHECKERIMPL2_H
#define MODELCHECKERIMPL2_H

#include "ModelChecker_if.h"
#include "Model.h"
#include "PluginManager.h"
#include "List.h"

class ModelCheckerImpl2 : public ModelChecker_if {
public:
    ModelCheckerImpl2(Model* model);
    ModelCheckerImpl2(const ModelCheckerImpl2& orig);
    virtual ~ModelCheckerImpl2();
public:
    virtual bool checkAll();
    virtual bool checkConnected();
    virtual bool checkSymbols();
    virtual bool checkActivationCode();
    virtual bool checkLimits();
private:
    bool _recursiveConnectedTo(PluginManager* pluginManager, ModelComponent* comp, List<ModelComponent*>* visited, List<ModelComponent*>* unconnected, bool* drenoFound);
private:
    Model* _model;
};

#endif /* MODELCHECKERIMPL2_H */

