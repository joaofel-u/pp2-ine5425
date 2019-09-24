/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Remove.h
 * Author: Group 5
 */

#ifndef REMOVE_H
#define REMOVE_H

#include <string>
#include "ModelComponent.h"
#include "Plugin.h"

class Remove : public ModelComponent {
public:
    Remove(Model* model);
    Remove(const Remove& orig);
    virtual ~Remove();
public:
    void setQueueName(std::string _queueName);
    std::string getQueueName() const;
    void setRank(std::string _rank);
    std::string getRank() const;
public:
    virtual std::string show();
public:
    static PluginInformation* GetPluginInformation();
    static ModelComponent* LoadInstance(Model* model, std::map<std::string, std::string>* fields);
protected:
    virtual void _execute(Entity* entity);
    virtual void _initBetweenReplications();
    virtual bool _loadInstance(std::map<std::string, std::string>* fields);
    virtual std::map<std::string, std::string>* _saveInstance();
    virtual bool _check(std::string* errorMessage);
private:
    /* Queue Name: Name of the queue from which the entity will be removed. */
    std::string _queueName = "Queue";

    /* Rank of Entity: Rank of the entity to remove from within the queue. */
    std::string _rank = "1";
};

#endif /* REMOVE_H */

