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
#include "Delay.h"
#include "Dispose.h"
#include "Release.h"
#include "Decide.h"
#include "Hold.h"
#include "Seize.h"
#include "Signal.h"

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
    infos->setReplicationLength(299);
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
    
    EntityType* ack = new EntityType(elements, "Ack");
    elements->insert(Util::TypeOf<EntityType>(), ack);
    
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
    variable_taxaErro->setValue(0.05);
    elements->insert(Util::TypeOf<Variable>(), variable_taxaErro);
    
    
    /* Components. */
    Create* create_aguardeAck = new Create(model);
    Assign* assign1 = new Assign(model);
    Hold* hold_holdAck = new Hold(model);
    Dispose* dispose_reciclaPacotes = new Dispose(model);
    
    Create* create_inTransmissor = new Create(model);
    Assign* assign2 = new Assign(model);
    Decide* decide_verificaFilaAck = new Decide(model);
    Dispose* dispose_ignoraAck = new Dispose(model);
    Decide* decide_verificaAck = new Decide(model);
    Signal* signal_retiraDaFilaAck = new Signal(model);
    Dispose* dispose_descartaAck = new Dispose(model);
    
    /* Station 'AguardeAck'. */
    create_aguardeAck->setName("CreatePacote");
    create_aguardeAck->setEntityType(pacote);
    create_aguardeAck->setTimeBetweenCreationsExpression("1");
    create_aguardeAck->setTimeUnit(Util::TimeUnit::second);
    create_aguardeAck->setEntitiesCreated(1);
    create_aguardeAck->setFirstCreation(0.0);
    components->insert(create_aguardeAck);
    
    assign1->setName("AtribuiNumero1");
    assign1->getAssignments()->insert(new Assign::Assignment(Assign::DestinationType::Attribute, "NumeroPacote", "PacotesEnviados + 1"));
    assign1->getAssignments()->insert(new Assign::Assignment(Assign::DestinationType::Variable, "PacotesEnviados", "PacotesEnviados + 1"));
    components->insert(assign1);
    
    
    /* Hold 'HoldAck'. */
    Queue* queue_filaAck = new Queue(elements, "FilaAck");
    queue_filaAck->setOrderRule(Queue::OrderRule::FIFO);
    elements->insert(Util::TypeOf<Queue>(), queue_filaAck);
    
    hold_holdAck->setName("HoldAck");
    hold_holdAck->setType(Hold::Type::WaitForSignal);
    hold_holdAck->setWaitForValueExpr("NumeroPacote");
    hold_holdAck->setLimit(0);
    hold_holdAck->setQueue(queue_filaAck);
    components->insert(hold_holdAck);
    
    
    /* Route 'ReciclaPacotes'. */
    dispose_reciclaPacotes->setName("ReciclaPacotes");
    components->insert(dispose_reciclaPacotes);
    
    /* Connect the components that control the ACK waiting. */
    create_aguardeAck->getNextComponents()->insert(assign1);
    assign1->getNextComponents()->insert(hold_holdAck);
    hold_holdAck->getNextComponents()->insert(dispose_reciclaPacotes);
    
    
    /* Station 'InTransmissor'. */
    create_inTransmissor->setName("CreateAck");
    create_inTransmissor->setEntityType(ack);
    create_inTransmissor->setTimeBetweenCreationsExpression("1");
    create_inTransmissor->setTimeUnit(Util::TimeUnit::second);
    create_inTransmissor->setEntitiesCreated(1);
    create_inTransmissor->setFirstCreation(1.0);
    components->insert(create_inTransmissor);
    
    assign2->setName("AtribuiNumero2");
    assign2->getAssignments()->insert(new Assign::Assignment(Assign::DestinationType::Attribute, "NumeroPacote", "PacotesEnviados"));
    components->insert(assign2);
    
    /* Decide 'VerificaFilaAck'. */
    decide_verificaFilaAck->setName("VerificaFilaAck");
    decide_verificaFilaAck->getConditions()->insert("NQ(FilaAck) > 0");
    components->insert(decide_verificaFilaAck);
    
    
    /* Dispose 'IgnoraAck'. */
    dispose_ignoraAck->setName("IgnoraAck");
    components->insert(dispose_ignoraAck);
    
    
    /* Decide 'VerificaAck'. */
    decide_verificaAck->setName("VerificaAck");
    decide_verificaAck->getConditions()->insert("NumeroPacote == AQUE(FilaAck,1,NumeroPacote)");
    components->insert(decide_verificaAck);
    
    
    /* Signal 'RetiraDaFilaAck'. */
    signal_retiraDaFilaAck->setName("RetiraDaFilaAck");
    signal_retiraDaFilaAck->setLimit(0);
    signal_retiraDaFilaAck->setSignalValue("NumeroPacote");
    components->insert(signal_retiraDaFilaAck);
    
    signal_retiraDaFilaAck->getListeners()->insert(hold_holdAck);
    
    /* Dispose 'DescartaAck'. */
    dispose_descartaAck->setName("DescartaAck");
    components->insert(dispose_descartaAck);
    
    
    /* Connect the components that control the ACK received check. */
    create_inTransmissor->getNextComponents()->insert(assign2);
    assign2->getNextComponents()->insert(decide_verificaFilaAck);
    decide_verificaFilaAck->getNextComponents()->insert(decide_verificaAck);
    decide_verificaFilaAck->getNextComponents()->insert(dispose_ignoraAck);
    decide_verificaAck->getNextComponents()->insert(signal_retiraDaFilaAck);
    decide_verificaAck->getNextComponents()->insert(dispose_ignoraAck);
    signal_retiraDaFilaAck->getNextComponents()->insert(dispose_descartaAck);
    
    
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
