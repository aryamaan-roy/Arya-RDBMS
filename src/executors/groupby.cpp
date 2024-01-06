#include "global.h"

bool syntacticParseGROUPBY()
{
    logger.log("syntacticParseGROUPBY");
    if (tokenizedQuery.size() != 13 || tokenizedQuery[2] != "GROUP" || tokenizedQuery[3] != "BY" || tokenizedQuery[5] != "FROM" || tokenizedQuery[7] != "HAVING" || tokenizedQuery[11] != "RETURN")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    parsedQuery.queryType = GROUPBY;
    parsedQuery.groupByResultRelationName = tokenizedQuery[0];
    parsedQuery.groupByRelationName = tokenizedQuery[6];
    parsedQuery.groupByColumnName = tokenizedQuery[4];
    parsedQuery.groupByAggregateAttribute1 = tokenizedQuery[8];
    parsedQuery.groupByAttributeValue = stoi(tokenizedQuery[10]);
    parsedQuery.groupByAggregateAttribute2 = tokenizedQuery[12];

    if (tokenizedQuery[9] == "<")
        parsedQuery.groupByBinaryOperator = LESS_THAN;
    else if (tokenizedQuery[9] == ">")
        parsedQuery.groupByBinaryOperator = GREATER_THAN;
    else if (tokenizedQuery[9] == "<=")
        parsedQuery.groupByBinaryOperator = LEQ;
    else if (tokenizedQuery[9] == ">=")
        parsedQuery.groupByBinaryOperator = GEQ;
    else if (tokenizedQuery[9] == "==")
        parsedQuery.groupByBinaryOperator = EQUAL;
    else if (tokenizedQuery[9] == "!=")
        parsedQuery.groupByBinaryOperator = NOT_EQUAL;
    else
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    // using regex to extract the aggregate function
    regex aggregateFunctionRegex("(SUM|AVG|MIN|MAX|COUNT)\\((.*)\\)");
    smatch match;

    if (regex_match(tokenizedQuery[8], match, aggregateFunctionRegex))
    {
        if (match[1] == "SUM")
            parsedQuery.groupByAggregateFunction = SUM;
        else if (match[1] == "AVG")
            parsedQuery.groupByAggregateFunction = AVG;
        else if (match[1] == "MIN")
            parsedQuery.groupByAggregateFunction = MIN;
        else if (match[1] == "MAX")
            parsedQuery.groupByAggregateFunction = MAX;
        else if (match[1] == "COUNT")
            parsedQuery.groupByAggregateFunction = COUNT;
        parsedQuery.groupByAggregateAttribute1 = match[2];
    }
    else
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    // doing the same for the second aggregate function
    if (regex_match(tokenizedQuery[12], match, aggregateFunctionRegex))
    {
        if (match[1] == "SUM")
            parsedQuery.groupByAggregateFunction2 = SUM;
        else if (match[1] == "AVG")
            parsedQuery.groupByAggregateFunction2 = AVG;
        else if (match[1] == "MIN")
            parsedQuery.groupByAggregateFunction2 = MIN;
        else if (match[1] == "MAX")
            parsedQuery.groupByAggregateFunction2 = MAX;
        else if (match[1] == "COUNT")
            parsedQuery.groupByAggregateFunction2 = COUNT;
        parsedQuery.groupByAggregateAttribute2 = match[2];
    }
    else
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    return true;
}

bool semanticParseGROUPBY()
{
    logger.log("semanticParseGROUPBY");
    if (tableCatalogue.isTable(parsedQuery.groupByResultRelationName))
    {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue.isTable(parsedQuery.groupByRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.groupByColumnName, parsedQuery.groupByRelationName) || !tableCatalogue.isColumnFromTable(parsedQuery.groupByAggregateAttribute1, parsedQuery.groupByRelationName) || !tableCatalogue.isColumnFromTable(parsedQuery.groupByAggregateAttribute2, parsedQuery.groupByRelationName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }

    if (parsedQuery.groupByAggregateAttribute1 != parsedQuery.groupByAggregateAttribute2)
    {
        cout << "SEMANTIC ERROR: Different aggregate attribute" << endl;
        return false;
    }

    if (parsedQuery.groupByAggregateFunction == NO_GROUPBY_AGGREGATE_FUNCTION || parsedQuery.groupByAggregateFunction2 == NO_GROUPBY_AGGREGATE_FUNCTION)
    {
        cout << "SEMANTIC ERROR: Aggregate functions not allowed" << endl;
        return false;
    }

    if (parsedQuery.groupByBinaryOperator == NO_BINOP_CLAUSE)
    {
        cout << "SEMANTIC ERROR: Binary operator not allowed" << endl;
        return false;
    }

    return true;
}

vector<int> helperGroupBy(long long int currentGroupByValue, long long int currentGroupByValueCount, long long int currentGroupByValueSum, long long int currentGroupByValueMin, long long int currentGroupByValueMax)
{
    if (parsedQuery.groupByAggregateFunction2 == SUM)
        return {(int)currentGroupByValue, (int)currentGroupByValueSum};
    else if (parsedQuery.groupByAggregateFunction2 == AVG)
        return {(int)currentGroupByValue, (int)((1.0 * currentGroupByValueSum) / currentGroupByValueCount)};
    else if (parsedQuery.groupByAggregateFunction2 == MIN)
        return {(int)currentGroupByValue, (int)currentGroupByValueMin};
    else if (parsedQuery.groupByAggregateFunction2 == MAX)
        return {(int)currentGroupByValue, (int)currentGroupByValueMax};
    else if (parsedQuery.groupByAggregateFunction2 == COUNT)
        return {(int)currentGroupByValue, (int)currentGroupByValueCount};
    else
        return {-1, -1};
}

void executeGROUPBY()
{
    logger.log("executeGROUPBY");

    Table *table = tableCatalogue.getTable(parsedQuery.groupByRelationName);
    
    string tempTableName = "GroupBy" + parsedQuery.groupByRelationName;
    tempTableName = tempTableName + "Temp";
    
    Table *sortedInitialTable = new Table(tempTableName);

    tableCatalogue.insertTable(sortedInitialTable);

    sortedInitialTable = tableCatalogue.getTable(tempTableName);

    sortedInitialTable->columnCount = table->columnCount;
    sortedInitialTable->rowCount = table->rowCount;
    sortedInitialTable->blockCount = table->blockCount;
    sortedInitialTable->maxRowsPerBlock = table->maxRowsPerBlock;
    sortedInitialTable->rowsPerBlockCount = table->rowsPerBlockCount;

    sortedInitialTable->columns.clear();
    for (auto x : table->columns)
    {
        sortedInitialTable->columns.push_back(x);
    }

    int newPageIndex = 0;
    vector<int> row;
    Cursor cursor;

    if (!table->indexed)
    {
        cursor = table->getCursor();
        row = cursor.getNext();
    }

    vector<vector<int>> rowsData;
    while (!row.empty())
    {
        rowsData.push_back(row);
        if (!table->indexed)
            row = cursor.getNext();
        
        if(rowsData.size() == sortedInitialTable->maxRowsPerBlock)
        {
            bufferManager.writeTablePage(sortedInitialTable->tableName, newPageIndex, rowsData, rowsData.size());
            newPageIndex++;
            rowsData.clear();
        }
    }

    if(!rowsData.empty())
    {
        bufferManager.writeTablePage(sortedInitialTable->tableName, newPageIndex, rowsData, rowsData.size());
        newPageIndex++;
        rowsData.clear();
    }

    vector<int> columnIndices;
    columnIndices.push_back(sortedInitialTable->getColumnIndex(parsedQuery.groupByColumnName));
    vector<SortingStrategy> sortingStrategies;
    sortingStrategies.push_back(ASC);

    sortedInitialTable->sortTable(columnIndices, sortingStrategies);

    // sortedInitialTable->print();

    // cout<<"yaha tak sab changa si"<<endl;


    ////////////////////////////////////////////
    /*
        Now creating the resultant table
    */
    ////////////////////////////////////////////

    string newColumnName = parsedQuery.groupByAggregateAttribute1;
    if (parsedQuery.groupByAggregateFunction2 == SUM)
        newColumnName = "SUM" + newColumnName;
    else if (parsedQuery.groupByAggregateFunction2 == AVG)
        newColumnName = "AVG" + newColumnName;
    else if (parsedQuery.groupByAggregateFunction2 == MIN)
        newColumnName = "MIN" + newColumnName;
    else if (parsedQuery.groupByAggregateFunction2 == MAX)
        newColumnName = "MAX" + newColumnName;
    else if (parsedQuery.groupByAggregateFunction2 == COUNT)
        newColumnName = "COUNT" + newColumnName;

    
    Table *resTable = new Table(parsedQuery.groupByResultRelationName, {parsedQuery.groupByColumnName, newColumnName});
    resTable->blockCount = 0;
    resTable->columnCount = 2;
    resTable->columns = {parsedQuery.groupByColumnName, newColumnName};
    resTable->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (sizeof(int) * resTable->columnCount));
    resTable->rowCount = 0;
    resTable->rowsPerBlockCount.resize(resTable->blockCount * 3, 0);
    resTable->blockify();
    tableCatalogue.insertTable(resTable);

    vector<vector<int>> rows(resTable->maxRowsPerBlock, vector<int>(resTable->columnCount, 0));

    Cursor cursor2 = sortedInitialTable->getCursor();
    vector<int> row2 = cursor2.getNext();

    long long int currentGroupByValueCount = 0;
    long long int currentGroupByValueTotalCount = 0;
    long long int currentGroupByValueSum = 0;
    long long int currentGroupByValueMin = INT_MAX;
    long long int currentGroupByValueMax = INT_MIN;

    vector<vector<int>> groupRows;

    long long int count = 0;

    while (!row2.empty())
    {
        long long int currentGroupByValue = row2[table->getColumnIndex(parsedQuery.groupByColumnName)];
        vector<long long int> groupRow = {currentGroupByValue, 0};

        while (!row2.empty() && row2[table->getColumnIndex(parsedQuery.groupByColumnName)] == currentGroupByValue)
        {
            currentGroupByValueCount++;
            currentGroupByValueTotalCount++;
            currentGroupByValueSum += (long long int)row2[table->getColumnIndex(parsedQuery.groupByAggregateAttribute1)];
            currentGroupByValueMin = (long long int)min(currentGroupByValueMin, (long long int)row2[table->getColumnIndex(parsedQuery.groupByAggregateAttribute1)]);
            currentGroupByValueMax = (long long int)max(currentGroupByValueMax, (long long int)row2[table->getColumnIndex(parsedQuery.groupByAggregateAttribute1)]);

            row2 = cursor2.getNext();
        }

        if (parsedQuery.groupByAggregateFunction == SUM)
            groupRow[1] = currentGroupByValueSum;
        else if (parsedQuery.groupByAggregateFunction == AVG)
            groupRow[1] = (long long int)((1.0 * currentGroupByValueSum) / currentGroupByValueCount);
        else if (parsedQuery.groupByAggregateFunction == MIN)
            groupRow[1] = currentGroupByValueMin;
        else if (parsedQuery.groupByAggregateFunction == MAX)
            groupRow[1] = currentGroupByValueMax;
        else if (parsedQuery.groupByAggregateFunction == COUNT)
            groupRow[1] = currentGroupByValueCount;


        if (parsedQuery.groupByBinaryOperator == LESS_THAN && groupRow[1] < parsedQuery.groupByAttributeValue)
        {
            groupRows.push_back(helperGroupBy(currentGroupByValue, currentGroupByValueCount, currentGroupByValueSum, currentGroupByValueMin, currentGroupByValueMax));
            count++;
        }
        else if (parsedQuery.groupByBinaryOperator == GREATER_THAN && groupRow[1] > parsedQuery.groupByAttributeValue)
        {
            groupRows.push_back(helperGroupBy(currentGroupByValue, currentGroupByValueCount, currentGroupByValueSum, currentGroupByValueMin, currentGroupByValueMax));
            count++;
        }
        else if (parsedQuery.groupByBinaryOperator == LEQ && groupRow[1] <= parsedQuery.groupByAttributeValue)
        {
            groupRows.push_back(helperGroupBy(currentGroupByValue, currentGroupByValueCount, currentGroupByValueSum, currentGroupByValueMin, currentGroupByValueMax));
            count++;
        }
        else if (parsedQuery.groupByBinaryOperator == GEQ && groupRow[1] >= parsedQuery.groupByAttributeValue)
        {
            groupRows.push_back(helperGroupBy(currentGroupByValue, currentGroupByValueCount, currentGroupByValueSum, currentGroupByValueMin, currentGroupByValueMax));
            count++;
        }
        else if (parsedQuery.groupByBinaryOperator == EQUAL && groupRow[1] == parsedQuery.groupByAttributeValue)
        {
            groupRows.push_back(helperGroupBy(currentGroupByValue, currentGroupByValueCount, currentGroupByValueSum, currentGroupByValueMin, currentGroupByValueMax));
            count++;
        }
        else if (parsedQuery.groupByBinaryOperator == NOT_EQUAL && groupRow[1] != parsedQuery.groupByAttributeValue)
        {
            groupRows.push_back(helperGroupBy(currentGroupByValue, currentGroupByValueCount, currentGroupByValueSum, currentGroupByValueMin, currentGroupByValueMax));
            count++;
        }


        if (groupRows.size() == resTable->maxRowsPerBlock)
        {
            bufferManager.writeTablePage(resTable->tableName, resTable->blockCount, groupRows, count);
            resTable->blockCount++;
            resTable->rowsPerBlockCount.emplace_back(groupRows.size());
            resTable->rowCount += count;
            groupRows.clear();
            count = 0;
        }
        currentGroupByValueCount = 0;
        currentGroupByValueSum = 0;
        currentGroupByValueMin = INT_MAX;
        currentGroupByValueMax = INT_MIN;
    }

    if (groupRows.size())
    {
        bufferManager.writeTablePage(resTable->tableName, resTable->blockCount, groupRows, count);
        resTable->blockCount++;
        resTable->rowsPerBlockCount.emplace_back(groupRows.size());
        resTable->rowCount += count;
        groupRows.clear();
        count = 0;
    }

    tableCatalogue.insertTable(resTable);
    resTable->makePermanent();

    // deleting the sorted table
    for (int i = 0; i < sortedInitialTable->blockCount; i++)
        bufferManager.deleteTableFile(sortedInitialTable->tableName, i);

    tableCatalogue.deleteTable(sortedInitialTable->tableName);

    return;
}