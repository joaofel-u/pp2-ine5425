#ifndef SIGNAL_H
#define SIGNAL_H

#include "ModelComponent.h"
#include "List.h"

class Signal: public ModelComponent {
public:
    Signal(Model *model);
    Signal(const Signal& orig);
    virtual ~Signal();
    virtual std::string show();
    static PluginInformation* GetPluginInformation();
    static ModelComponent* LoadInstance(Model* model, std::map<std::string, std::string>* fields);

    void setSignalValue(std::string expr);
    void setLimit(int limit);
    
    std::string getSignalValue();
    int getLimit();

private:
    int _limit = 10;
    std::string _signalValue;
    List<ModelComponent*>* _signalListeners;
    
protected:
    virtual void _execute(Entity* entity);
    virtual void _initBetweenReplications();

};

#endif