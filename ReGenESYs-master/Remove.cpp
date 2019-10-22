/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Remove.cpp
 * Author: Group 5
 */

/*
* The Remove module removes a single entity from a specified position in a queue
* and sends it to a designated module.
*/

#include "Remove.h"
#include "Model.h"
#include "Queue.h"
#include <string>

Remove::Remove(Model* model) : ModelComponent(model, Util::TypeOf<Remove>()) {
}

Remove::Remove(const Remove& orig) : ModelComponent(orig) {
}

Remove::~Remove() {
}

std::string Remove::show() {
    return ModelComponent::show() +
    ",queueName=" + this->_queueName +
    ",rank=" + this->_rank;
}

ModelComponent* Remove::LoadInstance(Model* model, std::map<std::string, std::string>* fields) {
    Remove* newComponent = new Remove(model);
    try {
        newComponent->_loadInstance(fields);
    } catch (const std::exception& e) {
    }
    return newComponent;
}

void Remove::setQueueName(std::string _queueName) {
    this->_queueName = _queueName;
}

std::string Remove::getQueueName() const {
    return _queueName;
}

void Remove::setRank(std::string rank) {
    this->_rank = rank;
}

std::string Remove::getRank() const {
    return _rank;
}

/*
* When an entity arrives at a Remove module, it removes the entity from the 
* specified queue and sends it to the connected module.
*
* The rank of the entity signifies the location of the entity within the queue
*
* The entity that caused the removal proceeds to the next module specified and 
* is processed before the removed entity
*/
void Remove::_execute(Entity* entity) {
    std::string queueName = this->_queueName;
    int rankToRemove = (_model->parseExpression(this->_rank)) - 1;  /* Correction factor to underlying list starting at 0. */

    /* Gets the specified queue from its name. */
    Queue* queue = dynamic_cast<Queue*> (_model->getElementManager()->getElement(Util::TypeOf<Queue>(), queueName));
    if (queue == nullptr)
        throw std::invalid_argument("Queue does not exist");
	
    /* Get the element (entity) within the queue. */
    Waiting* element = queue->getAtRank(rankToRemove);

    if(element != nullptr) {
        queue->removeElement(element);
        /* Removed element goes separated. */
        this->_model->sendEntityToComponent(element->getEntity(), this->getNextComponents()->getAtRank(1), 0.0);
    }

    /* The original entity goes to the "normal" path. */
    this->_model->sendEntityToComponent(entity, this->getNextComponents()->getAtRank(0), 0.0);
}

bool Remove::_loadInstance(std::map<std::string, std::string>* fields) {
    bool res = ModelComponent::_loadInstance(fields);
    if (res) {
        this->_queueName = ((*(fields->find("queueName"))).second);
        this->_rank = ((*(fields->find("rank"))).second);
    }
    return res;
}

void Remove::_initBetweenReplications() {
}

std::map<std::string, std::string>* Remove::_saveInstance() {
    std::map<std::string, std::string>* fields = ModelComponent::_saveInstance(); 
    fields->emplace("queueName", this->_queueName);
    fields->emplace("rank", this->_rank);
    return fields;
}

bool Remove::_check(std::string* errorMessage) {
    bool resultAll =  true;
    resultAll &= _model->checkExpression(_rank, "rank", errorMessage);
    return resultAll;

}

PluginInformation* Remove::GetPluginInformation(){
    return new PluginInformation(Util::TypeOf<Remove>(), &Remove::LoadInstance);
}
