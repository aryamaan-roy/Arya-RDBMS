#include "global.h"
/**
 * @brief 
 * SYNTAX: LOAD MATRIX matrix_name
 */
bool syntacticParsePRINTMATRIX()
{
    logger.log("syntacticParsePRINTMATRIX");
    if (tokenizedQuery.size() != 3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = PRINTMATRIX;
    parsedQuery.printMatrixName = tokenizedQuery[2];
    return true;
}

bool semanticParsePRINTMATRIX()
{
    logger.log("semanticParseLOADMATRIX");
    if (!matrixCatalogue.isMatrix(parsedQuery.printMatrixName))
    {
        cout << "SEMANTIC ERROR: Matrix does not exists" << endl;
        return false;
    }
    return true;
}

void executePRINTMATRIX()
{
    logger.log("executePRINTMATRIX");
    Matrix *matrix = matrixCatalogue.getMatrix(parsedQuery.printMatrixName);
    matrix->printMatrix();
    return;
}