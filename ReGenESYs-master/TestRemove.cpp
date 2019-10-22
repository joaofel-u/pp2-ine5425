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

// you have to included need libs

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
    infos->setNumberOfReplications(10);
    infos->setReplicationLength(2);
    infos->setReplicationLengthTimeUnit(Util::TimeUnit::hour);
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
    createCarro->setEntityType(carro);
    createCarro->setTimeBetweenCreationsExpression("10");
    createCarro->setTimeUnit(Util::TimeUnit::minute);
    createCarro->setFirstCreation(0.0);
    components->insert(createCarro);

    Queue* queueHold = new Queue(elements, "queue1");
    queueHold->setOrderRule(Queue::OrderRule::FIFO);
    elements->insert(Util::TypeOf<Queue>(), queueHold);
    
    Hold* hold1 = new Hold(model);
    hold1->setType(Hold::Type::InfiniteHold);
    hold1->setQueue(queueHold);
    
    Create* createComprador = new Create(model);
    createComprador->setEntityType(comprador);
    createComprador->setTimeBetweenCreationsExpression("20");
    createComprador->setTimeUnit(Util::TimeUnit::minute);
    createComprador->setFirstCreation(5.0);
    components->insert(createComprador);
    
    Dispose* disposeComprador = new Dispose(model);
    components->insert(disposeComprador);
    
    Dispose* disposeCarro = new Dispose(model);
    components->insert(disposeCarro);
    
    Remove* remove1 = new Remove(model);
    remove1->setQueueName("queue1");
    remove1->setRank("1");
    components->insert(remove1);
    
    
    /* Connection 1. */
    createCarro->getNextComponents()->insert(hold1);
    /* Necessidade de conectar saida hold em algum lugar. */
    
    /* Connection 2. */
    createComprador->getNextComponents()->insert(remove1);
    remove1->getNextComponents()->insert(disposeComprador);
    remove1->getNextComponents()->insert(disposeCarro);
    
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