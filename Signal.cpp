/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Signal.cpp
 * Author: rlcancian
 * 
 * Created on 03 de Junho de 2019, 15:20
 */

#include "Signal.h"
#include "Model.h"
#include "Hold.h"

Signal::Signal(Model* model) : ModelComponent(model, Util::TypeOf<Signal>()) {
}

Signal::Signal(const Signal& orig) : ModelComponent(orig) {
}

Signal::~Signal() {
}

std::string Signal::show() {
    return ModelComponent::show() + 
	    ",signalValue=" + this->_signalValue +
	    ",limit=" + this->_limit;
}

PluginInformation* Signal::GetPluginInformation() {
    PluginInformation* info = new PluginInformation(Util::TypeOf<Signal>(), &Signal::LoadInstance);
    return info;
}

ModelComponent* Signal::LoadInstance(Model* model, std::map<std::string, std::string>* fields) {
    Signal* newComponent = new Signal(model);
    try {
	newComponent->_loadInstance(fields);
    } catch (const std::exception& e) {
        
    }
    return newComponent;
}

std::string Signal::getSignalValue() const {
    return _signalValue;
}

std::string Signal::getLimitExpr() const {
    return _limit;
}

void Signal::setSignalValue(std::string _signalValue) {
    this->_signalValue = _signalValue;
}

void Signal::setLimitExpr(std::string _limit) {
    this->_limit = _limit;
}

void Signal::_execute(Entity* entity) {
    int limit = _model->parseExpression(_limit);
    int sigVal = _model->parseExpression(_signalValue);
    
    List<ModelElement*> *elements = _model->getElementManager()->getElements(Util::TypeOf<Hold>());
    ModelElement* element = elements->front();

    for (int i = 0; i < elements->size(); ++i) {
        Hold* hold = dynamic_cast<Hold*>(element);

        if (hold->getType() == Hold::Type::WaitForSignal) {
            hold->handleSignalReceived(sigVal, limit);
        }
        
        element = elements->next();
    }
    
    _model->sendEntityToComponent(entity, this->getNextComponents()->frontConnection(), 0.0);
}

bool Signal::_loadInstance(std::map<std::string, std::string>* fields) {
    bool res = ModelComponent::_loadInstance(fields);
    if (res) {
        this->_signalValue = ((*(fields->find("signalValue"))).second);
        this->_limit = ((*(fields->find("limit"))).second);
    }
    
    return res;
}

void Signal::_initBetweenReplications() {
}

std::map<std::string, std::string>* Signal::_saveInstance() {
    std::map<std::string, std::string>* fields = ModelComponent::_saveInstance(); //Util::TypeOf<Seize>());
    fields->emplace("limit", (this->_limit));
    fields->emplace("signalValue", (this->_signalValue));
    return fields;
}

bool Signal::_check(std::string* errorMessage) {
    bool resultAll = true;
    resultAll &= _model->checkExpression(_signalValue, "Signal value", errorMessage);
    resultAll &= _model->checkExpression(_limit, "Limit", errorMessage);

    return resultAll;
}

