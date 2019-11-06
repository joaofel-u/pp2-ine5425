/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PartialTest.h
 * Author: joaofel-u
 *
 * Created on 5 de novembro de 2019, 19:08
 */

#ifndef PARTIALTEST_H
#define PARTIALTEST_H

#include "BaseConsoleGenesysApplication.h"
#include "Model.h"

class PartialTest: public BaseConsoleGenesysApplication {
public:
    PartialTest();
    PartialTest(const PartialTest& orig);
    virtual ~PartialTest();
public:
    virtual int main(int argc, char** argv);
};

#endif /* PARTIALTEST_H */

