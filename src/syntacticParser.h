#ifndef __SYNTACTICPARSER_H
#define __SYNTACTICPARSER_H

#include "tableCatalogue.h"
#include "matrixCatalogue.h"

using namespace std;

enum DataType
{
    TABLE,
    MATRIX
};

enum QueryType
{
    CHECKSYMMETRY,
    COMPUTE,
    CLEAR,
    CROSS,
    DISTINCT,
    EXPORT,
    GROUPBY,
    INDEX,
    JOIN,
    LIST,
    LOAD,
    LOADMATRIX,
    ORDERBY,
    PRINT,
    PRINTMATRIX,
    PROJECTION,
    RENAME,
    RENAMEMATRIX,
    SELECTION,
    SORT,
    SOURCE,
    TRANSPOSEMATRIX,
    UNDETERMINED
};

enum BinaryOperator
{
    LESS_THAN,
    GREATER_THAN,
    LEQ,
    GEQ,
    EQUAL,
    NOT_EQUAL,
    NO_BINOP_CLAUSE
};

// enum SortingStrategy
// {
//     ASC,
//     DESC,
//     NO_SORT_CLAUSE
// };

enum SelectType
{
    COLUMN,
    INT_LITERAL,
    NO_SELECT_CLAUSE
};

enum GroupByAggregateFunction
{
    SUM,
    AVG,
    MIN,
    MAX,
    COUNT,
    NO_GROUPBY_AGGREGATE_FUNCTION
};

class ParsedQuery
{

public:
    QueryType queryType = UNDETERMINED;

    string clearRelationName = "";

    string crossResultRelationName = "";
    string crossFirstRelationName = "";
    string crossSecondRelationName = "";

    string checksymmetryMatrixName = "";

    string computeMatrixName = "";

    string deleteRelationName = "";
    vector<int> deleteRow;

    string distinctResultRelationName = "";
    string distinctRelationName = "";

    string exportRelationOrMatrixName = "";

    string groupByRelationName = "";
    string groupByResultRelationName = "";
    string groupByColumnName = "";
    GroupByAggregateFunction groupByAggregateFunction = NO_GROUPBY_AGGREGATE_FUNCTION;
    string groupByAggregateAttribute1 = "";
    BinaryOperator groupByBinaryOperator = NO_BINOP_CLAUSE;
    int groupByAttributeValue = 0;
    GroupByAggregateFunction groupByAggregateFunction2 = NO_GROUPBY_AGGREGATE_FUNCTION;
    string groupByAggregateAttribute2 = "";


    IndexingStrategy indexingStrategy = NOTHING;
    string indexColumnName = "";
    string indexRelationName = "";

    string insertRelationName = "";
    vector<int> insertRow;

    BinaryOperator joinBinaryOperator = NO_BINOP_CLAUSE;
    string joinResultRelationName = "";
    string joinFirstRelationName = "";
    string joinSecondRelationName = "";
    string joinFirstColumnName = "";
    string joinSecondColumnName = "";

    string loadRelationName = "";
    string loadMatrixName = "";

    string orderByAttributeName = "";
    string orderByRelationName = "";
    string orderByResultRelationName = "";
    SortingStrategy orderBySortingStrategy = NO_SORT_CLAUSE;

    string transposeMatrixName = ""; 

    string printRelationName = "";
    string printMatrixName = "";

    string projectionResultRelationName = "";
    vector<string> projectionColumnList;
    string projectionRelationName = "";

    string renameFromColumnName = "";
    string renameToColumnName = "";
    string renameRelationName = "";

    string renameFromMatrixName = "";
    string renameToMatrixName = "";

    SelectType selectType = NO_SELECT_CLAUSE;
    BinaryOperator selectionBinaryOperator = NO_BINOP_CLAUSE;
    string selectionResultRelationName = "";
    string selectionRelationName = "";
    string selectionFirstColumnName = "";
    string selectionSecondColumnName = "";
    int selectionIntLiteral = 0;

    SortingStrategy sortingStrategy = NO_SORT_CLAUSE;
    string sortResultRelationName = "";
    vector<string> sortColumnNameList;
    vector<int> sortColumnIndexList;
    vector<SortingStrategy> sortingStrategyList;
    string sortRelationName = "";
    int sortBufferSize = 10;
    // int sortBufferSize = 3;


    string sourceFileName = "";

    ParsedQuery();
    void clear();
};

bool syntacticParse();
bool syntacticParseCHECKSYMMETRY();
bool syntacticParseCOMPUTE();
bool syntacticParseCLEAR();
bool syntacticParseCROSS();
bool syntacticParseDISTINCT();
bool syntacticParseEXPORT();
bool syntacticParseINDEX();
bool syntacticParseGROUPBY();
bool syntacticParseJOIN();
bool syntacticParseLIST();
bool syntacticParseLOAD();
bool syntacticParseLOADMATRIX();
bool syntacticParseORDERBY();
bool syntacticParsePRINT();
bool syntacticParsePRINTMATRIX();
bool syntacticParsePROJECTION();
bool syntacticParseRENAME();
bool syntacticParseRENAMEMATRIX();
bool syntacticParseSELECTION();
bool syntacticParseSORT();
bool syntacticParseSOURCE();
bool syntacticParseTRANSPOSE();

bool isFileExists(string tableName);
bool isQueryFile(string fileName);
bool isMatrix(string matrixName);

#endif