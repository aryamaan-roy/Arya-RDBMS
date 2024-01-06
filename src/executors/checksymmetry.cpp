#include "global.h"
/**
 * @brief 
 * SYNTAX: TRANSPOSE matrix_name
 */
bool syntacticParseCHECKSYMMETRY()
{
    logger.log("syntacticParseCHECKSYMMETRY");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = CHECKSYMMETRY;
    parsedQuery.checksymmetryMatrixName = tokenizedQuery[1];
    return true;
}

bool semanticParseCHECKSYMMETRY()
{
    logger.log("semanticParseCHECKSYMMETRY");
    if (!matrixCatalogue.isMatrix(parsedQuery.checksymmetryMatrixName))
    {
        cout << "SEMANTIC ERROR: Matrix doesn't exist" << endl;
        return false;
    }
    return true;

    if (!isMatrix(parsedQuery.checksymmetryMatrixName))
    {
        cout << "SEMANTIC ERROR: The file isn't a matrix" << endl;
        return false;
    }

    return true;
}

void executeCHECKSYMMETRY()
{
    logger.log("executeCheckSymmetryMatrixName");
    Matrix *matrix = matrixCatalogue.getMatrix(parsedQuery.checksymmetryMatrixName);
    matrix->checksymmetry();
    return;
}