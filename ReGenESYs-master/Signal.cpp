#include "Signal.h"
#include "Entity.h"
#include "Hold.h"
#include "Model.h"
#include "Util.h"

Signal::Signal(Model *model): ModelComponent(model, Util::TypeOf<Signal>()) {

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
    for(auto it = this->_signalListeners->front(); it != this->_signalListeners->last(); it = this->_signalListeners->next()) {
        Hold* h = dynamic_cast<Hold*>(it);
        int signalValue = _model->parseExpression((this->_signalValue));
        h->release_signal(signalValue, this->_limit);
    }
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

ModelComponent* Signal::LoadInstance(Model* model, std::map<std::string, std::string>* fields) {
    Signal* newComponent = new Signal(model);
    try {
    newComponent->_loadInstance(fields);
    } catch (const std::exception& e) {
    }
    return newComponent;
}