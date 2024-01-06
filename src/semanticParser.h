#ifndef __SEMANTICPARSER_H
#define __SEMANTICPARSER_H

#include"syntacticParser.h"

bool semanticParse();

bool semanticParseCHECKSYMMETRY();
bool semanticParseCOMPUTE();
bool semanticParseCLEAR();
bool semanticParseCROSS();
bool semanticParseDISTINCT();
bool semanticParseEXPORT();
bool semanticParseGROUPBY();
bool semanticParseINDEX();
bool semanticParseJOIN();
bool semanticParseLIST();
bool semanticParseLOAD();
bool semanticParseORDERBY();
bool semanticParseLOADMATRIX();
bool semanticParsePRINT();
bool semanticParsePRINTMATRIX();
bool semanticParsePROJECTION();
bool semanticParseRENAME();
bool semanticParseRENAMEMATRIX();
bool semanticParseSELECTION();
bool semanticParseSORT();
bool semanticParseSOURCE();
bool semanticParseTRANSPOSE();

#endif