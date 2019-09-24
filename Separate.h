#ifndef SEPARATE_H
#define SEPARATE_H

#include <string>
#include <list>
#include <assert.h>

#include "ModelComponent.h"
#include "Model.h"
#include "Group.h"

class Separate : public ModelComponent {
public:
    Separate(Model* model);
    Separate(const Separate& orig);
    virtual ~Separate();
public:
    virtual std::string show();
public:
    static PluginInformation* GetPluginInformation();
    static ModelComponent* LoadInstance(Model* model, std::map<std::string, std::string>* fields);

    void setSeparateName(std::string value);
    void setSplitBatch(bool value);
    void setAmountToDuplicate(unsigned int value);
    void setTakeAllRepresentativeValues(bool value);
protected:
    virtual void _execute(Entity* entity);
    virtual void _initBetweenReplications();
    virtual bool _loadInstance(std::map<std::string, std::string>* fields);
    virtual std::map<std::string, std::string>* _saveInstance();
    virtual bool _check(std::string* errorMessage);
private:
    unsigned int _amountToDup;
    std::string _separateName;
    bool _splitBatch;
    bool _takeAllRepresentativeValues;
};

#endif /* SEPARATE_H */

