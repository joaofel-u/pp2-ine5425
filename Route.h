/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Route.h
 * Author: joaofel-u
 *
 * Created on 05 de Setembro de 2019, 22:12
 */

#ifndef ROUTE_H
#define ROUTE_H

#include "ModelComponent.h"
#include "Plugin.h"

class Route : public ModelComponent {

public:
    enum class DestinationType : int {
	Station=0, BySequence=1
    };
    
public:
    Route(Model* model);
    Route(const Route& orig);
    virtual ~Route();
    
    virtual std::string show();
    
    static PluginInformation* GetPluginInformation();
    static ModelComponent* LoadInstance(Model* model, std::map<std::string, std::string>* fields);

public:
    // Getters & Setters fir the class attributes
    void setRouteTimeUnit(Util::TimeUnit _routeTimeUnit);
    Util::TimeUnit getRouteTimeUnit() const;
    void setRouteTimeExpression(std::string _routeTimeExpression);
    std::string getRouteTimeExpression() const;
    void setRouteDestinationType(DestinationType _routeDestinationType);
    DestinationType getRouteDestinationType() const;
    
protected:
    virtual void _execute(Entity* entity);
    virtual bool _loadInstance(std::map<std::string, std::string>* fields);
    virtual void _initBetweenReplications();
    virtual std::map<std::string, std::string>* _saveInstance();
    virtual bool _check(std::string* errorMessage);
    
private:
    // Route attributes
    std::string _routeTimeExpression = "0.0";
    Util::TimeUnit _routeTimeUnit = Util::TimeUnit::second;
    DestinationType _routeDestinationType = DestinationType::Station;
};

#endif /* ROUTE_H */

