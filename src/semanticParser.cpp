#include"global.h"

bool semanticParse(){
    logger.log("semanticParse");

    switch(parsedQuery.queryType){
        case CHECKSYMMETRY: return semanticParseCHECKSYMMETRY();
        case COMPUTE: return semanticParseCOMPUTE();
        case CLEAR: return semanticParseCLEAR();
        case CROSS: return semanticParseCROSS();
        case DISTINCT: return semanticParseDISTINCT();
        case EXPORT: return semanticParseEXPORT();
        case GROUPBY: return semanticParseGROUPBY();
        case INDEX: return semanticParseINDEX();
        case JOIN: return semanticParseJOIN();
        case LIST: return semanticParseLIST();
        case LOAD: return semanticParseLOAD();
        case LOADMATRIX: return semanticParseLOADMATRIX();
        case ORDERBY: return semanticParseORDERBY();
        case PRINT: return semanticParsePRINT();
        case PRINTMATRIX: return semanticParsePRINTMATRIX();
        case PROJECTION: return semanticParsePROJECTION();
        case RENAME: return semanticParseRENAME();
        case RENAMEMATRIX: return semanticParseRENAMEMATRIX();
        case SELECTION: return semanticParseSELECTION();
        case SORT: return semanticParseSORT();
        case SOURCE: return semanticParseSOURCE();
        case TRANSPOSEMATRIX: return semanticParseTRANSPOSE();
        default: cout<<"SEMANTIC ERROR"<<endl;
    }

    return false;
}