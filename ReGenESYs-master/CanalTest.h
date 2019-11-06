/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CanalTest.h
 * Author: joaofel-u
 *
 * Created on 5 de novembro de 2019, 19:08
 */

#ifndef CANALTEST_H
#define CANALTEST_H

#include "BaseConsoleGenesysApplication.h"
#include "Model.h"

class CanalTest: public BaseConsoleGenesysApplication {
public:
    CanalTest();
    CanalTest(const CanalTest& orig);
    virtual ~CanalTest();
public:
    virtual int main(int argc, char** argv);
};

#endif /* CANALTEST_H */

