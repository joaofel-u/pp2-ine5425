/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Signal.h
 * Author: rlcancian
 *
 * Created on 03 de Junho de 2019, 15:20
 */

#ifndef SIGNAL_H
#define SIGNAL_H

#include "ModelComponent.h"
#include "Plugin.h"

class Signal : public ModelComponent {
public:
    Signal(Model* model);
    Signal(const Signal& orig);
    virtual ~Signal();
    
    virtual std::string show();
    static PluginInformation* GetPluginInformation();
    static ModelComponent* LoadInstance(Model* model, std::map<std::string, std::string>* fields);
    
    std::string getSignalValue() const;
    std::string getLimitExpr() const;
    
    void setSignalValue(std::string _signalValue);
    void setLimitExpr(std::string _limit);
    
protected:
    virtual void _execute(Entity* entity);
    virtual bool _loadInstance(std::map<std::string, std::string>* fields);
    virtual void _initBetweenReplications();
    virtual std::map<std::string, std::string>* _saveInstance();
    virtual bool _check(std::string* errorMessage);
    
private:
    std::string _signalValue = "1.0";
    std::string _limit = "0.0";

};

#endif /* SIGNAL_H */

