/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TestRemove.h
 * Author: joaofel-u
 *
 * Created on 20 de setembro de 2019, 15:28
 */

#ifndef TESTREMOVE_H
#define TESTREMOVE_H

#include "BaseConsoleGenesysApplication.h"
#include "Model.h"

class TestRemove: public BaseConsoleGenesysApplication {
public:
    TestRemove();
    TestRemove(const TestRemove& orig);
    virtual ~TestRemove();
public:
    virtual int main(int argc, char** argv);
};

#endif /* TESTREMOVE_H */

