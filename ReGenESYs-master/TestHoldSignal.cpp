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
#include "Assign.h"
#include "Hold.h"
#include "Dispose.h"
#include "Signal.h"

// Model elements
#include "Entity.h"
#include "EntityType.h"
#include "Queue.h"
#include "Resource.h"
#include "Variable.h"
#include "Attribute.h"


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
    infos->setNumberOfReplications(1);
    infos->setReplicationLength(42);
    infos->setReplicationLengthTimeUnit(Util::TimeUnit::minute);
    infos->setWarmUpPeriod(0);
    infos->setWarmUpPeriodTimeUnit(Util::TimeUnit::minute);

    //
    // build the simulation model
    //
    simulator->getTraceManager()->setTraceLevel(Util::TraceLevel::blockArrival);

    // create a (Source)ModelElement of type EntityType, used by a ModelComponent that follows
    EntityType* pacote = new EntityType(elements, "pacote");
    elements->insert(Util::TypeOf<EntityType>(), pacote);
    
    EntityType* ack = new EntityType(elements, "ack");
    elements->insert(Util::TypeOf<EntityType>(), ack);
    
    Variable* variablePacotesEnviados = new Variable("PacotesEnviados");
    variablePacotesEnviados->setValue(0);
    elements->insert(Util::TypeOf<Variable>(), variablePacotesEnviados);
    
    Variable* variableAcksEnviados = new Variable("AcksEnviados");
    variableAcksEnviados->setValue(0);
    elements->insert(Util::TypeOf<Variable>(), variableAcksEnviados);
    
    Attribute* attributeSigValue = new Attribute("SigValue");
    elements->insert(Util::TypeOf<Attribute>(), attributeSigValue);
    
    
    /* Components. */
    Create* createPacote = new Create(model);
    createPacote->setName("CreatePacote");
    createPacote->setEntityType(pacote);
    createPacote->setTimeBetweenCreationsExpression("5");
    createPacote->setTimeUnit(Util::TimeUnit::minute);
    createPacote->setFirstCreation(0.0);
    components->insert(createPacote);
    
    Assign* assignPacote = new Assign(model);
    assignPacote->getAssignments()->insert(new Assign::Assignment(Assign::DestinationType::Variable, "PacotesEnviados", "PacotesEnviados + 1"));
    assignPacote->getAssignments()->insert(new Assign::Assignment(Assign::DestinationType::Attribute, "SigValue", "PacotesEnviados"));
    assignPacote->setName("AssignPacote");
    components->insert(assignPacote);
    
    Queue* holdQueue = new Queue(elements, "Queue1");
    elements->insert(Util::TypeOf<Queue>(), holdQueue);
    
    Hold* holdPacote = new Hold(model);
    holdPacote->setName("HoldPacote");
    holdPacote->setQueue(holdQueue);
    holdPacote->setType(Hold::Type::WaitForSignal);
    holdPacote->setWaitForValueExpr("SigValue"); //Entity.SigValue
    components->insert(holdPacote);
    
    Dispose* disposePacote = new Dispose(model);
    disposePacote->setName("DisposePacote");
    components->insert(disposePacote);
    
    /* Ack section */
    Create* createAck = new Create(model);
    createAck->setName("CreateAck");
    createAck->setEntityType(ack);
    createAck->setTimeBetweenCreationsExpression("3");
    createAck->setTimeUnit(Util::TimeUnit::minute);
    createAck->setFirstCreation(30.0);
    components->insert(createAck);
    
    Assign* assignAck = new Assign(model);
    assignAck->getAssignments()->insert(new Assign::Assignment(Assign::DestinationType::Variable, "AcksEnviados", "AcksEnviados + 1"));
    assignAck->getAssignments()->insert(new Assign::Assignment(Assign::DestinationType::Attribute, "SigValue", "AcksEnviados"));
    assignAck->setName("AssignAck");
    components->insert(assignAck);
    
    
    Signal* signalAck = new Signal(model);
    signalAck->setName("SignalAck");
    signalAck->setSignalValue("Entity.SigValue");  // Entity.SigValue
    components->insert(signalAck);
    
    Dispose* disposeAck = new Dispose(model);
    disposeAck->setName("DisposeAck");
    components->insert(disposeAck);
    
    signalAck->getListeners()->insert(holdPacote);
    
    /* Connections. */
    createPacote->getNextComponents()->insert(assignPacote);
    assignPacote->getNextComponents()->insert(holdPacote);
    holdPacote->getNextComponents()->insert(disposePacote);
    
    createAck->getNextComponents()->insert(assignAck);
    assignAck->getNextComponents()->insert(signalAck);
    signalAck->getNextComponents()->insert(disposeAck);
    
    
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