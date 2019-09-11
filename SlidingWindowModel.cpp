/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SlidingWindowModel.cpp
 * Author: joaofel-u
 * 
 * Created on 11 de Setembro de 2019, 08:27
 */

#include "SlidingWindowModel.h"

// you have to included need libs

// GEneSyS Simulator
#include "Simulator.h"

// Model Components
#include "Create.h"
#include "Hold.h"
#include "Signal.h"
#include "Dispose.h"
#include "Seize.h"
#include "Station.h"
#include "Enter.h"
#include "Leave.h"

// Model elements
#include "EntityType.h"
#include "Queue.h"
#include "Delay.h"
#include "Resource.h"


SlidingWindowModel::SlidingWindowModel() {
}

/**
 * This is the main function of the application. 
 * It instanciates the simulator, builds a simulation model and then simulate that model.
 */
int SlidingWindowModel::main(int argc, char** argv) {
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
    EntityType* pacote = new EntityType(elements, "Pacote");
    elements->insert(Util::TypeOf<EntityType>(), pacote); // insert the element into the model

    // create a ModelComponent of type Create, used to insert entities into the model
    Create* create1 = new Create(model);
    create1->setEntityType(pacote);
    create1->setTimeBetweenCreationsExpression("1.5"); // create one new entity every 1.5 seconds
    components->insert(create1); // insert the component into the model
    
    Queue* queue1 = new Queue(elements, "q1");
    elements->insert(Util::TypeOf<Queue>(), queue1);
    
//    Resource* machine1 = new Resource(elements, "Machine_1");
//    machine1->setCapacity(1);
//    elements->insert(Util::TypeOf<Resource>(), machine1);
    
    Hold* hold1 = new Hold(model);
    hold1->setQueue(queue1);
    //hold1->setResource(machine1);
    components->insert(hold1);
    
    // create a (Sink)ModelComponent of type Dispose, used to remove entities from the model
    Dispose* dispose1 = new Dispose(model);  // insert the component into the model
    components->insert(dispose1);

    // connect model components to create a "workflow" -- should always start from a SourceModelComponent and end at a SinkModelComponent (it will be checked)
    Station* station1 = new Station(elements, "Station 1");
    elements->insert(Util::TypeOf<Station>(), station1);
    Enter* enter1 = new Enter(model);
    enter1->setStation(station1);
    components->insert(enter1);
    
    Leave* leave1 = new Leave(model);
    leave1->setStation(station1);
    components->insert(leave1);
    
    create1->getNextComponents()->insert(enter1);
    enter1->getNextComponents()->insert(hold1);
    hold1->getNextComponents()->insert(leave1);
    leave1->getNextComponents()->insert(dispose1);
    
    // insert the model into the simulator 
    simulator->getModelManager()->insert(model);

    // if the model is ok then save the model into a text file 
    if (model->checkModel()) {
	model->saveModel("./temp/slidingWindowModel.txt");
	// if the model is saved into a file, it could be just loaded instead of built (as above)
    }

    // execute the simulation util completed and show the report
    model->getSimulation()->startSimulation();
    return 0;
};

