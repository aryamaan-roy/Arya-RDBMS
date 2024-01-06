#include "global.h"
/**
 * @brief 
 * SYNTAX: TRANSPOSE matrix_name
 */
bool syntacticParseCOMPUTE()
{
    logger.log("syntacticParseTRANSPOSE");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = COMPUTE;
    parsedQuery.computeMatrixName = tokenizedQuery[1];
    return true;
}

bool semanticParseCOMPUTE()
{
    logger.log("semanticParseTRANSPOSE");
    if (!matrixCatalogue.isMatrix(parsedQuery.computeMatrixName))
    {
        cout << "SEMANTIC ERROR: Matrix doesn't exist" << endl;
        return false;
    }
    return true;

    if (!isMatrix(parsedQuery.computeMatrixName))
    {
        cout << "SEMANTIC ERROR: The file isn't a matrix" << endl;
        return false;
    }

    return true;
}

void executeCOMPUTE()
{
    logger.log("executeTRANSPOSE");
    Matrix *matrix = matrixCatalogue.getMatrix(parsedQuery.computeMatrixName);
    matrix->compute();
    return;
}