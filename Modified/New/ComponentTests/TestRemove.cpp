/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TestRemove.cpp
 * Author: joaofel-u
 * 
 * Created on 20 de Setembro de 2019, 15:30
 */

#include "TestRemove.h"

// GEneSyS Simulator
#include "Simulator.h"

// Model Components
#include "Create.h"
#include "Remove.h"
#include "Hold.h"
#include "Dispose.h"

// Model elements
#include "Entity.h"
#include "EntityType.h"
#include "Queue.h"
#include "Resource.h"


TestRemove::TestRemove() {}
TestRemove::TestRemove(const TestRemove& orig) {}
TestRemove::~TestRemove() {}

/**
 * This is the main function of the application. 
 * It instanciates the simulator, builds a simulation model and then simulate that model.
 */
int TestRemove::main(int argc, char** argv) {
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
    EntityType* carro = new EntityType(elements, "carro");
    elements->insert(Util::TypeOf<EntityType>(), carro);
    
    EntityType* comprador = new EntityType(elements, "comprador");
    elements->insert(Util::TypeOf<EntityType>(), comprador);
    

    Create* createCarro = new Create(model);
    createCarro->setName("CreateCarro");
    createCarro->setEntityType(carro);
    createCarro->setTimeBetweenCreationsExpression("10");
    createCarro->setTimeUnit(Util::TimeUnit::minute);
    createCarro->setFirstCreation(0.0);
    components->insert(createCarro);

    Queue* queueHold = new Queue(elements, "QueueHold");
    queueHold->setOrderRule(Queue::OrderRule::FIFO);
    elements->insert(Util::TypeOf<Queue>(), queueHold);
    
    Hold* hold1 = new Hold(model);
    hold1->setName("HoldCarro");
    hold1->setType(Hold::Type::InfiniteHold);
    hold1->setQueue(queueHold);
    components->insert(hold1);
    
    Create* createComprador = new Create(model);
    createComprador->setName("CreateComprador");
    createComprador->setEntityType(comprador);
    createComprador->setTimeBetweenCreationsExpression("60");
    createComprador->setTimeUnit(Util::TimeUnit::minute);
    createComprador->setFirstCreation(30.0);
    components->insert(createComprador);
    
    Dispose* disposeComprador = new Dispose(model);
    disposeComprador->setName("DisposeComprador");
    components->insert(disposeComprador);
    
    Dispose* disposeCarro = new Dispose(model);
    disposeCarro->setName("DisposeCarro");
    components->insert(disposeCarro);
    
    Remove* removeCarro = new Remove(model);
    removeCarro->setName("RemoveCarro");
    removeCarro->setQueueName("QueueHold");
    removeCarro->setRank("NQ(QueueHold)");
    components->insert(removeCarro);
    
    
    /* Connection 1. */
    createCarro->getNextComponents()->insert(hold1);
    
    /* Connection 2. */
    createComprador->getNextComponents()->insert(removeCarro);
    removeCarro->getNextComponents()->insert(disposeComprador);
    removeCarro->getNextComponents()->insert(disposeCarro);
    
    // insert the model into the simulator 
    simulator->getModelManager()->insert(model);

    // if the model is ok then save the model into a text file 
    if (model->checkModel()) {
	model->saveModel("./temp/testRemove.txt");
    }

    // execute the simulation util completed and show the report
    model->getSimulation()->startSimulation();
    return 0;
};