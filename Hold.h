/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Hold.h
 * Author: gstvob
 *
 * Created on 03 de Junho de 2019, 15:20
 */

#ifndef HOLD_H
#define HOLD_H

#include "ModelComponent.h"
#include "Plugin.h"
#include "Queue.h"

class Hold : public ModelComponent {

public:
    
    enum class Type: int {
        WaitForSignal=0,
        ScanForCondition=1,
        InfiniteHold=2
    };
    
    Hold(Model* model);
    Hold(const Hold& orig);
    virtual ~Hold();

    virtual std::string show();
    static PluginInformation* GetPluginInformation();
    static ModelComponent* LoadInstance(Model* model, std::map<std::string, std::string>* fields);

    void setWaitForValueExpr(std::string _expr);
    void setLimit(std:: string _limit);
    void setCondition(std::string _condition);
    void setType(Type _type);
    void setQueue(Queue* _queue);

    std::string getWaitForValueExpr() const;
    std::string getLimit() const;
    std::string getCondition() const;
    Type getType() const;
    Queue* getQueue() const;
    
    void handleSignalReceived(int sigVal, int sigLimit);

protected:
    virtual void _execute(Entity* entity);
    virtual bool _loadInstance(std::map<std::string, std::string>* fields);
    virtual void _initBetweenReplications();
    virtual std::map<std::string, std::string>* _saveInstance();
    virtual bool _check(std::string* errorMessage);

private:
    void releaseEntity(Entity* entity, double startedWaiting);
    
private:
    std::string _waitForValue = "1.0";
    std::string _limit = "0.0";
    std::string _condition = "";
    Type _type = Type::WaitForSignal;
    Queue* _queue;
};

#endif /* HOLD_H */

