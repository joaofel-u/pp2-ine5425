/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PartialTest.cpp
 * Author: joaofel-u
 *
 * Created on 05 de novembro de 2019, 18:10
 */

#include "PartialTest.h"

// GEneSyS Simulator
#include "Simulator.h"

// Model Components
#include "Create.h"
#include "Assign.h"
#include "Hold.h"
#include "Dispose.h"
#include "Signal.h"
#include "Enter.h"
#include "Leave.h"
#include "Station.h"
#include "Seize.h"
#include "Route.h"

// Model elements
#include "Entity.h"
#include "EntityType.h"
#include "Queue.h"
#include "Resource.h"
#include "Variable.h"
#include "Attribute.h"


PartialTest::PartialTest() {}
PartialTest::PartialTest(const PartialTest& orig) {}
PartialTest::~PartialTest() {}

/**
 * This is the main function of the application. 
 * It instanciates the simulator, builds a simulation model and then simulate that model.
 */
int PartialTest::main(int argc, char** argv) {
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

    /* Entities. */
    EntityType* pacote = new EntityType(elements, "Pacote");
    elements->insert(Util::TypeOf<EntityType>(), pacote);
    
    /* Elements. */
    Attribute* attribute_numeroPacote = new Attribute("NumeroPacote");
    elements->insert(Util::TypeOf<Attribute>(), attribute_numeroPacote);
    
    Attribute* attribute_tamanhoPacote = new Attribute("TamanhoPacote");
    elements->insert(Util::TypeOf<Attribute>(), attribute_tamanhoPacote);
    
    Attribute* attribute_delayCanal = new Attribute("DelayCanal");
    elements->insert(Util::TypeOf<Attribute>(), attribute_delayCanal);
    
    Attribute* attribute_delayTransmissao = new Attribute("DelayTransmissao");
    elements->insert(Util::TypeOf<Attribute>(), attribute_delayTransmissao);
    
    Variable* variable_taxaDeTransmissao = new Variable("TaxaDeTransmissao");
    variable_taxaDeTransmissao->setValue(100);
    elements->insert(Util::TypeOf<Variable>(), variable_taxaDeTransmissao);
    
    Variable* variable_pacotesEnviados = new Variable("PacotesEnviados");
    variable_pacotesEnviados->setValue(0);
    elements->insert(Util::TypeOf<Variable>(), variable_pacotesEnviados);
    
    
    /* Components. */
    Create* create_pacotes = new Create(model);
    Route* route_goToNovoPacote = new Route(model);
    
    /* Secao 'Transmissor'. */
    Enter* enter_novoPacote = new Enter(model);
    Station* station_novoPacote = new Station(elements, "NovoPacote");
    Leave* leave_novoPacote = new Leave(model);
    Assign* assign_atribuiNumero = new Assign(model);
    Assign* assign_atribuiTamanho = new Assign(model);
    Assign* assign_atribuiDelays = new Assign(model);
    Seize* seize_entraNaJanela = new Seize(model);
    Dispose* dispose_goToEnvioTransmissor = new Dispose(model);
    
    
    /* Create 'Pacotes'. */
    create_pacotes->setName("Pacotes");
    create_pacotes->setEntityType(pacote);
    create_pacotes->setTimeBetweenCreationsExpression("1");
    create_pacotes->setTimeUnit(Util::TimeUnit::second);
    create_pacotes->setEntitiesCreated(1);
    create_pacotes->setMaxCreations("MR(JanelaDeslizante)");
    create_pacotes->setFirstCreation(0.0);
    components->insert(create_pacotes);
    
    
    /* Route 'GoToNovoPacote'. */
    route_goToNovoPacote->setName("GoToNovoPacote");
    route_goToNovoPacote->setRouteTimeExpression("0");
    route_goToNovoPacote->setRouteTimeTimeUnit(Util::TimeUnit::second);
    route_goToNovoPacote->setRouteDestinationType(Route::DestinationType::Station);
    route_goToNovoPacote->setStation(station_novoPacote);
    components->insert(route_goToNovoPacote);
    
    
    /* Connect the components from 'Cria N pacotes'. */
    create_pacotes->getNextComponents()->insert(route_goToNovoPacote);
    
    
    /*
     * Secao Transmissor.
     */
    /* Station 'NovoPacote'. */
    elements->insert(Util::TypeOf<Station>(), station_novoPacote);
    station_novoPacote->setEnterIntoStationComponent(enter_novoPacote);
    
    enter_novoPacote->setName("EnterNovoPacote");
    enter_novoPacote->setStation(station_novoPacote);
    components->insert(enter_novoPacote);
    
    leave_novoPacote->setName("LeaveNovoPacote");
    leave_novoPacote->setStation(station_novoPacote);
    components->insert(leave_novoPacote);
    
    
    /* Assign 'AtribuiNumero'. */
    assign_atribuiNumero->setName("AtribuiNumero");
    assign_atribuiNumero->getAssignments()->insert(new Assign::Assignment(Assign::DestinationType::Attribute, "NumeroPacote", "PacotesEnviados + 1"));
    assign_atribuiNumero->getAssignments()->insert(new Assign::Assignment(Assign::DestinationType::Variable, "PacotesEnviados", "PacotesEnviados + 1"));
    components->insert(assign_atribuiNumero);
    
    
    /* Assign 'AtribuiTamanho'. */
    assign_atribuiTamanho->setName("AtribuiTamanho");
    assign_atribuiNumero->getAssignments()->insert(new Assign::Assignment(Assign::DestinationType::Attribute, "TamanhoPacote", "1160 + 339 * BETA(5.8,1.53,1160,1500)"));
    components->insert(assign_atribuiTamanho);
    
    
    /* Assign 'AtribuiDelays'. */
    assign_atribuiDelays->setName("AtribuiDelays");
    assign_atribuiNumero->getAssignments()->insert(new Assign::Assignment(Assign::DestinationType::Attribute, "DelayCanal", "UNIF(1,50)"));
    assign_atribuiNumero->getAssignments()->insert(new Assign::Assignment(Assign::DestinationType::Attribute, "DelayTransmissao", "TamanhoPacote * 8 / TaxaDeTransmissao"));
    components->insert(assign_atribuiDelays);
    
    
    /* Seize 'EntraNaJanela'. */
    Resource* resource_janelaDeslizante = new Resource(elements, "JanelaDeslizante");
    resource_janelaDeslizante->setCapacity(5);
    elements->insert(Util::TypeOf<Resource>(), resource_janelaDeslizante);
    
    Queue* queue_filaJanela = new Queue(elements, "FilaJanela");
    queue_filaJanela->setOrderRule(Queue::OrderRule::FIFO);
    elements->insert(Util::TypeOf<Queue>(), queue_filaJanela);
    
    seize_entraNaJanela->setName("EntraNaJanela");
    seize_entraNaJanela->setResourceType(Resource::ResourceType::RESOURCE);
    seize_entraNaJanela->setResource(resource_janelaDeslizante);
    /* @todo See if it's necessary to define allocationType (where the stat is saved). */
    seize_entraNaJanela->setPriority(2);
    seize_entraNaJanela->setQueue(queue_filaJanela);
    seize_entraNaJanela->setQuantity("1.0");
    components->insert(seize_entraNaJanela);
    
    
    /* Dispose 'GoToEnvioTransmissor'. */
    dispose_goToEnvioTransmissor->setName("GoToEnvioTransmissor");
    components->insert(dispose_goToEnvioTransmissor);
    
    
    /* Connect the components from 'Transmissor'. */
    enter_novoPacote->getNextComponents()->insert(leave_novoPacote);
    leave_novoPacote->getNextComponents()->insert(assign_atribuiNumero);
    assign_atribuiNumero->getNextComponents()->insert(assign_atribuiTamanho);
    assign_atribuiTamanho->getNextComponents()->insert(assign_atribuiDelays);
    assign_atribuiDelays->getNextComponents()->insert(seize_entraNaJanela);
    seize_entraNaJanela->getNextComponents()->insert(dispose_goToEnvioTransmissor);
    
    
    // insert the model into the simulator 
    simulator->getModelManager()->insert(model);

    // if the model is ok then save the model into a text file 
    if (model->checkModel()) {
	model->saveModel("./temp/partialTest.txt");
    }

    // execute the simulation util completed and show the report
    model->getSimulation()->startSimulation();
    return 0;
};
