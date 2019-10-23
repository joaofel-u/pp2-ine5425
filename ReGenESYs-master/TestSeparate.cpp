/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TestSeparate.cpp
 * Author: joaofel-u
 * 
 * Created on 20 de Setembro de 2019, 15:34
 */

#include "TestSeparate.h"

// you have to included need libs

// GEneSyS Simulator
#include "Simulator.h"

// Model Components
#include "Create.h"
#include "Separate.h"
#include "Dispose.h"

// Model elements
#include "Entity.h"
#include "EntityType.h"
#include "Queue.h"
#include "Resource.h"
#include "Hold.h"


TestSeparate::TestSeparate() {
}

/**
 * This is the main function of the application. 
 * It instanciates the simulator, builds a simulation model and then simulate that model.
 */
int TestSeparate::main(int argc, char** argv) {
    Simulator* simulator = new Simulator();

    // insert "fake plugins" since plugins based on dynamic loaded library are not implemented yet
    this->insertFakePluginsByHand(simulator);

    // creates an empty model
    Model* model = new Model(simulator);

    // Handle traces and simulation events to output them
    TraceManager* tm = model->getTraceManager();
    this->setDefaultTraceHandlers(tm);

    // get easy access to classes used to insert components and elements into a model
    ComponentManager* components = model->getComponentManager();
    ElementManager* elements = model->getElementManager();
    
    /*
     * @brief Defines the replication params.
     */
    ModelInfo* infos = model->getInfos();
    infos->setAnalystName("Joao Fellipe Uller");
    infos->setNumberOfReplications(1);
    infos->setReplicationLength(120);
    infos->setReplicationLengthTimeUnit(Util::TimeUnit::minute);
    infos->setWarmUpPeriod(0);
    infos->setWarmUpPeriodTimeUnit(Util::TimeUnit::minute);
    
    
    //
    // build the simulation model
    //
    simulator->getTraceManager()->setTraceLevel(Util::TraceLevel::blockArrival);

    // create a (Source)ModelElement of type EntityType, used by a ModelComponent that follows
    EntityType* entity1 = new EntityType(elements, "entity1");
    elements->insert(Util::TypeOf<EntityType>(), entity1); // insert the element into the model

    // create a ModelComponent of type Create, used to insert entities into the model
    Create* create1 = new Create(model);
    create1->setName("Create1");
    create1->setEntityType(entity1);
    create1->setTimeBetweenCreationsExpression("10");
    create1->setTimeUnit(Util::TimeUnit::minute);
    create1->setFirstCreation(10.0);
    components->insert(create1); // insert the component into the model
    
    Separate* separate1 = new Separate(model);
    separate1->setName("Separate1");
    separate1->setAmountToDuplicate("1");
    separate1->setPctCostToDuplicate("50");
    separate1->setSplitBatch(false);
    components->insert(separate1);
    
    Queue* queue1 = new Queue(elements, "Queue1");
    queue1->setOrderRule(Queue::OrderRule::FIFO);
    elements->insert(Util::TypeOf<Queue>(), queue1);
    
    Hold* hold1 = new Hold(model);
    hold1->setName("Hold1");
    hold1->setType(Hold::Type::InfiniteHold);
    hold1->setQueue(queue1);
    components->insert(hold1);
    
    // create a (Sink)ModelComponent of type Dispose, used to remove entities from the model
    Dispose* dispose1 = new Dispose(model);  // insert the component into the model
    dispose1->setName("Dispose1");
    components->insert(dispose1);
    
    /* Connect the components to compound the test model. */
    create1->getNextComponents()->insert(separate1);
    separate1->getNextComponents()->insert(hold1);
    separate1->getNextComponents()->insert(dispose1);
    /* Necessidade de ligar saida Hold em algum lugar. */
    
    
    // insert the model into the simulator 
    simulator->getModelManager()->insert(model);

    // if the model is ok then save the model into a text file 
    if (model->checkModel()) {
	model->saveModel("./temp/testSeparate.txt");
    }

    // execute the simulation util completed and show the report
    model->getSimulation()->startSimulation();
    return 0;
};