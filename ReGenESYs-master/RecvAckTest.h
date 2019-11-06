/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   RecvAckTest.h
 * Author: joaofel-u
 *
 * Created on 5 de novembro de 2019, 19:08
 */

#ifndef RECVACKTEST_H
#define RECVACKTEST_H

#include "BaseConsoleGenesysApplication.h"
#include "Model.h"

class RecvAckTest: public BaseConsoleGenesysApplication {
public:
    RecvAckTest();
    RecvAckTest(const RecvAckTest& orig);
    virtual ~RecvAckTest();
public:
    virtual int main(int argc, char** argv);
};

#endif /* RECVACKTEST_H */

