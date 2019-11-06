/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TransmissorTest.h
 * Author: joaofel-u
 *
 * Created on 5 de novembro de 2019, 19:08
 */

#ifndef TRANSMISSORTEST_H
#define TRANSMISSORTEST_H

#include "BaseConsoleGenesysApplication.h"
#include "Model.h"

class TransmissorTest: public BaseConsoleGenesysApplication {
public:
    TransmissorTest();
    TransmissorTest(const TransmissorTest& orig);
    virtual ~TransmissorTest();
public:
    virtual int main(int argc, char** argv);
};

#endif /* TRANSMISSORTEST_H */

