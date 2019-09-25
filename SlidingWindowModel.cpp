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
#include "Assign.h"
#include "Create.h"
#include "Decide.h"
#include "Delay.h"
#include "Dispose.h"
#include "Enter.h"
#include "Hold.h"
#include "Leave.h"
#include "Record.h"
#include "Route.h"
#include "Seize.h"
#include "Separate.h"
#include "Signal.h"
#include "Station.h"


// Model elements
#include "EntityType.h"
#include "Queue.h"
#include "Resource.h"


SlidingWindowModel::SlidingWindowModel() {
}

/**
 * This is the main function of the application. 
 * It instanciates the simulator, builds a simulation model and then simulate that model.
 *
 * @todo TODO: Specify the nomenclature of the original model macros.
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
    
    /* Define a new EntityType Pacote. */
    EntityType* pacote = new EntityType(elements, "Pacote");
    elements->insert(Util::TypeOf<EntityType>(), pacote);
    
    /*
     * @brief Defines all the existing components on the model.
     */
    /* Secao 'Cria N pacotes'. */
    Create* create_pacotes;
    Route* route_goToNovoPacote;
    
    /* Secao 'Transmissor'. */
    Enter* enter_novoPacote;
    Station* station_novoPacote;
    Leave* leave_novoPacote;
    Assign* assign_atribuiNumero;
    Assign* assign_atribuiTamanho;
    Assign* assign_atribuiDelays;
    Seize* seize_entraNaJanela;
    Route* route_goToEnvioTransmissor;
    
    /* Secao 'Envio transmissor'. */
    Enter* enter_envioTransmissor;
    Station* station_envioTransmissor;
    Leave* leave_envioTransmissor;
    Separate* separate_criaEsperaAck;
    Enter* enter_goToAguardeAck;
    Station* station_goToAguardeAck;
    Leave* leave_goToAguardeAck;
    Seize* seize_alocaCanal;
    Delay* delay_atrasoDeTransmissao;
    Release* release_livraCanal;  /* @todo TODO: Check if it is a release. */
    Separate* separate_criaTimeout;
    Route* route_goToCountTimeout;
    Route* route_goToOutTransmissor;
    
    /* Secao 'Canal de transmissão'. */
    // Transmissor > Receptor
    Enter* enter_outTransmissor;
    Station* station_outTransmissor;
    Leave* leave_outTransmissor;
    Seize* seize_alocaRotaPacote;
    Delay* delay_latenciaCanal;
    Release* release_livraRotaPacote;
    Decide* decide_decideErroPacote;
    Route* route_goToInReceptor;
    Dispose* dispose_erroPacote;
    
    // Receptor > Transmissor
    Enter* enter_outReceptor;
    Station* station_outReceptor;
    Leave* leave_outReceptor;
    Seize* seize_alocaRotaAck;
    Delay* delay_latenciaCanalAck;
    Release* release_livraRotaAck;
    Decide* decide_decideErroAck;
    Route* route_goToInTransmissor;
    Dispose* dispose_erroAck;
    
    
    /* Secao 'Receptor'. */
    Enter* enter_inReceptor;
    Station* station_inReceptor;
    Leave* leave_inReceptor;
    Assign* assign_converteAck;
    Assign* assign_atribuiDelaysAck;
    Route* route_goToEnvioReceptor;
    
    
    /* Secao 'Envio Receptor'. */
    Enter* enter_envioReceptor;
    Station* station_envioReceptor;
    Leave* leave_envioReceptor;
    Seize* seize_alocaCanalAck;
    Delay* delay_atrasoDeTransmissaoAck;
    Release* release_livraCanalAck;
    Route* route_goToOutReceptor;
    
    
    /* Secao 'Aguarda ACK'. */
    Enter* enter_aguardeAck;
    Station* station_aguardeAck;
    Leave* leave_aguardeAck;
    Hold* hold_holdAck;
    Release* release_livraJanela;
    Record* record_contaEnviadosComSucesso;
    Route* route_reciclaPacotes;
    
    Enter* enter_inTransmissor;
    Station* station_inTransmissor;
    Leave* leave_inTransmissor;
    Decide* decide_verificaFilaAck;
    Dispose* dispose_ignoraAck;
    Decide* decide_verificaAck;
    Signal* signal_retiraDaFilaAck;
    Dispose* dispose_descartaAck;
    
    Enter* enter_countTimeout;
    Station* station_countTimeout;
    Leave* leave_countTimeout;
    Delay* delay_timeoutAck;
    Decide* decide_verificaFilaAckTimeout;
    Dispose* dispose_descartaTimeout;
    Decide* decide_verificaTimeout;
    Decide* decide_removeFilaInteira;
    Remove* remove_removeDaFila;
    Record* record_contaReenviados;
    Route* route_reenviaPacote;
    
    
    /*
     * Secao Cria N pacotes.
     *
     * @todo TODO: Define correct parameters for components.
     */
    /* Create 'Pacotes'. */
    create_pacotes = new Create(model);
    create_pacotes->setName("Pacotes");
    create_pacotes->setEntityType(pacote);
    create_pacotes->setTimeBetweenCreationsExpression("1");
    components->insert(create_pacotes);
    
    
    /* Route 'GoToNovoPacote'. */
    route_goToNovoPacote = new Route(model);
    route_goToNovoPacote->setName("GoToNovoPacote");
    route_goToNovoPacote->setStation(station_novoPacote);
    route_goToNovoPacote->setRouteTimeExpression("1");
    components->insert(route_goToNovoPacote);
    
    
    /* Connect the components from 'Cria N pacotes'. */
    create_pacotes->getNextComponents()->insert(route_goToNovoPacote);
    
    
    /*
     * Secao Transmissor.
     *
     * @todo TODO: Define correct parameters for components.
     */
    /* Station 'NovoPacote'. */
    station_novoPacote = new Station(elements, "NovoPacote");
    elements->insert(Util::TypeOf<Station>(), station_novoPacote);
    
    enter_novoPacote = new Enter(model);
    enter_novoPacote->setName("EnterNovoPacote");
    enter_novoPacote->setStation(station_novoPacote);
    components->insert(enter_novoPacote);
    
    leave_novoPacote = new Leave(model);
    leave_novoPacote->setName("LeaveNovoPacote");
    leave_novoPacote->setStation(station_novoPacote);
    components->insert(leave_novoPacote);
    
    
    /* Assign 'AtribuiNumero'. */
    assign_atribuiNumero = new Assign(model);
    assign_atribuiNumero->setName("AtribuiNumero");
    components->insert(assign_atribuiNumero);
    
    
    /* Assign 'AtribuiTamanho'. */
    assign_atribuiTamanho = new Assign(model);
    assign_atribuiTamanho->setName("AtribuiTamanho");
    components->insert(assign_atribuiTamanho);
    
    
    /* Assign 'AtribuiDelays'. */
    assign_atribuiDelays = new Assign(model);
    assign_atribuiDelays->setName("AtribuiDelays");
    components->insert(assign_atribuiDelays);
    
    
    /* Seize 'EntraNaJanela'. */
    Resource* resource_janelaDeslizante = new Resource(elements, "JanelaDeslizante");
    resource_janelaDeslizante->setCapacity("1");
    elements->insert(Util::TypeOf<Resource>(), resource_janelaDeslizante);
    
    Queue* queue_filaJanela = new Queue(elements, "FilaJanela");
    queue_filaJanela->setOrderRule(Queue::OrderRule::FIFO);
    elements->insert(Util::TypeOf<Queue>(), queue_filaJanela);
    
    seize_entraNaJanela = new Seize(model);
    seize_entraNaJanela->setName("EntraNaJanela");
    seize_entraNaJanela->setResource(resource_janelaDeslizante);
    seize_entraNaJanela->setQueue(queue_filaJanela);
    components->insert(seize_entraNaJanela);
    
    
    /* Route 'GoToEnvioTransmissor'. */
    route_goToEnvioTransmissor = new Route(model);
    route_goToEnvioTransmissor->setName("GoToEnvioTransmissor");
    route_goToEnvioTransmissor->setStation(station_envioTransmissor);
    route_goToEnvioTransmissor->setRouteTimeExpression("1");
    components->insert(route_goToEnvioTransmissor);
    
    
    /* Connect the components from 'Transmissor'. */
    leave_novoPacote->getNextComponents()->insert(assign_atribuiNumero);
    assign_atribuiNumero->getNextComponents()->insert(assign_atribuiTamanho);
    assign_atribuiTamanho->getNextComponents()->insert(assign_atribuiDelays);
    assign_atribuiDelays->getNextComponents()->insert(seize_entraNaJanela);
    seize_entraNaJanela->getNextComponents()->insert(route_goToEnvioTransmissor);
    
    
    /*
     * Secao Envio transmissor.
     */
    /* Station 'EnvioTransmissor'. */
    station_envioTransmissor = new Station(elements, "EnvioTransmissor");
    elements->insert(Util::TypeOf<Station>(), station_envioTransmissor);
    
    enter_envioTransmissor = new Enter(model);
    enter_envioTransmissor->setName("EnterEnvioTransmissor");
    enter_envioTransmissor->setStation(station_envioTransmissor);
    components->insert(enter_envioTransmissor);
    
    leave_envioTransmissor = new Leave(model);
    leave_envioTransmissor->setName("LeaveEnvioTransmissor");
    leave_envioTransmissor->setStation(station_envioTransmissor);
    components->insert(leave_envioTransmissor);
    
    
    /*
     *  Separate 'CriaEsperaAck'.
     * 
     * @todo TODO: See 'Separate' implementation. 
     */
    separate_criaEsperaAck = new Separate(model);
    separate_criaEsperaAck->setName("CriaEsperaAck");
    components->insert(separate_criaEsperaAck);
    
    
    /* Station 'GoToAguardeAck'. */
    station_goToAguardeAck = new Station(elements, "GoToAguardeAck");
    elements->insert(Util::TypeOf<Station>(), station_goToAguardeAck);
    
    enter_goToAguardeAck = new Enter(model);
    enter_goToAguardeAck->setName("EnterGoToAguardeAck");
    enter_goToAguardeAck->setStation(station_goToAguardeAck);
    components->insert(enter_goToAguardeAck);
    
    leave_goToAguardeAck = new Leave(model);
    leave_goToAguardeAck->setName("LeaveGoToAguardeAck");
    leave_goToAguardeAck->setStation(station_goToAguardeAck);
    components->insert(leave_goToAguardeAck);
    
    
    /* Seize 'AlocaCanal'. */
    Resource* resource_canalDeTransmissaoPacote = new Resource(elements, "CanalDeTransmissaoPacote");
    resource_canalDeTransmissaoPacote->setCapacity("1");
    elements->insert(Util::TypeOf<Resource>(), resource_canalDeTransmissaoPacote);
    
    Queue* queue_filaCanal = new Queue(elements, "FilaCanal");
    queue_filaCanal->setOrderRule(Queue::OrderRule::FIFO);
    elements->insert(Util::TypeOf<Queue>(), queue_filaCanal);
    
    seize_alocaCanal = new Seize(model);
    seize_alocaCanal->setName("AlocaCanal");
    seize_alocaCanal->setResource(resource_canalDeTransmissaoPacote);
    seize_alocaCanal->setQueue(queue_filaCanal);
    components->insert(seize_alocaCanal);
    
    
    /* Delay 'AtrasoDeTransmissao'. */
    delay_atrasoDeTransmissao = new Delay(model);
    delay_atrasoDeTransmissao->setName("AtrasoDeTransmissao");
    delay_atrasoDeTransmissao->setDelayExpression("2");
    delay_atrasoDeTransmissao->setDelayTimeUnit(Util::TimeUnit::second);
    components->insert(delay_atrasoDeTransmissao);
    
    
    /* Release 'LivraCanal'. */
    release_livraCanal = new Release(model);
    release_livraCanal->setName("LivraCanal");
    release_livraCanal->setResource(resource_canalDeTransmissaoPacote);
    components->insert(release_livraCanal);
    
    
    /* 
     * Separate 'CriaTimeout'.
     * 
     * @todo TODO: See 'Separate' implementation. 
     */ 
    separate_criaTimeout = new Separate(model);
    separate_criaTimeout->setName("CriaTimeout");
    components->insert(separate_criaTimeout);
    
    
    /* Route 'GoToCountTimeout'. */
    route_goToCountTimeout = new Route(model);
    route_goToCountTimeout->setName("GoToCountTimeout");
    route_goToCountTimeout->setStation(station_countTimeout);
    route_goToCountTimeout->setRouteTimeExpression("1");
    components->insert(route_goToCountTimeout);
    
    
    /* Route 'GoToOutTransmissor'. */
    route_goToOutTransmissor = new Route(model);
    route_goToOutTransmissor->setName("GoToOutTransmissor");
    route_goToOutTransmissor->setStation(station_countTimeout);
    route_goToOutTransmissor->setRouteTimeExpression("1");
    components->insert(route_goToOutTransmissor);
    
    
    /* Connect the components from 'Envio transmissor'. */
    leave_envioTransmissor->getNextComponents()->insert(separate_criaEsperaAck);
    separate_criaEsperaAck->getNextComponents()->insert(seize_alocaCanal);
    /* @todo TODO: Insert second out from separate_criaEsperaAck. */
    seize_alocaCanal->getNextComponents()->insert(delay_atrasoDeTransmissao);
    delay_atrasoDeTransmissao->getNextComponents()->insert(release_livraCanal);
    release_livraCanal->getNextComponents()->insert(separate_criaTimeout);
    separate_criaTimeout->getNextComponents()->insert(route_goToOutTransmissor);
    /* @todo TODO: Insert second out from separate_criaTimeout. */
    
    
    /*
     * Secao Aguarda ACK.
     */
    /* Station 'AguardeAck'. */
    station_aguardeAck = new Station(elements, "AguardeAck");
    elements->insert(Util::TypeOf<Station>(), station_aguardeAck);
    
    enter_aguardeAck = new Enter(model);
    enter_aguardeAck->setName("EnterAguardeAck");
    enter_aguardeAck->setStation(station_aguardeAck);
    components->insert(enter_aguardeAck);
    
    leave_aguardeAck = new Leave(model);
    leave_aguardeAck->setName("LeaveAguardeAck");
    leave_aguardeAck->setStation(station_aguardeAck);
    components->insert(leave_aguardeAck);
    
    
    /* Hold 'HoldAck'. 
     *
     * @todo TODO: See Hold implementation
     */
    Queue* queue_filaAck = new Queue(elements, "FilaAck");
    queue_filaAck->setOrderRule(Queue::OrderRule::FIFO);
    elements->insert(Util::TypeOf<Queue>(), queue_filaAck);
    
    hold_holdAck = new Hold(model);
    hold_holdAck->setName("HoldAck");
    components->insert(hold_holdAck);
    
    
    /* Release 'LivraJanela'. */
    release_livraJanela = new Release(model);
    release_livraJanela->setName("LivraJanela");
    release_livraJanela->setResource(resource_janelaDeslizante);
    components->insert(release_livraJanela);
    
    
    /* Record 'ContaEnviadosComSucesso'. */
    record_contaEnviadosComSucesso = new Record(model);
    record_contaEnviadosComSucesso->setName("ContaEnviadosComSucesso");
    components->insert(record_contaEnviadosComSucesso);
    
    
    /* Route 'ReciclaPacotes'. */
    route_reciclaPacotes = new Route(model);
    route_reciclaPacotes->setName("ReciclaPacotes");
    route_reciclaPacotes->setStation(station_novoPacote);
    route_reciclaPacotes->setRouteTimeExpression("1");
    components->insert(route_reciclaPacotes);
    
    /* Connect the components that control the ACK waiting. */
    leave_aguardeAck->getNextComponents()->insert(hold_holdAck);
    hold_holdAck->getNextComponents()->insert(release_livraJanela);
    release_livraJanela->getNextComponents()->insert(record_contaEnviadosComSucesso);
    record_contaEnviadosComSucesso->getNextComponents()->insert(route_reciclaPacotes);
    
    
    /* Station 'InTransmissor'. */
    station_inTransmissor = new Station(elements, "InTransmissor");
    elements->insert(Util::TypeOf<Station>(), station_inTransmissor);
    
    enter_inTransmissor = new Enter(model);
    enter_inTransmissor->setName("EnterInTransmissor");
    enter_inTransmissor->setStation(station_inTransmissor);
    components->insert(enter_inTransmissor);
    
    leave_inTransmissor = new Leave(model);
    leave_inTransmissor->setName("LeaveInTransmissor");
    leave_inTransmissor->setStation(station_inTransmissor);
    components->insert(leave_inTransmissor);
    
    
    /* Decide 'VerificaFilaAck'. */
    decide_verificaFilaAck = new Decide(model);
    decide_verificaFilaAck->setName("VerificaFilaAck");
    decide_verificaFilaAck->getConditions()->insert("NQ(Queue_Machine_1) <= 2*NQ(Queue_Machine_2)");
    components->insert(decide_verificaFilaAck);
    
    
    /* Dispose 'IgnoraAck'. */
    dispose_ignoraAck = new Dispose(model);
    dispose_ignoraAck->setName("IgnoraAck");
    components->insert(dispose_ignoraAck);
    
    
    /* Decide 'VerificaAck'. */
    decide_verificaAck = new Decide(model);
    decide_verificaAck->setName("VerificaAck");
    decide_verificaAck->getConditions()->insert("NQ(Queue_Machine_1) <= 2*NQ(Queue_Machine_2)");
    components->insert(decide_verificaAck);
    
    
    /* Signal 'RetiraDaFilaAck'. */
    signal_retiraDaFilaAck = new Signal(model);
    signal_retiraDaFilaAck->setName("RetiraDaFilaAck");
    components->insert(signal_retiraDaFilaAck);
    
    
    /* Dispose 'DescartaAck'. */
    dispose_descartaAck = new Dispose(model);
    dispose_descartaAck->setName("DescartaAck");
    components->insert(dispose_descartaAck);
    
    
    /* Connect the components that control the ACK received check. */
    leave_inTransmissor->getNextComponents()->insert(decide_verificaFilaAck);
    decide_verificaFilaAck->getNextComponents()->insert(decide_verificaAck);
    decide_verificaFilaAck->getNextComponents()->insert(dispose_ignoraAck);
    decide_verificaAck->getNextComponents()->insert(signal_retiraDaFilaAck);
    decide_verificaAck->getNextComponents()->insert(dispose_ignoraAck);
    signal_retiraDaFilaAck->getNextComponents()->insert(dispose_descartaAck);
    
    
    /* Station 'CountTimeout'. */
    station_countTimeout = new Station(elements, "CountTimeout");
    elements->insert(Util::TypeOf<Station>(), station_countTimeout);
    
    enter_countTimeout = new Enter(model);
    enter_countTimeout->setName("EnterCountTimeout");
    enter_countTimeout->setStation(station_countTimeout);
    components->insert(enter_countTimeout);
    
    leave_countTimeout = new Leave(model);
    leave_countTimeout->setName("LeaveCountTimeout");
    leave_countTimeout->setStation(station_countTimeout);
    components->insert(leave_countTimeout);
    
    
    /* Delay 'TimeoutAck'. */
    delay_timeoutAck = new Delay(model);
    delay_timeoutAck->setName("TimeoutAck");
    delay_timeoutAck->setDelayExpression("2");
    delay_timeoutAck->setDelayTimeUnit(Util::TimeUnit::second);
    components->insert(delay_timeoutAck);
    
    
    /* Decide 'VerificaFilaAckTimeout'. */
    decide_verificaFilaAckTimeout = new Decide(model);
    decide_verificaFilaAckTimeout->setName("VerificaFilaAckTimeout");
    decide_verificaFilaAckTimeout->getConditions()->insert("NQ(Queue_Machine_1) <= 2*NQ(Queue_Machine_2)");
    components->insert(decide_verificaFilaAckTimeout);
    
    
    /* Dispose 'DescartaTimeout'. */
    dispose_descartaTimeout = new Dispose(model);
    dispose_descartaTimeout->setName("DescartaTimeout");
    components->insert(dispose_descartaTimeout);
    
    
    /* Decide 'VerificaTimeout'. */
    decide_verificaTimeout = new Decide(model);
    decide_verificaTimeout->setName("VerificaTimeout");
    decide_verificaTimeout->getConditions()->insert("NQ(Queue_Machine_1) <= 2*NQ(Queue_Machine_2)");
    components->insert(decide_verificaTimeout);
    
    
    /* Decide 'RemoveFilaInteira'. */
    decide_removeFilaInteira = new Decide(model);
    decide_removeFilaInteira->setName("RemoveFilaInteira");
    decide_removeFilaInteira->getConditions()->insert("NQ(Queue_Machine_1) <= 2*NQ(Queue_Machine_2)");
    components->insert(decide_removeFilaInteira);
    
    
    /* Remove 'RemoveDaFila'.
     * 
     * @todo TODO: See 'Remove' implementation.
     */
    remove_removeDaFila = new Remove(model);
    remove_removeDaFila->setName("RemoveDaFila");
    components->insert(remove_removeDaFila);
    
    
    /* Record 'ContaReenviados'. */
    record_contaReenviados = new Record(model);
    record_contaReenviados->setName("ContaReenviados");
    components->insert(record_contaReenviados);
    
    
    /* Route 'ReenviaPacote'. */
    route_reenviaPacote = new Route(model);
    route_reenviaPacote->setName("ReenviaPacote");
    route_reenviaPacote->setStation(station_envioTransmissor);
    route_reenviaPacote->setRouteTimeExpression("1");
    components->insert(route_reenviaPacote);
    
    
    /* Connect the components that control the ACK timeout. */
    leave_countTimeout->getNextComponents()->insert(delay_timeoutAck);
    delay_timeoutAck->getNextComponents()->insert(decide_verificaFilaAckTimeout);
    decide_verificaFilaAckTimeout->getNextComponents()->insert(decide_verificaTimeout);
    decide_verificaFilaAckTimeout->getNextComponents()->insert(dispose_descartaTimeout);
    decide_verificaTimeout->getNextComponents()->insert(decide_removeFilaInteira);
    decide_verificaTimeout->getNextComponents()->insert(dispose_descartaTimeout);
    decide_removeFilaInteira->getNextComponents()->insert(remove_removeDaFila);
    decide_removeFilaInteira->getNextComponents()->insert(dispose_descartaTimeout);
    remove_removeDaFila->getNextComponents()->insert(decide_removeFilaInteira);
    /* removed entity from remove go to contaReenviados. */
    record_contaReenviados->getNextComponents()->insert(route_reenviaPacote);
    
    
    /*
     * Secao Canal de transmissao.
     * 
     * Transmissor > Receptor
     */
    /* Station 'OutTransmissor'. */
    station_outTransmissor = new Station(elements, "OutTransmissor");
    elements->insert(Util::TypeOf<Station>(), station_outTransmissor);
    
    enter_outTransmissor = new Enter(model);
    enter_outTransmissor->setName("EnterOutTransmissor");
    enter_outTransmissor->setStation(station_outTransmissor);
    components->insert(enter_outTransmissor);
    
    leave_outTransmissor = new Leave(model);
    leave_outTransmissor->setName("LeaveOutTransmissor");
    leave_outTransmissor->setStation(station_outTransmissor);
    components->insert(leave_outTransmissor);
    
    
    /* Seize 'AlocaRotaPacote'. */
    Resource* resource_rotaPacote = new Resource(elements, "RotaPacote");
    resource_rotaPacote->setCapacity("1");
    elements->insert(Util::TypeOf<Resource>(), resource_rotaPacote);
    
    Queue* queue_filaAlocaRotaPacote = new Queue(elements, "FilaAlocaRotaPacote");
    queue_filaAlocaRotaPacote->setOrderRule(Queue::OrderRule::FIFO);
    elements->insert(Util::TypeOf<Queue>(), queue_filaAlocaRotaPacote);
    
    seize_alocaRotaPacote = new Seize(model);
    seize_alocaRotaPacote->setName("AlocaRotaPacote");
    seize_alocaRotaPacote->setResource(resource_rotaPacote);
    seize_alocaRotaPacote->setQueue(queue_filaAlocaRotaPacote);
    components->insert(seize_alocaRotaPacote);
    
    
    /* Delay 'LatenciaCanal'. */
    delay_latenciaCanal = new Delay(model);
    delay_latenciaCanal->setName("LatenciaCanal");
    delay_latenciaCanal->setDelayExpression("2");
    delay_latenciaCanal->setDelayTimeUnit(Util::TimeUnit::second);
    components->insert(delay_latenciaCanal);
    
    
    /* Release 'LivraRotaPacote'. */
    release_livraRotaPacote = new Release(model);
    release_livraRotaPacote->setName("LivraRotaPacote");
    release_livraRotaPacote->setResource(resource_rotaPacote);
    components->insert(release_livraRotaPacote);
    
    
    /* Decide 'DecideErroPacote'. */
    decide_decideErroPacote = new Decide(model);
    decide_decideErroPacote->setName("DecideErroPacote");
    decide_decideErroPacote->getConditions()->insert("NQ(Queue_Machine_1) <= 2*NQ(Queue_Machine_2)");
    components->insert(decide_decideErroPacote);
    
    
    /* Route 'GoToInReceptor'. */
    route_goToInReceptor = new Route(model);
    route_goToInReceptor->setName("GoToInReceptor");
    route_goToInReceptor->setStation(station_inReceptor);
    route_goToInReceptor->setRouteTimeExpression("1");
    components->insert(route_goToInReceptor);
    
    
    /* Dispose 'ErroPacote'. */
    dispose_erroPacote = new Dispose(model);
    dispose_erroPacote->setName("ErroPacote");
    components->insert(dispose_erroPacote);
    
    
    /* Connect the components of 'Canal de Transmissao - Transmissor > Receptor'*/
    leave_OutTransmissor->getNextComponents()->insert(seize_alocaRotaPacote);
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
    station_outReceptor = new Station(elements, "OutReceptor");
    elements->insert(Util::TypeOf<Station>(), station_outReceptor);
    
    enter_outReceptor = new Enter(model);
    enter_outReceptor->setName("EnterOutReceptor");
    enter_outReceptor->setStation(station_outReceptor);
    components->insert(enter_outReceptor);
    
    leave_outReceptor = new Leave(model);
    leave_outReceptor->setName("LeaveOutReceptor");
    leave_outReceptor->setStation(station_outReceptor);
    components->insert(leave_outReceptor);
    
    
    /* Seize 'AlocaRotaAck'. */
    Resource* resource_rotaAck = new Resource(elements, "RotaAck");
    resource_rotaAck->setCapacity("1");
    elements->insert(Util::TypeOf<Resource>(), resource_rotaAck);
    
    Queue* queue_filaAlocaRotaAck = new Queue(elements, "FilaAlocaRotaAck");
    queue_filaAlocaRotaAck->setOrderRule(Queue::OrderRule::FIFO);
    elements->insert(Util::TypeOf<Queue>(), queue_filaAlocaRotaAck);
    
    seize_alocaRotaAck = new Seize(model);
    seize_alocaRotaAck->setName("AlocaRotaAck");
    seize_alocaRotaAck->setResource(resource_rotaAck);
    seize_alocaRotaAck->setQueue(queue_filaAlocaRotaAck);
    components->insert(seize_alocaRotaAck);
    
    
    /* Delay 'LatenciaCanalAck'. */
    delay_latenciaCanalAck = new Delay(model);
    delay_latenciaCanalAck->setName("LatenciaCanalAck");
    delay_latenciaCanalAck->setDelayExpression("2");
    delay_latenciaCanalAck->setDelayTimeUnit(Util::TimeUnit::second);
    components->insert(delay_latenciaCanalAck);
    
    
    /* Release 'LivraRotaAck'. */
    release_livraRotaAck = new Release(model);
    release_livraRotaAck->setName("LivraRotaAck");
    release_livraRotaAck->setResource(resource_rotaAck);
    components->insert(release_livraRotaAck);
    
    
    /* Decide 'DecideErroAck'. */
    decide_decideErroAck = new Decide(model);
    decide_decideErroAck->setName("DecideErroAck");
    decide_decideErroAck->getConditions()->insert("NQ(Queue_Machine_1) <= 2*NQ(Queue_Machine_2)");
    components->insert(decide_decideErroAck);
    
    
    /* Route 'GoToInTransmissor'. */
    route_goToInTransmissor = new Route(model);
    route_goToInTransmissor->setName("GoToInTransmissor");
    route_goToInTransmissor->setStation(station_inTransmissor);
    route_goToInTransmissor->setRouteTimeExpression("1");
    components->insert(route_goToInTransmissor);
    
    
    /* Dispose 'ErroAck'. */
    dispose_erroAck = new Dispose(model);
    dispose_erroAck->setName("ErroAck");
    components->insert(dispose_erroAck);
    
    
    /* Connect the components of 'Canal de Transmissao - Receptor > Transmissor'*/
    leave_OutReceptor->getNextComponents()->insert(seize_alocaRotaAck);
    seize_alocaRotaAck->getNextComponents()->insert(delay_latenciaCanalAck);
    delay_latenciaCanalAck->getNextComponents()->insert(release_livraRotaAck);
    release_livraRotaAck->getNextComponents()->insert(decide_decideErroAck);
    decide_decideErroAck->getNextComponents()->insert(route_goToInTransmissor);
    decide_decideErroAck->getNextComponents()->insert(dispose_erroAck);
    
    
    /*
     * Secao Receptor.
     */
    /* Station 'InReceptor'. */
    station_inReceptor = new Station(elements, "InReceptor");
    elements->insert(Util::TypeOf<Station>(), station_inReceptor);
    
    enter_inReceptor = new Enter(model);
    enter_inReceptor->setName("EnterInReceptor");
    enter_inReceptor->setStation(station_inReceptor);
    components->insert(enter_inReceptor);
    
    leave_inReceptor = new Leave(model);
    leave_inReceptor->setName("LeaveInReceptor");
    leave_inReceptor->setStation(station_inReceptor);
    components->insert(leave_inReceptor);
    
    
    /* Assign 'ConverteAck'. */
    assign_converteAck = new Assign(model);
    assign_converteAck->setName("ConverteAck");
    components->insert(assign_converteAck);
    
    
    /* Assign 'AtribuiDelaysAck'. */
    assign_atribuiDelaysAck = new Assign(model);
    assign_atribuiDelaysAck->setName("AtribuiDelaysAck");
    components->insert(assign_atribuiDelaysAck);
    
    
    /* Route 'GoToEnvioReceptor'. */
    route_goToEnvioReceptor = new Route(model);
    route_goToEnvioReceptor->setName("GoToEnvioReceptor");
    route_goToEnvioReceptor->setStation(station_envioReceptor);
    route_goToEnvioReceptor->setRouteTimeExpression("1");
    components->insert(route_goToEnvioReceptor);
    
    
    /* Connect the components of 'Receptor'. */
    leave_inReceptor->getNextComponents()->insert(assign_converteAck);
    assign_converteAck->getNextComponents()->insert(assign_atribuiDelaysAck);
    assign_atribuiDelaysAck->getNextComponents()->insert(route_goToEnvioReceptor);
    
    
    /*
     * Secao Envio receptor.
     */
    /* Station 'EnvioReceptor'. */
    station_envioReceptor = new Station(elements, "EnvioReceptor");
    elements->insert(Util::TypeOf<Station>(), station_envioReceptor);
    
    enter_envioReceptor = new Enter(model);
    enter_envioReceptor->setName("EnterEnvioReceptor");
    enter_envioReceptor->setStation(station_envioReceptor);
    components->insert(enter_envioReceptor);
    
    leave_envioReceptor = new Leave(model);
    leave_envioReceptor->setName("LeaveEnvioReceptor");
    leave_envioReceptor->setStation(station_envioReceptor);
    components->insert(leave_envioReceptor);
    
    
    /* Seize 'AlocaCanalAck'. */
    Resource* resource_canalDeTransmissaoAck = new Resource(elements, "CanalDeTransmissaoAck");
    resource_canalDeTransmissaoAck->setCapacity("1");
    elements->insert(Util::TypeOf<Resource>(), resource_canalDeTransmissaoAck);
    
    Queue* queue_filaTransmissaoAck = new Queue(elements, "FilaTransmissaoAck");
    queue_filaTransmissaoAck->setOrderRule(Queue::OrderRule::FIFO);
    elements->insert(Util::TypeOf<Queue>(), queue_filaTransmissaoAck);
    
    seize_alocaCanalAck = new Seize(model);
    seize_alocaCanalAck->setName("AlocaCanalAck");
    seize_alocaCanalAck->setResource(resource_canalDeTransmissaoAck);
    seize_alocaCanalAck->setQueue(queue_filaTransmissaoAck);
    components->insert(seize_alocaCanalAck);
    
    
    /* Delay 'AtrasoDeTransmissaoAck'. */
    delay_atrasoDeTransmissaoAck = new Delay(model);
    delay_atrasoDeTransmissaoAck->setName("AtrasoDeTransmissaoAck");
    delay_atrasoDeTransmissaoAck->setDelayExpression("2");
    delay_atrasoDeTransmissaoAck->setDelayUnit(Util::TimeUnit::second);
    components->insert(delay_atrasoDeTransmissaoAck);
    
    
    /* Release 'LivraCanalAck'. */
    release_livraCanalAck = new Release(model);
    release_livraCanalAck->setName("LivraCanalAck");
    release_livraCanalAck->setResource(resource_canalDeTransmissaoAck);
    components->insert(release_livraCanalAck);
    
    
    /* Route 'GoToOutReceptor'. */
    route_goToOutReceptor = new Route(model);
    route_goToOutReceptor->setName("GoToOutReceptor");
    route_goToOutReceptor->setStation(station_outReceptor);
    route_goToOutReceptor->setRouteTimeExpression("1");
    components->insert(route_goToOutReceptor);
    
    
    /* Connect the components of 'Receptor'. */
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

