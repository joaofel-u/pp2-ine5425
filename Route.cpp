/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Route.cpp
 * Author: joaofel-u
 * 
 * Created on 05 de Setembro de 2019, 22:20
 */

#include "Route.h"
#include "Model.h"
#include "Attribute.h"

Route::Route(Model* model) : ModelComponent(model, Util::TypeOf<Route>()) {
}

Route::Route(const Route& orig) : ModelComponent(orig) {
}

Route::~Route() {
}

std::string Route::show() {
    return ModelComponent::show() +
	    "";
}

PluginInformation* Route::GetPluginInformation() {
    return new PluginInformation(Util::TypeOf<Route>(), &Route::LoadInstance);
}

ModelComponent* Route::LoadInstance(Model* model, std::map<std::string, std::string>* fields) {
    Route* newComponent = new Route(model);
    try {
	newComponent->_loadInstance(fields);
    } catch (const std::exception& e) {

    }
    return newComponent;
}


void Route::setRouteTimeUnit(Util::TimeUnit _routeTimeUnit) {
    this->_routeTimeUnit = _routeTimeUnit;
}

Util::TimeUnit Route::getRouteTimeUnit() const {
    return this->_routeTimeUnit;
}

void Route::setRouteTimeExpression(std::string _routeTimeExpression) {
    this->_routeTimeExpression = _routeTimeExpression;
}

std::string Route::getRouteTimeExpression() const {
    return this->_routeTimeExpression;
}

void Route::setRouteDestinationType(DestinationType _routeDestinationType) {
    this->_routeDestinationType = _routeDestinationType;
}

Route::DestinationType Route::getRouteDestinationType() const {
    return this->_routeDestinationType;
}
    
    
void Route::_execute(Entity* entity) {
    // IMPLEMENTATION MADE ONLY FOR STATION DESTINATION TYPE
    // adds the route time to the TransferTime statistics / attribute related to the Entitys 
    double routeTime = _model->parseExpression(_routeTimeExpression) * Util::TimeUnitConvert(_routeTimeUnit, _model->getInfos()->getReplicationLengthTimeUnit());
    entity->getEntityType()->getCstatTransferTime()->getStatistics()->getCollector()->addValue(routeTime);
    entity->setAttributeValue("Entity.TransferTime", entity->getAttributeValue("Entity.TransferTime") + routeTime);
    
    // sets the Entity.Station attribute to the next Station
    entity->setAttributeValue("Entity.Station", this->getNextComponents()->front()->getId());
    
    // calculates when this Entity will reach the end of this route and schedule this Event
    double routeEndTime = _model->getSimulation()->getSimulatedTime() + routeTime;
    Event* newEvent = new Event(routeEndTime, entity, this->getNextComponents()->front());
    _model->getEvents()->insert(newEvent);
    _model->getTraceManager()->trace(Util::TraceLevel::blockInternal, "End of route of entity " + std::to_string(entity->getId()) + " scheduled to time " + std::to_string(routeEndTime));
}

bool Route::_loadInstance(std::map<std::string, std::string>* fields) {
    bool res = ModelComponent::_loadInstance(fields);
    if (res) {
        this->_routeTimeExpression = (*fields->find("routeTimeExpression")).second;
        this->_routeTimeUnit = static_cast<Util::TimeUnit> (std::stoi((*fields->find("routeTimeUnit")).second));
        this->_routeDestinationType = static_cast<Route::DestinationType> (std::stoi((*fields->find("routeDestinationType")).second));
    }
    return res;
}

void Route::_initBetweenReplications() {}

std::map<std::string, std::string>* Route::_saveInstance() {
    std::map<std::string, std::string>* fields = ModelComponent::_saveInstance(); //Util::TypeOf<Route>());    
    fields->emplace("routeTimeExpression", this->_routeTimeExpression);
    fields->emplace("routeTimeUnit", std::to_string(static_cast<int> (this->_routeTimeUnit)));
    fields->emplace("routeDestinationType", std::to_string(static_cast<int> (this->_routeDestinationType)));
    return fields;
}

// IMPLEMENT
bool Route::_check(std::string* errorMessage) {
    //include attributes needed
    ElementManager* elements = _model->getElementManager();
    std::vector<std::string> neededNames = {"Entity.TransferTime", "Entity.Station"};
    std::string neededName;
    for (unsigned int i = 0; i < neededNames.size(); i++) {
	neededName = neededNames[i];
	if (elements->getElement(Util::TypeOf<Attribute>(), neededName) == nullptr) {
	    Attribute* attr1 = new Attribute(neededName);
	    elements->insert(Util::TypeOf<Attribute>(), attr1);
	}
    }
    
    return _model->checkExpression(_routeTimeExpression, "Route time expression", errorMessage);
}

