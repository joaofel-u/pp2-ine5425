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
#include <iostream>

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

    switch (this->_type) {
        case Type::ScanForCondition:
            res += ", Condition=" + (this->_condition);
            break;

        case Type::WaitForSignal:
            res += ", waitForValue=" + (this->_waitForValue) +
                    ", limit=" + std::to_string(this->_limit);
            break;

        case Type::InfiniteHold:
            break;

        default:  // UNREACHABLE
            break;
    }

    return res;
}

PluginInformation* Hold::GetPluginInformation() {
    return new PluginInformation(Util::TypeOf<Hold>(), &Hold::LoadInstance);
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

void Hold::setLimit(int _limit) {
    this->_limit = _limit;
}

void Hold::setCondition(std::string _condition) {
    this->_condition = _condition;
}

void Hold::setType(Type _type) {
    this->_type = _type;
}

void Hold::setQueueName(std::string _name) throw() {
    Queue* queue = dynamic_cast<Queue*>(_model->getElementManager()->getElement(Util::TypeOf<Queue>(), _name));
    if (queue != nullptr)
        this->_queue = queue;
    else
        throw std::invalid_argument("Queue does not exist");
}

void Hold::setQueue(Queue* queue) {
    this->_queue = queue;
}

Hold::Type Hold::getType() const {
    return this->_type;
}

std::string Hold::getWaitForValueExpr() const {
    return this->_waitForValue;
}

int Hold::getLimit() const {
    return this->_limit;
}

std::string Hold::getCondition() const {
    return this->_condition;
}

std::string Hold::getQueueName() const {
    return this->_queue->getName();
}

Queue* Hold::getQueue() const {
    return this->_queue;
}

void Hold::_execute(Entity* entity) {
    if (this->_type == Type::ScanForCondition)
    {
        /* Evaluate corretude. */
        Waiting* waiting = new Waiting(entity, this, _model->getSimulation()->getSimulatedTime());
        this->_queue->insertElement(waiting);
        double condition = _model->parseExpression((_waitForValue));
	    _model->getTraceManager()->traceSimulation(Util::TraceLevel::blockInternal, _model->getSimulation()->getSimulatedTime(), entity, this, _waitForValue + "the condition evaluated to " + std::to_string(condition));
        if (condition) {
            _model->sendEntityToComponent(entity, this->getNextComponents()->front(), 0.0);
            return;
        }
    }
    else /* WaitForSignal and InfiniteHold. */
    {
        Waiting* waiting = new Waiting(entity, this, _model->getSimulation()->getSimulatedTime());
        this->_queue->insertElement(waiting);
        return; 
    }
}

/* @todo See if the condition check is only in the first queue element. */
void Hold::release_signal(int signal, int recvLimit) {
    int limit;            /* Max number of entities that can be released. */
    double waitingSignal; /* Holds the waiting signal value of an entity. */
    
    if (this->_limit == 0)
        limit = recvLimit;
    else if (recvLimit == 0)
        limit = this->_limit;
    else
        limit = std::min(this->_limit, recvLimit);

    /* Releases the entities from the queue. */
    for (int i = 0; i < this->_queue->size(); ++i)
    {
        Waiting* waiting = _queue->getAtRank(i);
        //waitingSignal = _model->parseExpression((_waitForValue));
        
        waitingSignal = waiting->getEntity()->getAttributeValue("SigValue");
        //std::cout << waitingSignal << std::endl;
        
        if (waitingSignal == signal)
        {
            Waiting* waiting = _queue->getAtRank(i);
            _model->sendEntityToComponent(waiting->getEntity(), this->getNextComponents()->front(), 0.0);
            this->_limit--;
            this->_queue->removeElement(waiting);
            i--;
        }
        else
        {
            continue;
        }
        
        if (limit == 0)
            break;
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
        this->_limit = std::stoi((*(fields->find("limit"))).second);
        this->_condition = ((*(fields->find("condition"))).second);
        this->_type = static_cast<Hold::Type> (std::stoi((*fields->find("type")).second));
    }

    return res;
}

std::map<std::string, std::string>* Hold::_saveInstance() {
    std::map<std::string, std::string>* fields = ModelComponent::_saveInstance(); //Util::TypeOf<Seize>());
    fields->emplace("queueName", (this->_queue->getName()));
    fields->emplace("waitForValue", (this->_waitForValue));
    fields->emplace("limit", std::to_string(this->_limit));
    fields->emplace("condition", (this->_condition));
    fields->emplace("type", std::to_string(static_cast<int> (this->_type)));
    return fields;
}

bool Hold::_check(std::string* errorMessage) {
    bool resultAll = true;
    switch (this->_type) {
        case Type::ScanForCondition:
            resultAll &= _model->checkExpression(this->_condition, "Condition", errorMessage);
            break;

        case Type::WaitForSignal:
            resultAll &= this->_limit >= 0;
            resultAll &= _model->checkExpression(this->_waitForValue, "Wait for Value", errorMessage);
            break;

        case Type::InfiniteHold:
            break;

        default:  // UNREACHABLE
            return false;
    }

    resultAll &= _model->getElementManager()->check(Util::TypeOf<Queue>(), this->_queue, "Queue", errorMessage);

    return resultAll;
}