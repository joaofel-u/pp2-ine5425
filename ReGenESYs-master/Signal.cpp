#include "Signal.h"
#include "Entity.h"
#include "Hold.h"
#include "Model.h"
#include "Util.h"

//#include <iostream>

Signal::Signal(Model *model): ModelComponent(model, Util::TypeOf<Signal>()) {
    this->_signalListeners = new List<Hold*>();
}

Signal::Signal(const Signal& orig): ModelComponent(orig) {

}


Signal::~Signal() {
}
template<typename Base, typename T>
inline bool instanceof(const T*) {
   return std::is_base_of<Base, T>::value;
}

void Signal::_execute(Entity* entity) {
//    double signalValue = _model->parseExpression((this->_signalValue));
//    std::cout << signalValue << std::endl;
    
    int signalValue = entity->getAttributeValue("SigValue");
    //std::cout << value << std::endl;
    
    for(auto it = this->_signalListeners->front(); it != this->_signalListeners->last(); it = this->_signalListeners->next()) {
        //Hold* h = dynamic_cast<Hold*>(it);
        it->release_signal(signalValue, this->_limit);
    }
    
    _model->sendEntityToComponent(entity, this->getNextComponents()->front(), 0.0);
}
std::string Signal::show() {
    return ModelComponent::show() + "";
}
PluginInformation* Signal::GetPluginInformation() {
    return new PluginInformation(Util::TypeOf<Signal>(), &Hold::LoadInstance);
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