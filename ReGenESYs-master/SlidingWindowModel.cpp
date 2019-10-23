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

// GEneSyS Simulator
#include "Simulator.h"
#include "ModelInfo.h"

// Model Components
#include "Assign.h"
#include "Create.h"
#include "Decide.h"
#include "Delay.h"
#include "Dispose.h"
#include "Enter.h"
#include "Hold.h"
#include "Leave.h"
#include "Record.h"
#include "Remove.h"
#include "Route.h"
#include "Seize.h"
#include "Separate.h"
#include "Signal.h"
#include "Release.h"


// Model elements
#include "Attribute.h"
#include "Counter.h"
#include "EntityType.h"
#include "Queue.h"
#include "Resource.h"
#include "Station.h"
#include "Variable.h"


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
    
    /* Creates an empty model. */
    Model* model = new Model(simulator);

    // Handle traces and simulation events to output them
    TraceManager* tm = model->getTraceManager();
    this->setDefaultTraceHandlers(tm);

    // get easy access to classes used to insert components and elements into a model
    ComponentManager* components = model->getComponentManager();
    ElementManager* elements = model->getElementManager();

    /* Build the simulation model. */
    simulator->getTraceManager()->setTraceLevel(Util::TraceLevel::blockArrival);
    
    /*
     * @brief Defines the replication params.
     */
    ModelInfo* infos = model->getInfos();
    infos->setAnalystName("Joao Fellipe Uller");
    infos->setNumberOfReplications(10);
    infos->setReplicationLength(1000000);
    infos->setReplicationLengthTimeUnit(Util::TimeUnit::minute);
    infos->setWarmUpPeriod(100000);
    infos->setWarmUpPeriodTimeUnit(Util::TimeUnit::minute);

    
    /*
     * @brief Defines the entities of the model.
     */
    EntityType* pacote = new EntityType(elements, "Pacote");
    elements->insert(Util::TypeOf<EntityType>(), pacote);
    
    /*
     * @brief Defines all the existing components on the model.
     */
    /* Secao 'Cria N pacotes'. */
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
    Route* route_goToEnvioTransmissor = new Route(model);
    
    /* Secao 'Envio transmissor'. */
    Enter* enter_envioTransmissor = new Enter(model);
    Station* station_envioTransmissor = new Station(elements, "EnvioTransmissor");
    Leave* leave_envioTransmissor = new Leave(model);
    Separate* separate_criaEsperaAck = new Separate(model);
    Route* route_goToAguardeAck = new Route(model);
    Seize* seize_alocaCanal = new Seize(model);
    Delay* delay_atrasoDeTransmissao = new Delay(model);
    Release* release_livraCanal = new Release(model);
    Separate* separate_criaTimeout = new Separate(model);
    Route* route_goToCountTimeout = new Route(model);
    Route* route_goToOutTransmissor = new Route(model);
    
    /* Secao 'Canal de transmissão'. */
    // Transmissor > Receptor
    Enter* enter_outTransmissor = new Enter(model);
    Station* station_outTransmissor = new Station(elements, "OutTransmissor");
    Leave* leave_outTransmissor = new Leave(model);
    Seize* seize_alocaRotaPacote = new Seize(model);
    Delay* delay_latenciaCanal = new Delay(model);
    Release* release_livraRotaPacote = new Release(model);
    Decide* decide_decideErroPacote = new Decide(model);
    Route* route_goToInReceptor = new Route(model);
    Dispose* dispose_erroPacote = new Dispose(model);
    
    // Receptor > Transmissor
    Enter* enter_outReceptor = new Enter(model);
    Station* station_outReceptor = new Station(elements, "OutReceptor");
    Leave* leave_outReceptor = new Leave(model);
    Seize* seize_alocaRotaAck = new Seize(model);
    Delay* delay_latenciaCanalAck = new Delay(model);
    Release* release_livraRotaAck = new Release(model);
    Decide* decide_decideErroAck = new Decide(model);
    Route* route_goToInTransmissor = new Route(model);
    Dispose* dispose_erroAck = new Dispose(model);
    
    
    /* Secao 'Receptor'. */
    Enter* enter_inReceptor = new Enter(model);
    Station* station_inReceptor = new Station(elements, "InReceptor");
    Leave* leave_inReceptor = new Leave(model);
    Assign* assign_converteAck = new Assign(model);
    Assign* assign_atribuiDelaysAck = new Assign(model);
    Route* route_goToEnvioReceptor = new Route(model);
    
    
    /* Secao 'Envio Receptor'. */
    Enter* enter_envioReceptor = new Enter(model);
    Station* station_envioReceptor = new Station(elements, "EnvioReceptor");
    Leave* leave_envioReceptor = new Leave(model);
    Seize* seize_alocaCanalAck = new Seize(model);
    Delay* delay_atrasoDeTransmissaoAck = new Delay(model);
    Release* release_livraCanalAck = new Release(model);
    Route* route_goToOutReceptor = new Route(model);
    
    
    /* Secao 'Aguarda ACK'. */
    Enter* enter_aguardeAck = new Enter(model);
    Station* station_aguardeAck = new Station(elements, "AguardeAck");
    Leave* leave_aguardeAck = new Leave(model);
    Hold* hold_holdAck = new Hold(model);
    Release* release_livraJanela = new Release(model);
    Record* record_contaEnviadosComSucesso = new Record(model);
    Route* route_reciclaPacotes = new Route(model);
    
    Enter* enter_inTransmissor = new Enter(model);
    Station* station_inTransmissor = new Station(elements, "InTransmissor");;
    Leave* leave_inTransmissor = new Leave(model);
    Decide* decide_verificaFilaAck = new Decide(model);
    Dispose* dispose_ignoraAck = new Dispose(model);
    Decide* decide_verificaAck = new Decide(model);
    Signal* signal_retiraDaFilaAck = new Signal(model);
    Dispose* dispose_descartaAck = new Dispose(model);
    
    Enter* enter_countTimeout = new Enter(model);
    Station* station_countTimeout = new Station(elements, "CountTimeout");;
    Leave* leave_countTimeout = new Leave(model);
    Delay* delay_timeoutAck = new Delay(model);
    Decide* decide_verificaFilaAckTimeout = new Decide(model);
    Dispose* dispose_descartaTimeout = new Dispose(model);
    Decide* decide_verificaTimeout = new Decide(model);
    Decide* decide_removeFilaInteira = new Decide(model);
    Remove* remove_removeDaFila = new Remove(model);
    Record* record_contaReenviados = new Record(model);
    Route* route_reenviaPacote = new Route(model);
    
    
    /*
     * @brief Instantiates all attributes and variables that will be used on the model.
     */
    Attribute* attribute_numeroPacote = new Attribute("NumeroPacote");
    elements->insert(Util::TypeOf<Attribute>(), attribute_numeroPacote);
    
    Attribute* attribute_tamanhoPacote = new Attribute("TamanhoPacote");
    elements->insert(Util::TypeOf<Attribute>(), attribute_tamanhoPacote);
    
    Attribute* attribute_delayCanal = new Attribute("DelayCanal");
    elements->insert(Util::TypeOf<Attribute>(), attribute_delayCanal);
    
    Attribute* attribute_delayTransmissao = new Attribute("DelayTransmissao");
    elements->insert(Util::TypeOf<Attribute>(), attribute_delayTransmissao);
    
    Variable* variable_tempoMaximoAck = new Variable("TempoMaximoAck");
    variable_tempoMaximoAck->setValue(75);
    elements->insert(Util::TypeOf<Variable>(), variable_tempoMaximoAck);
    
    Variable* variable_taxaErro = new Variable("TaxaErro");
    variable_taxaErro->setValue(0.01);
    elements->insert(Util::TypeOf<Variable>(), variable_taxaErro);
    
    Variable* variable_taxaDeTransmissao = new Variable("TaxaDeTransmissao");
    variable_taxaDeTransmissao->setValue(100);
    elements->insert(Util::TypeOf<Variable>(), variable_taxaDeTransmissao);
    
    Variable* variable_tamanhoPacoteAck = new Variable("TamanhoPacoteAck");
    variable_tamanhoPacoteAck->setValue(128);
    elements->insert(Util::TypeOf<Variable>(), variable_tamanhoPacoteAck);
    
    Variable* variable_pacotesEnviados = new Variable("PacotesEnviados");
    variable_pacotesEnviados->setValue(0);
    elements->insert(Util::TypeOf<Variable>(), variable_pacotesEnviados);
    
    Counter* counter_pacotesReenviados = new Counter(elements);
    counter_pacotesReenviados->setName("PacotesReenviados");
    elements->insert(Util::TypeOf<Counter>(), counter_pacotesReenviados);
    
    Counter* counter_pacotesEnviadosComSucesso = new Counter(elements);
    counter_pacotesEnviadosComSucesso->setName("PacotesEnviadosComSucesso");
    elements->insert(Util::TypeOf<Counter>(), counter_pacotesEnviadosComSucesso);
    
    
    /* Instantiates all stations that will be used on the model. */
      
    /*
     * Secao Cria N pacotes.
     */
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
    
    
    /* Route 'GoToEnvioTransmissor'. */
    route_goToEnvioTransmissor->setName("GoToEnvioTransmissor");
    route_goToEnvioTransmissor->setRouteTimeExpression("0");
    route_goToEnvioTransmissor->setRouteTimeTimeUnit(Util::TimeUnit::second);
    route_goToEnvioTransmissor->setRouteDestinationType(Route::DestinationType::Station);
    route_goToEnvioTransmissor->setStation(station_envioTransmissor);
    components->insert(route_goToEnvioTransmissor);
    
    
    /* Connect the components from 'Transmissor'. */
    enter_novoPacote->getNextComponents()->insert(leave_novoPacote);
    leave_novoPacote->getNextComponents()->insert(assign_atribuiNumero);
    assign_atribuiNumero->getNextComponents()->insert(assign_atribuiTamanho);
    assign_atribuiTamanho->getNextComponents()->insert(assign_atribuiDelays);
    assign_atribuiDelays->getNextComponents()->insert(seize_entraNaJanela);
    seize_entraNaJanela->getNextComponents()->insert(route_goToEnvioTransmissor);
    
    
    /*
     * Secao Envio transmissor.
     */
    /* Station 'EnvioTransmissor'. */
    elements->insert(Util::TypeOf<Station>(), station_envioTransmissor);
    station_envioTransmissor->setEnterIntoStationComponent(enter_envioTransmissor);
    
    enter_envioTransmissor->setName("EnterEnvioTransmissor");
    enter_envioTransmissor->setStation(station_envioTransmissor);
    components->insert(enter_envioTransmissor);
    
    leave_envioTransmissor->setName("LeaveEnvioTransmissor");
    leave_envioTransmissor->setStation(station_envioTransmissor);
    components->insert(leave_envioTransmissor);
    
    
    /* Separate 'CriaEsperaAck'. */
    separate_criaEsperaAck->setName("CriaEsperaAck");
    separate_criaEsperaAck->setAmountToDuplicate("1");
    separate_criaEsperaAck->setSplitBatch(0);
    /* @todo See replication costs. 
     * separate_criaEsperaAck->setPercentCostToDup("50");
     */
    components->insert(separate_criaEsperaAck);
    
    
    /* Route 'GoToAguardeAck'. */
    route_goToAguardeAck->setName("GoToAguardeAck");
    route_goToAguardeAck->setRouteTimeExpression("0");
    route_goToAguardeAck->setRouteTimeTimeUnit(Util::TimeUnit::second);
    route_goToAguardeAck->setRouteDestinationType(Route::DestinationType::Station);
    route_goToAguardeAck->setStation(station_aguardeAck);
    components->insert(route_goToAguardeAck);

    
    /* Seize 'AlocaCanal'. */
    Resource* resource_canalDeTransmissaoPacote = new Resource(elements, "CanalDeTransmissaoPacote");
    resource_canalDeTransmissaoPacote->setCapacity(1);
    elements->insert(Util::TypeOf<Resource>(), resource_canalDeTransmissaoPacote);
    
    Queue* queue_filaTransmissaoPacote = new Queue(elements, "FilaTransmissaoPacote");
    queue_filaTransmissaoPacote->setOrderRule(Queue::OrderRule::FIFO);
    elements->insert(Util::TypeOf<Queue>(), queue_filaTransmissaoPacote);
    
    seize_alocaCanal->setName("AlocaCanal");
    seize_alocaCanal->setResourceType(Resource::ResourceType::RESOURCE);
    seize_alocaCanal->setResource(resource_canalDeTransmissaoPacote);
    /* @todo See if it's necessary to define allocationType (where the stat is saved). */
    seize_alocaCanal->setPriority(2);
    seize_alocaCanal->setQueue(queue_filaTransmissaoPacote);
    seize_alocaCanal->setQuantity("1.0");
    components->insert(seize_alocaCanal);
    
    
    /* Delay 'AtrasoDeTransmissao'. */
    delay_atrasoDeTransmissao->setName("AtrasoDeTransmissao");
    /* @todo Ver necessidade de especificar allocation. */
    delay_atrasoDeTransmissao->setDelayExpression("DelayTransmissao"); /* See if it's correct. */
    delay_atrasoDeTransmissao->setDelayTimeUnit(Util::TimeUnit::second);
    components->insert(delay_atrasoDeTransmissao);
    
    
    /* Release 'LivraCanal'. */
    release_livraCanal->setName("LivraCanal");
    release_livraCanal->setResourceType(Resource::ResourceType::RESOURCE);
    release_livraCanal->setResource(resource_canalDeTransmissaoPacote);
    release_livraCanal->setQuantity("1");
    components->insert(release_livraCanal);
    
    
    /* Separate 'CriaTimeout'. */ 
    separate_criaTimeout->setName("CriaTimeout");
    separate_criaTimeout->setAmountToDuplicate("1");
    separate_criaTimeout->setSplitBatch(0);
    /* @todo See replication costs. 
     * separate_criaTimeout->setPercentCostToDup("50");
     */
    components->insert(separate_criaTimeout);
    
    
    /* Route 'GoToCountTimeout'. */
    route_goToCountTimeout->setName("GoToCountTimeout");
    route_goToCountTimeout->setRouteTimeExpression("0");
    route_goToCountTimeout->setRouteTimeTimeUnit(Util::TimeUnit::second);
    route_goToCountTimeout->setRouteDestinationType(Route::DestinationType::Station);
    route_goToCountTimeout->setStation(station_countTimeout);
    components->insert(route_goToCountTimeout);
    
    
    /* Route 'GoToOutTransmissor'. */
    route_goToOutTransmissor->setName("GoToOutTransmissor");
    route_goToOutTransmissor->setRouteTimeExpression("0");
    route_goToOutTransmissor->setRouteTimeTimeUnit(Util::TimeUnit::second);
    route_goToOutTransmissor->setRouteDestinationType(Route::DestinationType::Station);
    route_goToOutTransmissor->setStation(station_outTransmissor);
    components->insert(route_goToOutTransmissor);
    
    
    /* Connect the components from 'Envio transmissor'. */
    enter_envioTransmissor->getNextComponents()->insert(leave_envioTransmissor);
    leave_envioTransmissor->getNextComponents()->insert(separate_criaEsperaAck);
    separate_criaEsperaAck->getNextComponents()->insert(seize_alocaCanal);
    separate_criaEsperaAck->getNextComponents()->insert(route_goToAguardeAck);
    seize_alocaCanal->getNextComponents()->insert(delay_atrasoDeTransmissao);
    delay_atrasoDeTransmissao->getNextComponents()->insert(release_livraCanal);
    release_livraCanal->getNextComponents()->insert(separate_criaTimeout);
    separate_criaTimeout->getNextComponents()->insert(route_goToOutTransmissor);
    separate_criaTimeout->getNextComponents()->insert(route_goToCountTimeout);
    
    
    /*
     * Secao Aguarda ACK.
     */
    /* Station 'AguardeAck'. */
    elements->insert(Util::TypeOf<Station>(), station_aguardeAck);
    station_aguardeAck->setEnterIntoStationComponent(enter_aguardeAck);
    
    enter_aguardeAck->setName("EnterAguardeAck");
    enter_aguardeAck->setStation(station_aguardeAck);
    components->insert(enter_aguardeAck);
    
    leave_aguardeAck->setName("LeaveAguardeAck");
    leave_aguardeAck->setStation(station_aguardeAck);
    components->insert(leave_aguardeAck);
    
    
    /* Hold 'HoldAck'. 
     *
     * @todo See Hold implementation
     */
    Queue* queue_filaAck = new Queue(elements, "FilaAck");
    queue_filaAck->setOrderRule(Queue::OrderRule::FIFO);
    elements->insert(Util::TypeOf<Queue>(), queue_filaAck);
    
    hold_holdAck->setName("HoldAck");
    hold_holdAck->setType(Hold::Type::WaitForSignal);
    hold_holdAck->setWaitForValueExpr("NumeroPacote");
    hold_holdAck->setLimit(0);
    hold_holdAck->setQueue(queue_filaAck);
    components->insert(hold_holdAck);
    
    
    /* Release 'LivraJanela'. */
    release_livraJanela->setName("LivraJanela");
    release_livraJanela->setResourceType(Resource::ResourceType::RESOURCE);
    release_livraJanela->setResource(resource_janelaDeslizante);
    release_livraJanela->setQuantity("1");
    components->insert(release_livraJanela);
    
    
    /* 
     * Record 'ContaEnviadosComSucesso'. 
     * 
     * @todo Define correct parameters.
     */
    record_contaEnviadosComSucesso->setName("ContaEnviadosComSucesso");
    components->insert(record_contaEnviadosComSucesso);
    
    
    /* Route 'ReciclaPacotes'. */
    route_reciclaPacotes->setName("ReciclaPacotes");
    route_reciclaPacotes->setRouteTimeExpression("0");
    route_reciclaPacotes->setRouteTimeTimeUnit(Util::TimeUnit::second);
    route_reciclaPacotes->setRouteDestinationType(Route::DestinationType::Station);
    route_reciclaPacotes->setStation(station_novoPacote);
    components->insert(route_reciclaPacotes);
    
    /* Connect the components that control the ACK waiting. */
    enter_aguardeAck->getNextComponents()->insert(leave_aguardeAck);
    leave_aguardeAck->getNextComponents()->insert(hold_holdAck);
    hold_holdAck->getNextComponents()->insert(release_livraJanela);
    release_livraJanela->getNextComponents()->insert(record_contaEnviadosComSucesso);
    record_contaEnviadosComSucesso->getNextComponents()->insert(route_reciclaPacotes);
    
    
    /* Station 'InTransmissor'. */
    elements->insert(Util::TypeOf<Station>(), station_inTransmissor);
    station_inTransmissor->setEnterIntoStationComponent(enter_inTransmissor);
    
    enter_inTransmissor->setName("EnterInTransmissor");
    enter_inTransmissor->setStation(station_inTransmissor);
    components->insert(enter_inTransmissor);
    
    leave_inTransmissor->setName("LeaveInTransmissor");
    leave_inTransmissor->setStation(station_inTransmissor);
    components->insert(leave_inTransmissor);
    
    
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
    
    
    /* 
     * Signal 'RetiraDaFilaAck'. 
     * 
     * @todo See Signal implementation.
     * @todo Define correct parameters.
     */
    signal_retiraDaFilaAck->setName("RetiraDaFilaAck");
    components->insert(signal_retiraDaFilaAck);
    
    
    /* Dispose 'DescartaAck'. */
    dispose_descartaAck->setName("DescartaAck");
    components->insert(dispose_descartaAck);
    
    
    /* Connect the components that control the ACK received check. */
    enter_inTransmissor->getNextComponents()->insert(leave_inTransmissor);
    leave_inTransmissor->getNextComponents()->insert(decide_verificaFilaAck);
    decide_verificaFilaAck->getNextComponents()->insert(decide_verificaAck);
    decide_verificaFilaAck->getNextComponents()->insert(dispose_ignoraAck);
    decide_verificaAck->getNextComponents()->insert(signal_retiraDaFilaAck);
    decide_verificaAck->getNextComponents()->insert(dispose_ignoraAck);
    signal_retiraDaFilaAck->getNextComponents()->insert(dispose_descartaAck);
    
    
    /* Station 'CountTimeout'. */
    elements->insert(Util::TypeOf<Station>(), station_countTimeout);
    station_countTimeout->setEnterIntoStationComponent(enter_countTimeout);
    
    enter_countTimeout->setName("EnterCountTimeout");
    enter_countTimeout->setStation(station_countTimeout);
    components->insert(enter_countTimeout);
    
    leave_countTimeout->setName("LeaveCountTimeout");
    leave_countTimeout->setStation(station_countTimeout);
    components->insert(leave_countTimeout);
    
    
    /* Delay 'TimeoutAck'. */
    delay_timeoutAck->setName("TimeoutAck");
    /* @todo Ver necessidade de especificar allocation. */
    delay_timeoutAck->setDelayExpression("TempoMaximoAck");
    delay_timeoutAck->setDelayTimeUnit(Util::TimeUnit::second);
    components->insert(delay_timeoutAck);
    
    
    /* Decide 'VerificaFilaAckTimeout'. */
    decide_verificaFilaAckTimeout->setName("VerificaFilaAckTimeout");
    decide_verificaFilaAckTimeout->getConditions()->insert("NQ(FilaAck) > 0");
    components->insert(decide_verificaFilaAckTimeout);
    
    
    /* Dispose 'DescartaTimeout'. */
    dispose_descartaTimeout->setName("DescartaTimeout");
    components->insert(dispose_descartaTimeout);
    
    
    /* Decide 'VerificaTimeout'. */
    decide_verificaTimeout->setName("VerificaTimeout");
    decide_verificaTimeout->getConditions()->insert("NumeroPacote == AQUE(FilaAck,1,NumeroPacote)");
    components->insert(decide_verificaTimeout);
    
    
    /* Decide 'RemoveFilaInteira'. */
    decide_removeFilaInteira->setName("RemoveFilaInteira");
    decide_removeFilaInteira->getConditions()->insert("NQ(FilaAck) > 0");
    components->insert(decide_removeFilaInteira);
    
    
    /* Remove 'RemoveDaFila'. */
    remove_removeDaFila->setName("RemoveDaFila");
    remove_removeDaFila->setQueueName("FilaAck");
    remove_removeDaFila->setRank("1");
    components->insert(remove_removeDaFila);
    
    
    /* 
     * Record 'ContaReenviados'. 
     * 
     * @todo Define correct parameters.
     * @todo Define as a counter.
     */
    record_contaReenviados->setName("ContaReenviados");
    record_contaReenviados->setExpressionName("PacotesReenviados");
    record_contaReenviados->setExpression("PacotesReenviados + 1");
    record_contaReenviados->setFilename("./temp/PacotesReenviados.txt");
    components->insert(record_contaReenviados);
    
    
    /* Route 'ReenviaPacote'. */
    route_reenviaPacote->setName("ReenviaPacote");
    route_reenviaPacote->setRouteTimeExpression("0");
    route_reenviaPacote->setRouteTimeTimeUnit(Util::TimeUnit::second);
    route_reenviaPacote->setRouteDestinationType(Route::DestinationType::Station);
    route_reenviaPacote->setStation(station_envioTransmissor);
    components->insert(route_reenviaPacote);
    
    
    /* Connect the components that control the ACK timeout. */
    enter_countTimeout->getNextComponents()->insert(leave_countTimeout);
    leave_countTimeout->getNextComponents()->insert(delay_timeoutAck);
    delay_timeoutAck->getNextComponents()->insert(decide_verificaFilaAckTimeout);
    decide_verificaFilaAckTimeout->getNextComponents()->insert(decide_verificaTimeout);
    decide_verificaFilaAckTimeout->getNextComponents()->insert(dispose_descartaTimeout);
    decide_verificaTimeout->getNextComponents()->insert(decide_removeFilaInteira);
    decide_verificaTimeout->getNextComponents()->insert(dispose_descartaTimeout);
    decide_removeFilaInteira->getNextComponents()->insert(remove_removeDaFila);
    decide_removeFilaInteira->getNextComponents()->insert(dispose_descartaTimeout);
    remove_removeDaFila->getNextComponents()->insert(decide_removeFilaInteira);
    remove_removeDaFila->getNextComponents()->insert(record_contaReenviados);
    record_contaReenviados->getNextComponents()->insert(route_reenviaPacote);
    
    
    /*
     * Secao Canal de transmissao.
     * 
     * Transmissor > Receptor
     */
    /* Station 'OutTransmissor'. */
    elements->insert(Util::TypeOf<Station>(), station_outTransmissor);
    station_outTransmissor->setEnterIntoStationComponent(enter_outTransmissor);
    
    enter_outTransmissor->setName("EnterOutTransmissor");
    enter_outTransmissor->setStation(station_outTransmissor);
    components->insert(enter_outTransmissor);
    
    leave_outTransmissor->setName("LeaveOutTransmissor");
    leave_outTransmissor->setStation(station_outTransmissor);
    components->insert(leave_outTransmissor);
    
    
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
    decide_decideErroPacote->getConditions()->insert("UNIF(0,1) <= (100 - TaxaErro)");
    components->insert(decide_decideErroPacote);
    
    
    /* Route 'GoToInReceptor'. */
    route_goToInReceptor->setName("GoToInReceptor");
    route_goToInReceptor->setRouteTimeExpression("0");
    route_goToInReceptor->setRouteTimeTimeUnit(Util::TimeUnit::second);
    route_goToInReceptor->setRouteDestinationType(Route::DestinationType::Station);
    route_goToInReceptor->setStation(station_inReceptor);
    components->insert(route_goToInReceptor);
    
    
    /* Dispose 'ErroPacote'. */
    dispose_erroPacote->setName("ErroPacote");
    components->insert(dispose_erroPacote);
    
    
    /* Connect the components of 'Canal de Transmissao - Transmissor > Receptor'*/
    enter_outTransmissor->getNextComponents()->insert(leave_outTransmissor);
    leave_outTransmissor->getNextComponents()->insert(seize_alocaRotaPacote);
    seize_alocaRotaPacote->getNextComponents()->insert(delay_latenciaCanal);
    delay_latenciaCanal->getNextComponents()->insert(release_livraRotaPacote);
    release_livraRotaPacote->getNextComponents()->insert(decide_decideErroPacote);
    decide_decideErroPacote->getNextComponents()->insert(route_goToInReceptor);
    decide_decideErroPacote->getNextComponents()->insert(dispose_erroPacote);
    
    
    /*
     * Secao Canal de transmissao.
     * 
     * Receptor > Transmissor
     */
    /* Station 'OutReceptor'. */
    elements->insert(Util::TypeOf<Station>(), station_outReceptor);
    
    enter_outReceptor->setName("EnterOutReceptor");
    enter_outReceptor->setStation(station_outReceptor);
    components->insert(enter_outReceptor);
    
    leave_outReceptor->setName("LeaveOutReceptor");
    leave_outReceptor->setStation(station_outReceptor);
    components->insert(leave_outReceptor);
    
    
    /* Seize 'AlocaRotaAck'. */
    Resource* resource_rotaAck = new Resource(elements, "RotaAck");
    resource_rotaAck->setCapacity(1);
    elements->insert(Util::TypeOf<Resource>(), resource_rotaAck);
    
    Queue* queue_filaAlocaRotaAck = new Queue(elements, "FilaAlocaRotaAck");
    queue_filaAlocaRotaAck->setOrderRule(Queue::OrderRule::FIFO);
    elements->insert(Util::TypeOf<Queue>(), queue_filaAlocaRotaAck);
    
    seize_alocaRotaAck->setName("AlocaRotaAck");
    seize_alocaRotaAck->setResourceType(Resource::ResourceType::RESOURCE);
    seize_alocaRotaAck->setResource(resource_rotaAck);
    /* @todo See if it's necessary to define allocationType (where the stat is saved). */
    seize_alocaRotaAck->setPriority(2);
    seize_alocaRotaAck->setQueue(queue_filaAlocaRotaAck);
    seize_alocaRotaAck->setQuantity("1.0");
    components->insert(seize_alocaRotaAck);
    
    
    /* Delay 'LatenciaCanalAck'. */
    delay_latenciaCanalAck->setName("LatenciaCanalAck");
    /* @todo Ver necessidade de especificar allocation. */
    delay_latenciaCanalAck->setDelayExpression("DelayCanal");
    delay_latenciaCanalAck->setDelayTimeUnit(Util::TimeUnit::second);
    components->insert(delay_latenciaCanalAck);
    
    
    /* Release 'LivraRotaAck'. */
    release_livraRotaAck->setName("LivraRotaAck");
    release_livraRotaAck->setResourceType(Resource::ResourceType::RESOURCE);
    release_livraRotaAck->setResource(resource_rotaAck);
    release_livraRotaAck->setQuantity("1");
    components->insert(release_livraRotaAck);
    
    
    /* Decide 'DecideErroAck'. */
    decide_decideErroAck->setName("DecideErroAck");
    decide_decideErroAck->getConditions()->insert("UNIF(0,1) <= (100 - TaxaErro)");
    components->insert(decide_decideErroAck);
    
    
    /* Route 'GoToInTransmissor'. */
    route_goToInTransmissor->setName("GoToInTransmissor");
    route_goToInTransmissor->setRouteTimeExpression("0");
    route_goToInTransmissor->setRouteTimeTimeUnit(Util::TimeUnit::second);
    route_goToInTransmissor->setRouteDestinationType(Route::DestinationType::Station);
    route_goToInTransmissor->setStation(station_inTransmissor);
    components->insert(route_goToInTransmissor);
    
    
    /* Dispose 'ErroAck'. */
    dispose_erroAck->setName("ErroAck");
    components->insert(dispose_erroAck);
    
    
    /* Connect the components of 'Canal de Transmissao - Receptor > Transmissor'*/
    enter_outReceptor->getNextComponents()->insert(leave_outReceptor);
    leave_outReceptor->getNextComponents()->insert(seize_alocaRotaAck);
    seize_alocaRotaAck->getNextComponents()->insert(delay_latenciaCanalAck);
    delay_latenciaCanalAck->getNextComponents()->insert(release_livraRotaAck);
    release_livraRotaAck->getNextComponents()->insert(decide_decideErroAck);
    decide_decideErroAck->getNextComponents()->insert(route_goToInTransmissor);
    decide_decideErroAck->getNextComponents()->insert(dispose_erroAck);
    
    
    /*
     * Secao Receptor.
     */
    /* Station 'InReceptor'. */
    elements->insert(Util::TypeOf<Station>(), station_inReceptor);
    station_inReceptor->setEnterIntoStationComponent(enter_inReceptor);
    
    enter_inReceptor->setName("EnterInReceptor");
    enter_inReceptor->setStation(station_inReceptor);
    components->insert(enter_inReceptor);
    
    leave_inReceptor->setName("LeaveInReceptor");
    leave_inReceptor->setStation(station_inReceptor);
    components->insert(leave_inReceptor);
    
    
    /* Assign 'ConverteAck'. */
    assign_converteAck->setName("ConverteAck");
    assign_atribuiNumero->getAssignments()->insert(new Assign::Assignment(Assign::DestinationType::Attribute, "TamanhoPacote", "128"));
    components->insert(assign_converteAck);
    
    
    /* Assign 'AtribuiDelaysAck'. */
    assign_atribuiDelaysAck->setName("AtribuiDelaysAck");
    assign_atribuiDelaysAck->getAssignments()->insert(new Assign::Assignment(Assign::DestinationType::Attribute, "DelayCanal", "UNIF(1,50)"));
    assign_atribuiDelaysAck->getAssignments()->insert(new Assign::Assignment(Assign::DestinationType::Attribute, "DelayTransmissao", "TamanhoPacote * 8 / TaxaDeTransmissao"));
    components->insert(assign_atribuiDelaysAck);
    
    
    /* Route 'GoToEnvioReceptor'. */
    route_goToEnvioReceptor->setName("GoToEnvioReceptor");
    route_goToEnvioReceptor->setRouteTimeExpression("0");
    route_goToEnvioReceptor->setRouteTimeTimeUnit(Util::TimeUnit::second);
    route_goToEnvioReceptor->setRouteDestinationType(Route::DestinationType::Station);
    route_goToEnvioReceptor->setStation(station_envioReceptor);
    components->insert(route_goToEnvioReceptor);
    
    
    /* Connect the components of 'Receptor'. */
    enter_inReceptor->getNextComponents()->insert(leave_inReceptor);
    leave_inReceptor->getNextComponents()->insert(assign_converteAck);
    assign_converteAck->getNextComponents()->insert(assign_atribuiDelaysAck);
    assign_atribuiDelaysAck->getNextComponents()->insert(route_goToEnvioReceptor);
    
    
    /*
     * Secao Envio receptor.
     */
    /* Station 'EnvioReceptor'. */
    elements->insert(Util::TypeOf<Station>(), station_envioReceptor);
    station_envioReceptor->setEnterIntoStationComponent(enter_envioReceptor);
    
    enter_envioReceptor->setName("EnterEnvioReceptor");
    enter_envioReceptor->setStation(station_envioReceptor);
    components->insert(enter_envioReceptor);
    
    leave_envioReceptor->setName("LeaveEnvioReceptor");
    leave_envioReceptor->setStation(station_envioReceptor);
    components->insert(leave_envioReceptor);
    
    
    /* Seize 'AlocaCanalAck'. */
    Resource* resource_canalDeTransmissaoAck = new Resource(elements, "CanalDeTransmissaoAck");
    resource_canalDeTransmissaoAck->setCapacity(1);
    elements->insert(Util::TypeOf<Resource>(), resource_canalDeTransmissaoAck);
    
    Queue* queue_filaTransmissaoAck = new Queue(elements, "FilaTransmissaoAck");
    queue_filaTransmissaoAck->setOrderRule(Queue::OrderRule::FIFO);
    elements->insert(Util::TypeOf<Queue>(), queue_filaTransmissaoAck);
    
    seize_alocaCanalAck->setName("AlocaCanalAck");
    seize_alocaCanalAck->setResourceType(Resource::ResourceType::RESOURCE);
    seize_alocaCanalAck->setResource(resource_canalDeTransmissaoAck);
    /* @todo See if it's necessary to define allocationType (where the stat is saved). */
    seize_alocaCanalAck->setPriority(2);
    seize_alocaCanalAck->setQueue(queue_filaTransmissaoAck);
    seize_alocaCanalAck->setQuantity("1.0");
    components->insert(seize_alocaCanalAck);
    
    
    /* Delay 'AtrasoDeTransmissaoAck'. */
    delay_atrasoDeTransmissaoAck->setName("AtrasoDeTransmissaoAck");
    /* @todo Ver necessidade de especificar allocation. */
    delay_atrasoDeTransmissaoAck->setDelayExpression("DelayTransmissao");
    delay_atrasoDeTransmissaoAck->setDelayTimeUnit(Util::TimeUnit::second);
    components->insert(delay_atrasoDeTransmissaoAck);
    
    
    /* Release 'LivraCanalAck'. */
    release_livraCanalAck->setName("LivraCanalAck");
    release_livraCanalAck->setResourceType(Resource::ResourceType::RESOURCE);
    release_livraCanalAck->setResource(resource_canalDeTransmissaoAck);
    release_livraCanalAck->setQuantity("1");
    components->insert(release_livraCanalAck);
    
    
    /* Route 'GoToOutReceptor'. */
    route_goToOutReceptor->setName("GoToOutReceptor");
    route_goToOutReceptor->setRouteTimeExpression("0");
    route_goToOutReceptor->setRouteTimeTimeUnit(Util::TimeUnit::second);
    route_goToOutReceptor->setRouteDestinationType(Route::DestinationType::Station);
    route_goToOutReceptor->setStation(station_outReceptor);
    components->insert(route_goToOutReceptor);
    
    
    /* Connect the components of 'Receptor'. */
    enter_envioReceptor->getNextComponents()->insert(leave_envioReceptor);
    leave_envioReceptor->getNextComponents()->insert(seize_alocaCanalAck);
    seize_alocaCanalAck->getNextComponents()->insert(delay_atrasoDeTransmissaoAck);
    delay_atrasoDeTransmissaoAck->getNextComponents()->insert(release_livraCanalAck);
    release_livraCanalAck->getNextComponents()->insert(route_goToOutReceptor);
    
    /* End of model construction. */
    
    
    // insert the model into the simulator 
    simulator->getModelManager()->insert(model);

    // if the model is ok then save the model into a text file 
    if (model->checkModel())
	model->saveModel("./temp/slidingWindowModel.txt");

    // execute the simulation until completed and show the report
    model->getSimulation()->startSimulation();
    
    return 0;
};

