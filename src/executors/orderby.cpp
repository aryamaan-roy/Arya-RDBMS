#include "global.h"

bool syntacticParseORDERBY()
{
    logger.log("syntacticParseORDERBY");
    if (tokenizedQuery.size() != 8 || tokenizedQuery[2] != "ORDER" || tokenizedQuery[3] != "BY" || tokenizedQuery[6] != "ON")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = ORDERBY;
    parsedQuery.orderByResultRelationName = tokenizedQuery[0];
    parsedQuery.orderByAttributeName = tokenizedQuery[4];
    if(tokenizedQuery[5]!="ASC" && tokenizedQuery[5]!="DESC")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.orderBySortingStrategy = (tokenizedQuery[5] == "ASC") ? ASC : DESC;
    parsedQuery.orderByRelationName = tokenizedQuery[7];
    return true;
}

bool semanticParseORDERBY()
{
    logger.log("semanticParseORDERBY");
    if (tableCatalogue.isTable(parsedQuery.orderByResultRelationName))
    {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue.isTable(parsedQuery.orderByRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.orderByAttributeName, parsedQuery.orderByRelationName))
    {    
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }
    return true;
}

void executeORDERBY()
{
    logger.log("executeORDERBY");

    Table *firstTable = tableCatalogue.getTable(parsedQuery.orderByRelationName);
    string sortedfirst = parsedQuery.orderByResultRelationName;
    Table *sortedFirstTable = new Table(sortedfirst);
    tableCatalogue.insertTable(sortedFirstTable);

    sortedFirstTable = tableCatalogue.getTable(sortedfirst);

    sortedFirstTable->columnCount = firstTable->columnCount;
    sortedFirstTable->rowCount = firstTable->rowCount;
    sortedFirstTable->blockCount = firstTable->blockCount;
    sortedFirstTable->maxRowsPerBlock = firstTable->maxRowsPerBlock;
    sortedFirstTable->rowsPerBlockCount = firstTable->rowsPerBlockCount;

    sortedFirstTable->columns.clear();
    for (auto x : firstTable->columns)
    {
        sortedFirstTable->columns.push_back(x);
    }

    int newPageIndex = 0;
    vector<int> row;
    Cursor cursor;

    if (!firstTable->indexed)
    {
        cursor = firstTable->getCursor();
        row = cursor.getNext();
    }

    vector<vector<int>> rows;
    while (!row.empty())
    {
        rows.push_back(row);
        if (!firstTable->indexed)
            row = cursor.getNext();
        
        if(rows.size() == sortedFirstTable->maxRowsPerBlock)
        {
            bufferManager.writeTablePage(sortedFirstTable->tableName, newPageIndex, rows, rows.size());
            newPageIndex++;
            rows.clear();
        }
    }

    if(!rows.empty())
    {
        bufferManager.writeTablePage(sortedFirstTable->tableName, newPageIndex, rows, rows.size());
        newPageIndex++;
        rows.clear();
    }

    vector<int> columnIndices;
    columnIndices.push_back(sortedFirstTable->getColumnIndex(parsedQuery.orderByAttributeName));

    vector<SortingStrategy> sortingStrategies;
    sortingStrategies.push_back(parsedQuery.orderBySortingStrategy);
    
    sortedFirstTable->sortTable(columnIndices, sortingStrategies);

    // to export into the csv file
    sortedFirstTable->makePermanent();

    return;
}