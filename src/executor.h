#ifndef __EXECUTOR_H
#define __EXECUTOR_H

#include"semanticParser.h"

void executeCommand();

void executeCHECKSYMMETRY();
void executeCOMPUTE();
void executeCLEAR();
void executeCROSS();
void executeDISTINCT();
void executeEXPORT();
void executeGROUPBY();
void executeINDEX();
void executeJOIN();
void executeLIST();
void executeLOAD();
void executeLOADMATRIX();
void executeORDERBY();
void executePRINT();
void executePRINTMATRIX();
void executePROJECTION();
void executeRENAME();
void executeRENAMEMATRIX();
void executeSELECTION();
void executeSORT();
void executeSOURCE();
void executeTRANSPOSE();

bool evaluateBinOp(int value1, int value2, BinaryOperator binaryOperator);
void printRowCount(int rowCount);

#endif