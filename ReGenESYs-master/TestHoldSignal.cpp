/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TestHoldSignal.cpp
 * Author: joaofel-u
 *
 * Created on 23 de outubro de 2019, 08:33
 */

#include "TestHoldSignal.h"

// GEneSyS Simulator
#include "Simulator.h"

// Model Components
#include "Create.h"
#include "Remove.h"
#include "Hold.h"
#include "Dispose.h"
#include "Signal.h"

// Model elements
#include "Entity.h"
#include "EntityType.h"
#include "Queue.h"
#include "Resource.h"


TestHoldSignal::TestHoldSignal() {}
TestHoldSignal::TestHoldSignal(const TestHoldSignal& orig) {}
TestHoldSignal::~TestHoldSignal() {}

/**
 * This is the main function of the application. 
 * It instanciates the simulator, builds a simulation model and then simulate that model.
 */
int TestHoldSignal::main(int argc, char** argv) {
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
  
    
    /* Components. */
    Create* create1 = new Create(model);
    create1->setEntityType(carro);
    create1->setTimeBetweenCreationsExpression("10");
    create1->setTimeUnit(Util::TimeUnit::minute);
    create1->setFirstCreation(0.0);
    components->insert(create1);

    
    /* Connections. */
    
    
    // insert the model into the simulator 
    simulator->getModelManager()->insert(model);

    // if the model is ok then save the model into a text file 
    if (model->checkModel()) {
	model->saveModel("./temp/testHoldSignal.txt");
    }

    // execute the simulation util completed and show the report
    model->getSimulation()->startSimulation();
    return 0;
};