/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Hold.cpp
 * Author: gstvob
 * 
 * Created on 03 de Junho de 2019, 15:20
 */

#include "Hold.h"
#include "Model.h"
#include "Attribute.h"
#include "Resource.h"
#include "Signal.h"

Hold::Hold(Model* model): ModelComponent(model, Util::TypeOf<Hold>()) {
} 

Hold::Hold(const Hold& orig) : ModelComponent(orig) {
}

Hold::~Hold() {
}

std::string Hold::show() {
    std::string res = ModelComponent::show() +
	    ", Type=" + std::to_string(static_cast<int> (this->_type)) +
            ", QueueName" + (this->_queue->getName());
    
    if (this->_type == Type::ScanForCondition) {
        res += ", Condition=" + (this->_condition);
            
    } else if (this->_type == Type::WaitForSignal) {
        res += ", waitForValue=" + (this->_waitForValue) +
                    ", limit=" + (this->_limit);
    } else {
        // DO NOTHING
    }
    
    return res;
}

PluginInformation* Hold::GetPluginInformation() {
    PluginInformation* info = new PluginInformation(Util::TypeOf<Hold>(), &Hold::LoadInstance);
    info->insertDynamicLibFileDependence("queue.so");
    return info;
}

ModelComponent* Hold::LoadInstance(Model* model, std::map<std::string, std::string>* fields) {
    Hold* newComponent = new Hold(model);
    try {
	newComponent->_loadInstance(fields);
    } catch (const std::exception& e) {
    }
    return newComponent;
}


void Hold::setWaitForValueExpr(std::string _expr) {
    this->_waitForValue = _expr;
}

void Hold::setLimit(std:: string _limit) {
    this->_limit = _limit;
}

void Hold::setCondition(std::string _condition) {
    this->_condition = _condition;
}

void Hold::setType(Type _type) {
    this->_type = _type;
}

void Hold::setQueue(Queue* _queue) {
    this->_queue = _queue;
}

std::string Hold::getWaitForValueExpr() const {
    return this->_waitForValue;
}

std::string Hold::getLimit() const {
    return this->_limit;
}

std::string Hold::getCondition() const {
    return this->_condition;
}

Hold::Type Hold::getType() const {
    return this->_type;
}

Queue* Hold::getQueue() const {
    return _queue;
}

void Hold::handleSignalReceived(int sigVal, int sigLimit) {
    if (this->_type == Type::WaitForSignal) {
        int expectedSig = _model->parseExpression(_waitForValue);
        int limit = _model->parseExpression(_limit);
        Waiting* first = _queue->first();
        
        if (first != nullptr && expectedSig == sigVal) {
            int releaseLimit;
            if (sigLimit == 0)
                releaseLimit = limit;
            else if (limit == 0)
                releaseLimit = sigLimit;
            else
                releaseLimit = std::min(sigLimit, sigVal);
            
            // ReleaseLimit == 0 means releaseAll at this point
            if (releaseLimit == 0 || releaseLimit > _queue->size())
                releaseLimit = _queue->size();
                
            while (releaseLimit > 0) {
                first = _queue->first();
                this->_queue->removeElement(first);
                this->releaseEntity(first->getEntity(), first->getTimeStartedWaiting());
                releaseLimit--;
            }
        }
    }
}

/* Releases an entity from the Hold queue and sends it to the next component connected */
void Hold::releaseEntity(Entity* entity, double startedWaiting) {
    double waitingTime = (_model->getSimulation()->getSimulatedTime() - startedWaiting);
    entity->getEntityType()->getStatisticsCollector("Wait Time")->getStatistics()->getCollector()->addValue(waitingTime);
    entity->setAttributeValue("Entity.WaitTime", entity->getAttributeValue("Entity.WaitTime") + waitingTime);
    _model->sendEntityToComponent(entity, this->getNextComponents()->frontConnection(), 0.0);
}

void Hold::_execute(Entity* entity) {
    if (this->_type == Type::WaitForSignal || this->_type == Type::InfiniteHold) {
        Waiting* waiting = new Waiting(entity, this, _model->getSimulation()->getSimulatedTime());
        this->_queue->insertElement(waiting);
        _model->getTraceManager()->traceSimulation(Util::TraceLevel::blockInternal, _model->getSimulation()->getSimulatedTime(), entity, this, "Entity starts to wait for signal in queue \"" + _queue->getName());
    } else {
        // IMPLEMENT CORRECTLY
//            Waiting* waiting = new Waiting(entity, this, _model->getSimulation()->getSimulatedTime());
//            this->_queue->insertElement(waiting);
//            double condition = _model->parseExpression((_condition));
//                _model->getTraceManager()->traceSimulation(Util::TraceLevel::blockInternal, _model->getSimulation()->getSimulatedTime(), entity, this, _condition + "the condition evaluated to " + std::to_string(condition));
//            if (condition)
//                _model->sendEntityToComponent(entity, this->getNextComponents()->front(), 0.0);
    }
}

void Hold::_initBetweenReplications() {
    this->_queue->initBetweenReplications();
}

bool Hold::_loadInstance(std::map<std::string, std::string>* fields) {
    bool res = ModelComponent::_loadInstance(fields);
    if (res) {
        std::string queueName = ((*(fields->find("queueName"))).second);
	Queue* queue = dynamic_cast<Queue*> (_model->getElementManager()->getElement(Util::TypeOf<Queue>(), queueName));
        this->_queue = queue;
        
        this->_waitForValue = ((*(fields->find("waitForValue"))).second);
        this->_limit = ((*(fields->find("limit"))).second);
        this->_condition = ((*(fields->find("condition"))).second);
        this->_type = static_cast<Hold::Type> (std::stoi((*fields->find("type")).second));
    }
    
    return res;
}

std::map<std::string, std::string>* Hold::_saveInstance() {
    std::map<std::string, std::string>* fields = ModelComponent::_saveInstance(); //Util::TypeOf<Seize>());
    fields->emplace("queueName", (this->_queue->getName()));
    fields->emplace("waitForValue", (this->_waitForValue));
    fields->emplace("limit", (this->_limit));
    fields->emplace("condition", (this->_condition));
    fields->emplace("type", std::to_string(static_cast<int> (this->_type)));
    return fields;
}

bool Hold::_check(std::string* errorMessage) {
    bool resultAll = true;
    
    if (this->_type == Type::ScanForCondition) {
        resultAll &= _model->checkExpression(_condition, "Condition", errorMessage);
    } else if (this->_type == Type::WaitForSignal) {
        resultAll &= _model->checkExpression(_waitForValue, "Wait for Value", errorMessage);
        resultAll &= _model->checkExpression(_limit, "Limit", errorMessage);
    } else {
        // DO NOTHING
    }
    
    resultAll &= _model->getElementManager()->check(Util::TypeOf<Queue>(), _queue, "Queue", errorMessage);

    return resultAll;
}
