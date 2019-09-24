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
#include "Dispose.h"

// Model elements
#include "Entity.h"
#include "EntityType.h"
#include "Queue.h"
#include "Resource.h"


TestRemove::TestRemove() {
}

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

    //
    // build the simulation model
    //

    simulator->getTraceManager()->setTraceLevel(Util::TraceLevel::blockArrival);

    // create a (Source)ModelElement of type EntityType, used by a ModelComponent that follows
    EntityType* carro = new EntityType(elements, "carro");
    elements->insert(Util::TypeOf<EntityType>(), carro); // insert the element into the model
    
    EntityType* comprador = new EntityType(elements, "comprador");
    elements->insert(Util::TypeOf<EntityType>(), comprador);
    
    // create a ModelComponent of type Create, used to insert entities into the model
    Create* create1 = new Create(model);
    create1->setEntityType(carro);
    create1->setTimeBetweenCreationsExpression("1.5"); // create one new entity every 1.5 seconds
    components->insert(create1); // insert the component into the model

    // create a (Sink)ModelComponent of type Dispose, used to remove entities from the model
    Dispose* dispose1 = new Dispose(model);  // insert the component into the model
    components->insert(dispose1);
    
    
    // insert the model into the simulator 
    simulator->getModelManager()->insert(model);

    // if the model is ok then save the model into a text file 
    if (model->checkModel()) {
	model->saveModel("./temp/testRemove.txt");
	// if the model is saved into a file, it could be just loaded instead of built (as above)
    }

    // execute the simulation util completed and show the report
    model->getSimulation()->startSimulation();
    return 0;
};