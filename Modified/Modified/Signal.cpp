#include "Signal.h"
#include "Entity.h"
#include "Hold.h"
#include "Model.h"
#include "Util.h"


Signal::Signal(Model *model): ModelComponent(model, Util::TypeOf<Signal>()) {
    this->_signalListeners = new List<Hold*>();
}

Signal::Signal(const Signal& orig): ModelComponent(orig) {

}


Signal::~Signal() {
    delete this->_signalListeners;
}

void Signal::_execute(Entity* entity) {
    double signalValue = _model->parseExpression((this->_signalValue));
    
    for (unsigned int i = 0; i < this->_signalListeners->size(); ++i) {
        Hold* h = this->_signalListeners->getAtRank(i);
        h->release_signal(signalValue, this->_limit);
    }
    
    _model->sendEntityToComponent(entity, this->getNextComponents()->front(), 0.0);
}
std::string Signal::show() {
    return ModelComponent::show() + "";
}
PluginInformation* Signal::GetPluginInformation() {
    return new PluginInformation(Util::TypeOf<Signal>(), &Signal::LoadInstance);
}

void Signal::_initBetweenReplications() {
    
}

void Signal::setSignalValue(std::string expr) {
    this->_signalValue = expr;
}

std::string Signal::getSignalValue() {
    return this->_signalValue;
}

void Signal::setLimit(int limit) {
    this->_limit = limit;
}

int Signal::getLimit() {
    return this->_limit;
}

List<Hold*>* Signal::getListeners() {
    return this->_signalListeners;
}

ModelComponent* Signal::LoadInstance(Model* model, std::map<std::string, std::string>* fields) {
    Signal* newComponent = new Signal(model);
    try {
    newComponent->_loadInstance(fields);
    } catch (const std::exception& e) {
    }
    return newComponent;
}