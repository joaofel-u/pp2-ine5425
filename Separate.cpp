#include "Separate.h"

/*!
 * Constructor
 *
 * @param model is the object of the model executing.
 */
Separate::Separate(Model* model) : ModelComponent(model, Util::TypeOf<Separate>()) {
    this->_splitBatch = true;
    this->_amountToDup = 1;
    this->_takeAllRepresentativeValues = false;
}

Separate::Separate(const Separate& orig) : ModelComponent(orig) {}

Separate::~Separate() {}

/*!
 * Display information about the Separate component
 */
std::string Separate::show() {
    return ModelComponent::show() + ",separateName=\"" + this->_separateName + "\"";
}

ModelComponent* Separate::LoadInstance(Model* model, std::map<std::string, std::string>* fields) {
    Separate* newComponent = new Separate(model);

    try {
        newComponent->_loadInstance(fields);
    } catch (const std::exception& e) {
    }

    return newComponent;
}

/*!
 *  Set the name of the separate to be created when spliting entities.
 */
void Separate::setSeparateName(std::string value) {
    this->_separateName = value;
}

/*!
 *    Set the type of the separate, whether it must split existing batch or
 *  duplicate the incoming entity
 */
void Separate::setSplitBatch(bool value) {
    this->_splitBatch = value;
}

/*!
 *  Set the number of duplicates to an entity to be generated, when using
 * duplication
 */
void Separate::setAmountToDuplicate(unsigned int value) {
    this->_amountToDup = value;
}

/*!
 *  Set how to assign the representative entity attribute values to the
 *  original entities
 */
void Separate::setTakeAllRepresentativeValues(bool value) {
    this->_takeAllRepresentativeValues = value;
}

/*!
 *  Execute the default behavior of the Separate component, which receives an
 * entity with a Batch of entities. It must split the original batch or
 * duplicate it, based on it's parameters. Can receive either a Batch entity
 * or any other entity
 */
void Separate::_execute(Entity* entity) {
    double currentTime = _model->getSimulation()->getSimulatedTime();
    unsigned int batchesCount = (unsigned int) entity->getAttributeValue("Entity.GroupRankID");
    double copyId = entity->getAttributeValue("Entity.CopyNumber");
    std::string batchEntityName = entity->getEntityTypeName();
    std::string batchesCountStr = std::to_string(batchesCount);
    std::string attributeValue = std::to_string(entity->getAttributeValue("Entity.AttributeValue"));
    std::string batchEntityID = batchEntityName + batchesCountStr + attributeValue + std::to_string(copyId);

    ElementManager* elementManager = this->_model->getElementManager();
    TraceManager* traceManager = this->_model->getTraceManager();

    traceManager->trace(Util::TraceLevel::blockInternal, "Arrival of entity " + std::to_string(entity->getId()) +
                                                         " at time " + std::to_string(currentTime));

    Group* group = (Group*) elementManager->getElement(Util::TypeOf<Group>(), batchEntityID);

    if (this->_splitBatch) {
        /* Split the existing Batch entity */
        Entity* batchEntity;
        if ((bool)entity->getAttributeValue("Entity.Permanent")) {
            this->_model->sendEntityToComponent(entity, this->getNextComponents()->front(), 0.0);
            return;
        }
        assert(group != nullptr);

        for (auto i = 0u; i < group->size(); i++) {
            batchEntity = group->first();
            group->removeElement(batchEntity);
            this->_model->sendEntityToComponent(batchEntity, this->getNextComponents()->front(), 0.0);
        }
    } else {
        /* Duplicate the existing entity */
        ModelComponent* originalComponent = this->getNextComponents()->front();
        ModelComponent* duplicateComponent = originalComponent->getNextComponents()->front();
        Entity* duplicateEntity;
        this->_model->sendEntityToComponent(entity, originalComponent, 0.0);

        for (auto k = 0u; k < this->_amountToDup; k++) {
            /* Duplicate input entity in N entities*/
            if (entity->getEntityTypeName().find("_Batch") != std::string::npos) {
                assert(group != nullptr);
                duplicateEntity = new Entity(elementManager);
                duplicateEntity->setEntityType(entity->getEntityType());

                Entity* newEntity;
                std::string dupBatchEntityID = batchEntityName + batchesCountStr + attributeValue + std::to_string(copyId + 1);

                Group* duplicateGroup = new Group(elementManager, dupBatchEntityID);
                elementManager->insert(Util::TypeOf<Group>(), duplicateGroup);
                
                // VER
                //std::list<Entity*>* entitiesToDup = group->getList();
                std::list<Entity*>* entitiesToDup;
                std::list<Entity*>::iterator it = entitiesToDup->begin();

                for (auto i = 0u; i < entitiesToDup->size(); i++) {
                    auto original = std::next(it, 1);
                    newEntity = new Entity(elementManager);
                    newEntity->setEntityType((*original)->getEntityType());
                    duplicateGroup->insertElement(newEntity);
                }
                copyId++;

                duplicateEntity->setAttributeValue("Entity.GroupRankID", (double)elementManager->getRankOf(Util::TypeOf<Group>(), dupBatchEntityID));
                duplicateEntity->setAttributeValue("Entity.Permanent", (bool)entity->getAttributeValue("Entity.Permanent"));
                duplicateEntity->setAttributeValue("Entity.CopyNumber", copyId);
                duplicateEntity->setAttributeValue("Entity.AttributeValue", entity->getAttributeValue("Entity.AttributeValue"));

                elementManager->insert(Util::TypeOf<Entity>(), duplicateEntity);

                this->_model->sendEntityToComponent(duplicateEntity, duplicateComponent, 0.0);
            } else {
                duplicateEntity = new Entity(elementManager);
                duplicateEntity->setEntityType(entity->getEntityType());
                this->_model->sendEntityToComponent(duplicateEntity, duplicateComponent, 0.0);
            }
        }
    }
}


/*!
 *  Load a previous instance of the Separate class stored.
 */
bool Separate::_loadInstance(std::map<std::string, std::string>* fields) {
    bool res = ModelComponent::_loadInstance(fields);

    if (res) {
        this->_separateName = (*(fields->find("separateName"))).second;
        this->_splitBatch = (*(fields->find("splitBatch"))).second == "true";
    }

    return res;
}

/*!
 *  Store current instance of Separate in a std::map, to be used as settings
 *  backup
 */
std::map<std::string, std::string>* Separate::_saveInstance() {
    std::map<std::string, std::string>* fields = ModelComponent::_saveInstance();

    fields->emplace("separateName", this->_separateName);
    fields->emplace("splitBatch", std::to_string(this->_splitBatch));

    return fields;
}

/*!
 *  Execute the clean up before each new replication.
 */
void Separate::_initBetweenReplications() {}

bool Separate::_check(std::string* errorMessage) {
    return ModelComponent::_check(errorMessage);
}

PluginInformation* Separate::GetPluginInformation(){
    return new PluginInformation(Util::TypeOf<Separate>(), &Separate::LoadInstance);
}
