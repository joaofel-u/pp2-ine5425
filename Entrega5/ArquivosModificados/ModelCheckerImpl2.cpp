/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ModelCheckerImpl12.cpp
 * Author: joaofel-u
 * 
 * Created on 23 de outubro de 2019, 08:12
 */

#include "ModelCheckerImpl2.h"
#include "SourceModelComponent.h"
#include "SinkModelComponent.h"
#include "ComponentManager.h"
#include "Simulator.h"

#include <assert.h>

ModelCheckerImpl2::ModelCheckerImpl2(Model* model) {
    _model = model;
}

ModelCheckerImpl2::ModelCheckerImpl2(const ModelCheckerImpl2& orig) {
}

ModelCheckerImpl2::~ModelCheckerImpl2() {
}

bool ModelCheckerImpl2::checkAll() {
    bool res = true;
    res &= checkSymbols();
    res &= checkLimits();
    res &= checkConnected();
    return res;
}

bool ModelCheckerImpl2::_recursiveConnectedTo(PluginManager* pluginManager, ModelComponent* comp, List<ModelComponent*>* visited, List<ModelComponent*>* unconnected, bool* drenoFound) {
    visited->insert(comp);
    _model->getTraceManager()->trace(Util::TraceLevel::mostDetailed, "Connected to component " + comp->getName());
    Plugin* plugin = pluginManager->find(comp->getTypename());
    assert(plugin!= nullptr);
    if (plugin->getPluginInfo()->isSink() || (plugin->getPluginInfo()->isSendTransfer() && comp->getNextComponents()->size()==0)) {//(dynamic_cast<SinkModelComponent*> (comp) != nullptr) { 
	// it is a sink OR it can send entities throught a transfer and has no nextConnections
	*drenoFound = true;
    } else { // it is not a sink
	if (comp->getNextComponents()->size() == 0) {
	    unconnected->insert(comp);
	    _model->getTraceManager()->trace(Util::TraceLevel::errors, "Component \"" + comp->getName() + "\" is unconnected (not a sink with no next componentes connected to)");
	    *drenoFound = false;
	} else {
	    ModelComponent* nextComp;
	    for (std::list<Connection*>::iterator it = comp->getNextComponents()->getList()->begin(); it != comp->getNextComponents()->getList()->end(); it++) {
		nextComp = (*it)->first;
		if (visited->find(nextComp) == visited->getList()->end()) { // not visited yet
		    *drenoFound = false;
		    Util::IncIndent();
		    this->_recursiveConnectedTo(pluginManager, nextComp, visited, unconnected, drenoFound);
		    Util::DecIndent();
		} else {
		    Util::IncIndent();
	            _model->getTraceManager()->trace(Util::TraceLevel::mostDetailed, "Connected to component " + nextComp->getName());
		    Util::DecIndent();
		    *drenoFound = true;
		}
	    }
	}
    }
}


bool ModelCheckerImpl2::checkConnected() {
    /* TODO +-: not implemented yet */
    _model->getTraceManager()->trace(Util::TraceLevel::blockArrival, "Checking connected");
    bool resultAll = true;
    PluginManager* pluginManager = this->_model->getParentSimulator()->getPluginManager();
    Plugin* plugin;
    Util::IncIndent();
    {
	List<ModelComponent*>* visited = new List<ModelComponent*>();
	List<ModelComponent*>* unconnected = new List<ModelComponent*>();
	ModelComponent* comp;
	for (std::list<ModelComponent*>::iterator it = _model->getComponentManager()->begin(); it != _model->getComponentManager()->end(); it++) {
	    comp = (*it);
	    plugin = pluginManager->find(comp->getTypename());
	    assert(plugin != nullptr);
	    if (plugin->getPluginInfo()->isSource() || plugin->getPluginInfo()->isReceiveTransfer()) { //(dynamic_cast<SourceModelComponent*> (comp) != nullptr) {
		// it is a source component OR it can receive enetities from transfer
		bool drenoFound = false;
		_recursiveConnectedTo(pluginManager, comp, visited, unconnected, &drenoFound);
	    }
	}
	// check if any component remais unconnected
	for (std::list<ModelComponent*>::iterator it = _model->getComponentManager()->begin(); it != _model->getComponentManager()->end(); it++) {
	    comp = (*it);
	    if (visited->find(comp) == visited->getList()->end()) { //not found
		resultAll = false;
		_model->getTraceManager()->trace(Util::TraceLevel::errors, "Component \"" + comp->getName() + "\" is unconnected.");
	    }
	}

    }
    Util::DecIndent();
    return resultAll;
}


bool ModelCheckerImpl2::checkSymbols() {
    bool res = true;
    _model->getTraceManager()->trace(Util::TraceLevel::blockArrival, "Checking symbols");
    Util::IncIndent();
    {
	// check components
	_model->getTraceManager()->trace(Util::TraceLevel::blockInternal, "Components:");
	Util::IncIndent();
	{
	    //List<ModelComponent*>* components = _model->getComponents();
	    for (std::list<ModelComponent*>::iterator it = _model->getComponentManager()->begin(); it != _model->getComponentManager()->end(); it++) {
		res &= (*it)->Check((*it));
	    }
	}
	Util::DecIndent();

	// check elements
	_model->getTraceManager()->trace(Util::TraceLevel::blockInternal, "Elements:");
	Util::IncIndent();
	{
	    std::string elementType;
	    bool result;
	    ModelElement* element;
	    std::string* errorMessage = new std::string();
	    std::list<std::string>* elementTypes = _model->getElementManager()->getElementTypenames();
	    for (std::list<std::string>::iterator typeIt = elementTypes->begin(); typeIt != elementTypes->end(); typeIt++) {
		elementType = (*typeIt);
		List<ModelElement*>* elements = _model->getElementManager()->getElements(elementType);
		for (std::list<ModelElement*>::iterator it = elements->getList()->begin(); it != elements->getList()->end(); it++) {
		    element = (*it);
		    // copyed from modelCOmponent. It is not inside the ModelElement::Check because ModelElement has no access to Model to call Tracer
		    _model->getTraceManager()->trace(Util::TraceLevel::mostDetailed, "Checking " + element->getTypename() + ": " + element->getName()+ " (id "+std::to_string(element->getId())+")"); //std::to_string(component->_id));
		    Util::IncIndent();
		    {
			try {
			    result = element->Check((*it), errorMessage);
			    res &= result;
			    if (!result) {
				_model->getTraceManager()->trace(Util::TraceLevel::errors, "Error: Checking has failed with message '" + *errorMessage + "'");
			    }
			} catch (const std::exception& e) {
			    _model->getTraceManager()->traceError(e, "Error verifying component " + element->show());
			}
		    }
		    Util::DecIndent();

		    /*
		    _model->getTraceManager()->trace(Util::TraceLevel::mostDetailed, "Verifying symbols of " + elementType + " " + element->getName()); //std::to_string(component->_id));
		    result = element->Check((*it), errorMessage);
		    res &= result;
		    if (!result) {
			_model->getTraceManager()->trace(Util::TraceLevel::mostDetailed, "Verification of symbols of component \"" + element->getName() + "\" has failed with message " + *errorMessage);
		    }
		     */
		}
	    }
	}
	Util::DecIndent();
    }
    Util::DecIndent();

    return res;
}

bool ModelCheckerImpl2::checkActivationCode() {
    /* TODO +-: not implemented yet */
    _model->getTraceManager()->trace(Util::TraceLevel::blockArrival, "Checking activation code");
    Util::IncIndent();
    {

    }
    Util::DecIndent();
    return true;
}

bool ModelCheckerImpl2::checkLimits() {
    bool res = true;
    std::string text;
    unsigned int value;
    LicenceManager *licence = _model->getParentSimulator()->getLicenceManager();
    _model->getTraceManager()->trace(Util::TraceLevel::blockArrival, "Checking model limits");
    Util::IncIndent();
    {
	value = _model->getComponentManager()->getNumberOfComponents();
	res &= value <= licence->getModelComponentsLimit();
	_model->getTraceManager()->trace(Util::TraceLevel::blockInternal, "Model has "+std::to_string(value)+" components");
	if (!res) {
	    text = "Model has " + std::to_string(_model->getComponentManager()->getNumberOfComponents()) + " components, exceding the limit of " + std::to_string(licence->getModelComponentsLimit()) + " components imposed by the current activation code";
	    //_model->getTraceManager()->trace(Util::TraceLevel::errors, text);
	} else {
	    value = _model->getElementManager()->getNumberOfElements();
	    res &= value <= licence->getModelElementsLimit();
	    _model->getTraceManager()->trace(Util::TraceLevel::blockInternal, "Model has "+std::to_string(value)+" elements");
	    if (!res) {
		text = "Model has " + std::to_string(_model->getElementManager()->getNumberOfElements()) + " elements, exceding the limit of " + std::to_string(licence->getModelElementsLimit()) + " elements imposed by the current activation code";
		//_model->getTraceManager()->trace(Util::TraceLevel::errors, text);
	    }
	}
	if (!res) {
	    _model->getTraceManager()->trace(Util::TraceLevel::errors, "Error: Checking has failed with message '" + text + "'");
	}
    }
    Util::DecIndent();
//    return res;
    return true;
}