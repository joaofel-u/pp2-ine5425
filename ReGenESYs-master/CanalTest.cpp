/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CanalTest.cpp
 * Author: joaofel-u
 *
 * Created on 05 de novembro de 2019, 18:10
 */

#include "CanalTest.h"

// GEneSyS Simulator
#include "Simulator.h"

// Model Components
#include "Create.h"
#include "Assign.h"
#include "Delay.h"
#include "Dispose.h"
#include "Release.h"
#include "Decide.h"
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


CanalTest::CanalTest() {}
CanalTest::CanalTest(const CanalTest& orig) {}
CanalTest::~CanalTest() {}

/**
 * This is the main function of the application. 
 * It instanciates the simulator, builds a simulation model and then simulate that model.
 */
int CanalTest::main(int argc, char** argv) {
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
    infos->setReplicationLength(999);
    infos->setReplicationLengthTimeUnit(Util::TimeUnit::second);
    infos->setWarmUpPeriod(0);
    infos->setWarmUpPeriodTimeUnit(Util::TimeUnit::second);

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
    
    Variable* variable_taxaErro = new Variable("TaxaErro");
    variable_taxaErro->setValue(0.01);
    elements->insert(Util::TypeOf<Variable>(), variable_taxaErro);
    
    
    /* Components. */
    // Transmissor > Receptor
    Create* create_outTransmissor = new Create(model);
    Seize* seize_alocaRotaPacote = new Seize(model);
    Delay* delay_latenciaCanal = new Delay(model);
    Release* release_livraRotaPacote = new Release(model);
    Decide* decide_decideErroPacote = new Decide(model);
    Dispose* dispose_goToInReceptor = new Dispose(model);
    Dispose* dispose_erroPacote = new Dispose(model);
    
    
    /* Station 'OutTransmissor'. */
    create_outTransmissor->setName("Pacotes");
    create_outTransmissor->setEntityType(pacote);
    create_outTransmissor->setTimeBetweenCreationsExpression("1");
    create_outTransmissor->setTimeUnit(Util::TimeUnit::second);
    create_outTransmissor->setEntitiesCreated(1);
    create_outTransmissor->setFirstCreation(0.0);
    components->insert(create_outTransmissor);
    
    
    /* Seize 'AlocaRotaPacote'. */
    Resource* resource_rotaPacote = new Resource(elements, "RotaPacote");
    resource_rotaPacote->setCapacity(1);
    elements->insert(Util::TypeOf<Resource>(), resource_rotaPacote);
    
    Queue* queue_filaAlocaRotaPacote = new Queue(elements, "FilaAlocaRotaPacote");
    queue_filaAlocaRotaPacote->setOrderRule(Queue::OrderRule::FIFO);
    elements->insert(Util::TypeOf<Queue>(), queue_filaAlocaRotaPacote);
    
    seize_alocaRotaPacote->setName("AlocaRotaPacote");
    seize_alocaRotaPacote->setResourceType(Resource::ResourceType::RESOURCE);
    seize_alocaRotaPacote->setResource(resource_rotaPacote);
    /* @todo See if it's necessary to define allocationType (where the stat is saved). */
    seize_alocaRotaPacote->setPriority(2);
    seize_alocaRotaPacote->setQueue(queue_filaAlocaRotaPacote);
    seize_alocaRotaPacote->setQuantity("1.0");
    components->insert(seize_alocaRotaPacote);
    
    
    /* Delay 'LatenciaCanal'. */
    delay_latenciaCanal->setName("LatenciaCanal");
    /* @todo Ver necessidade de especificar allocation. */
    delay_latenciaCanal->setDelayExpression("DelayCanal");
    delay_latenciaCanal->setDelayTimeUnit(Util::TimeUnit::second);
    components->insert(delay_latenciaCanal);
    
    
    /* Release 'LivraRotaPacote'. */
    release_livraRotaPacote->setName("LivraRotaPacote");
    release_livraRotaPacote->setResourceType(Resource::ResourceType::RESOURCE);
    release_livraRotaPacote->setResource(resource_rotaPacote);
    release_livraRotaPacote->setQuantity("1");
    components->insert(release_livraRotaPacote);
    
    
    /* Decide 'DecideErroPacote'. */
    decide_decideErroPacote->setName("DecideErroPacote");
    decide_decideErroPacote->getConditions()->insert("UNIF(0,1) <= (0.99 - TaxaErro)");
    components->insert(decide_decideErroPacote);
    
    
    /* Dispose 'GoToInReceptor'. */
    dispose_goToInReceptor->setName("GoToInReceptor");
    components->insert(dispose_goToInReceptor);
    
    
    /* Dispose 'ErroPacote'. */
    dispose_erroPacote->setName("ErroPacote");
    components->insert(dispose_erroPacote);
    
    
    /* Connect the components of 'Canal de Transmissao - Transmissor > Receptor'*/
    create_outTransmissor->getNextComponents()->insert(seize_alocaRotaPacote);
    seize_alocaRotaPacote->getNextComponents()->insert(delay_latenciaCanal);
    delay_latenciaCanal->getNextComponents()->insert(release_livraRotaPacote);
    release_livraRotaPacote->getNextComponents()->insert(decide_decideErroPacote);
    decide_decideErroPacote->getNextComponents()->insert(dispose_goToInReceptor);
    decide_decideErroPacote->getNextComponents()->insert(dispose_erroPacote);
    
    
    // insert the model into the simulator 
    simulator->getModelManager()->insert(model);

    // if the model is ok then save the model into a text file 
    if (model->checkModel()) {
	model->saveModel("./temp/canalTest.txt");
    }

    // execute the simulation util completed and show the report
    model->getSimulation()->startSimulation();
    return 0;
};
