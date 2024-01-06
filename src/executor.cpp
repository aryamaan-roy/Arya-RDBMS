#include"global.h"

void executeCommand(){

    switch(parsedQuery.queryType){
        case CHECKSYMMETRY: executeCHECKSYMMETRY(); break;
        case COMPUTE: executeCOMPUTE(); break;
        case CLEAR: executeCLEAR(); break;
        case CROSS: executeCROSS(); break;
        case DISTINCT: executeDISTINCT(); break;
        case EXPORT: executeEXPORT(); break;
        case GROUPBY: executeGROUPBY(); break;
        case JOIN: executeJOIN(); break;
        case LIST: executeLIST(); break;
        case LOAD: executeLOAD(); break;
        case LOADMATRIX: executeLOADMATRIX(); break;
        case ORDERBY: executeORDERBY(); break;
        case PRINT: executePRINT(); break;
        case PRINTMATRIX: executePRINTMATRIX(); break;
        case PROJECTION: executePROJECTION(); break;
        case RENAME: executeRENAME(); break;
        case RENAMEMATRIX: executeRENAMEMATRIX(); break;
        case SELECTION: executeSELECTION(); break;
        case SORT: executeSORT(); break;
        case SOURCE: executeSOURCE(); break;
        case TRANSPOSEMATRIX: executeTRANSPOSE(); break;
        default: cout<<"PARSING ERROR"<<endl;
    }

    return;
}

void printRowCount(int rowCount){
    cout<<"\n\nRow Count: "<<rowCount<<endl;
    return;
}