/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Separate.h
 * Author: rlcancian
 *
 * Created on 03 de Junho de 2019, 15:14
 */

#ifndef SEPARATE_H
#define SEPARATE_H

#include "ModelComponent.h"

/*!
 This component ...
 */
class Separate : public ModelComponent {
public: // constructors
    Separate(Model* model);
    Separate(const Separate& orig);
    virtual ~Separate();
public:  // virtual
    virtual std::string show();
public:  // static
    static PluginInformation* GetPluginInformation();
    static ModelComponent* LoadInstance(Model* model, std::map<std::string, std::string>* fields);
protected:  // virtual
    virtual void _execute(Entity* entity);
    virtual void _initBetweenReplications();
    virtual bool _loadInstance(std::map<std::string, std::string>* fields);
    virtual std::map<std::string, std::string>* _saveInstance();
    virtual bool _check(std::string* errorMessage);
public: // methods
    void setSplitBatch(bool value);
    void setAmountToDuplicate(std::string value);
    void setPctCostToDuplicate(std::string value);
    void setTakeAllRepresentativeValues(bool value);
    void setAttributeType(bool value);
    
private: // attributes 1:1
private: // attributes 1:n
    std::string _amountToDup = "1";
    std::string _pctCostToDup = "50";
    bool _splitBatch = false; /* Separate::Type == SplitExistingBatch? */
    bool _takeAllRepresentativeValues = false;
    bool _attributeType = false;
};

#endif /* SEPARATE_H */
