/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TestHoldSignal.h
 * Author: joaofel-u
 *
 * Created on 23 de outubro de 2019, 08:32
 */

#ifndef TESTHOLDSIGNAL_H
#define TESTHOLDSIGNAL_H

#include "BaseConsoleGenesysApplication.h"
#include "Model.h"

class TestHoldSignal: public BaseConsoleGenesysApplication {
public:
    TestHoldSignal();
    TestHoldSignal(const TestHoldSignal& orig);
    virtual ~TestHoldSignal();
public:
    virtual int main(int argc, char** argv);
};

#endif /* TESTHOLDSIGNAL_H */

