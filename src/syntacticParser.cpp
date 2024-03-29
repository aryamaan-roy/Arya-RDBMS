#include "global.h"

bool syntacticParse()
{
    logger.log("syntacticParse");
    string possibleQueryType = tokenizedQuery[0];

    if (tokenizedQuery.size() < 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    if (possibleQueryType == "CHECKSYMMETRY")
    {
        return syntacticParseCHECKSYMMETRY();
    }
    else if(possibleQueryType == "COMPUTE")
    {
        return syntacticParseCOMPUTE();
    }
    else if (possibleQueryType == "CLEAR")
        return syntacticParseCLEAR();
    else if (possibleQueryType == "INDEX")
        return syntacticParseINDEX();
    else if (possibleQueryType == "LIST")
        return syntacticParseLIST();
    else if (possibleQueryType == "LOAD" && tokenizedQuery[1] == "MATRIX")
        return syntacticParseLOADMATRIX();
    else if (possibleQueryType == "LOAD")
        return syntacticParseLOAD();
    else if (possibleQueryType == "PRINT")
    {
        if(tokenizedQuery[1] == "MATRIX")
            return syntacticParsePRINTMATRIX();
        else
            return syntacticParsePRINT();
    }
    else if (possibleQueryType == "RENAME" && tokenizedQuery[1] == "MATRIX")
        return syntacticParseRENAMEMATRIX();
    else if (possibleQueryType == "RENAME")
        return syntacticParseRENAME();
    else if (possibleQueryType == "EXPORT")
        return syntacticParseEXPORT();
    else if (possibleQueryType == "SORT")
        return syntacticParseSORT();
    else if (possibleQueryType == "SOURCE")
        return syntacticParseSOURCE();
    else if (possibleQueryType == "TRANSPOSE" && tokenizedQuery[1] == "MATRIX")
    {
        return syntacticParseTRANSPOSE();
    }
    else
    {
        string resultantRelationName = possibleQueryType;
        if (tokenizedQuery[1] != "<-" || tokenizedQuery.size() < 3)
        {
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
        possibleQueryType = tokenizedQuery[2];
        
        if(possibleQueryType == "ORDER")
        {
            return syntacticParseORDERBY();
        }
        else if(possibleQueryType == "GROUP")
        {
            return syntacticParseGROUPBY();
        }
        else if (possibleQueryType == "PROJECT")
            return syntacticParsePROJECTION();
        else if (possibleQueryType == "SELECT")
            return syntacticParseSELECTION();
        else if (possibleQueryType == "JOIN")
            return syntacticParseJOIN();
        else if (possibleQueryType == "CROSS")
            return syntacticParseCROSS();
        else if (possibleQueryType == "DISTINCT")
            return syntacticParseDISTINCT();
        else
        {
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
    }
    return false;
}

ParsedQuery::ParsedQuery()
{
}

void ParsedQuery::clear()
{
    logger.log("ParseQuery::clear");
    this->queryType = UNDETERMINED;
    
    this->clearRelationName = "";

    this->crossResultRelationName = "";
    this->crossFirstRelationName = "";
    this->crossSecondRelationName = "";

    this->deleteRelationName = "";
    this->deleteRow.clear();

    this->distinctResultRelationName = "";
    this->distinctRelationName = "";

    this->exportRelationOrMatrixName = "";

    this->groupByResultRelationName = "";
    this->groupByRelationName = "";
    this->groupByColumnName = "";
    this->groupByAggregateFunction = NO_GROUPBY_AGGREGATE_FUNCTION;
    this->groupByAggregateAttribute1 = "";
    this->groupByBinaryOperator = NO_BINOP_CLAUSE;
    this->groupByAttributeValue = 0;
    this->groupByAggregateFunction2 = NO_GROUPBY_AGGREGATE_FUNCTION;
    this->groupByAggregateAttribute2 = "";


    this->indexingStrategy = NOTHING;
    this->indexColumnName = "";
    this->indexRelationName = "";

    this->insertRelationName = "";
    this->insertRow.clear();

    this->joinBinaryOperator = NO_BINOP_CLAUSE;
    this->joinResultRelationName = "";
    this->joinFirstRelationName = "";
    this->joinSecondRelationName = "";
    this->joinFirstColumnName = "";
    this->joinSecondColumnName = "";

    this->loadRelationName = "";
    this->loadMatrixName = "";

    this->transposeMatrixName = ""; 
    
    this->printRelationName = "";

    this->projectionResultRelationName = "";
    this->projectionColumnList.clear();
    this->projectionRelationName = "";

    this->renameFromColumnName = "";
    this->renameToColumnName = "";
    this->renameRelationName = "";

    this->selectType = NO_SELECT_CLAUSE;
    this->selectionBinaryOperator = NO_BINOP_CLAUSE;
    this->selectionResultRelationName = "";
    this->selectionRelationName = "";
    this->selectionFirstColumnName = "";
    this->selectionSecondColumnName = "";
    this->selectionIntLiteral = 0;

    this->sortingStrategy = NO_SORT_CLAUSE;
    this->sortResultRelationName = "";
    this->sortColumnNameList.clear();
    this->sortColumnIndexList.clear();
    this->sortingStrategyList.clear();
    this->sortRelationName = "";

    this->sourceFileName = "";
}

/**
 * @brief Checks to see if source file exists. Called when LOAD command is
 * invoked.
 *
 * @param tableName 
 * @return true 
 * @return false 
 */
bool isFileExists(string tableName)
{
    string fileName = "../data/" + tableName + ".csv";
    struct stat buffer;
    return (stat(fileName.c_str(), &buffer) == 0);
}

/**
 * @brief Checks to see if source file exists. Called when SOURCE command is
 * invoked.
 *
 * @param tableName 
 * @return true 
 * @return false 
 */
bool isQueryFile(string fileName){
    fileName = "../data/" + fileName + ".ra";
    struct stat buffer;
    return (stat(fileName.c_str(), &buffer) == 0);
}

/**
 * @brief Checks to see if source file is a matrix. Called when LOAD MATRIX
 * command or EXPORT (matrix) command is invoked.
 *
 * @param matrixName 
 * @return TABLE 
 * @return MATRIX 
 */

bool isMatrix(string matrixName)
{
    string fileName = "../data/" + matrixName + ".csv";
    ifstream fin(fileName, ios::in);

    string firstWord;
    getline(fin, firstWord, ',');
    firstWord.erase(std::remove_if(firstWord.begin(), firstWord.end(), ::isspace), firstWord.end());

    try
    {
        stoi(firstWord);
        return true;
    }
    catch (exception e)
    {
        return false;
    }
}