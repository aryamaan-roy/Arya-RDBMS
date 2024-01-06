#include "global.h"
/**
 * @brief 
 * SYNTAX: RENAME column_name TO column_name FROM relation_name
 */
bool syntacticParseRENAMEMATRIX()
{
    logger.log("syntacticParseRENAMEMATRIX");
    if (tokenizedQuery.size() != 4)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = RENAMEMATRIX;
    parsedQuery.renameFromMatrixName = tokenizedQuery[2];
    parsedQuery.renameToMatrixName = tokenizedQuery[3];
    return true;
}

bool semanticParseRENAMEMATRIX()
{
    logger.log("semanticParseRENAME");

    if (!matrixCatalogue.isMatrix(parsedQuery.renameFromMatrixName))
    {
        cout << "SEMANTIC ERROR: FROM Matrix doesn't exist" << endl;
        return false;
    }

    return true;

}

void executeRENAMEMATRIX()
{
    logger.log("executeRENAMEMATRIX");

    Matrix *matrix = matrixCatalogue.getMatrix(parsedQuery.renameFromMatrixName);
    matrix->renameMatrix(parsedQuery.renameToMatrixName);

    return;
}