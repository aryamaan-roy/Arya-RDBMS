#include "global.h"
/**
 * @brief
 * SYNTAX: R <- JOIN relation_name1, relation_name2 ON column_name1 bin_op column_name2
 */
bool syntacticParseJOIN()
{
    logger.log("syntacticParseJOIN");
    if (tokenizedQuery.size() != 9 || tokenizedQuery[5] != "ON")
    {
        cout << "SYNTAC ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = JOIN;
    parsedQuery.joinResultRelationName = tokenizedQuery[0];
    parsedQuery.joinFirstRelationName = tokenizedQuery[3];
    parsedQuery.joinSecondRelationName = tokenizedQuery[4];
    parsedQuery.joinFirstColumnName = tokenizedQuery[6];
    parsedQuery.joinSecondColumnName = tokenizedQuery[8];

    string binaryOperator = tokenizedQuery[7];
    if (binaryOperator == "<")
        parsedQuery.joinBinaryOperator = LESS_THAN;
    else if (binaryOperator == ">")
        parsedQuery.joinBinaryOperator = GREATER_THAN;
    else if (binaryOperator == ">=" || binaryOperator == "=>")
        parsedQuery.joinBinaryOperator = GEQ;
    else if (binaryOperator == "<=" || binaryOperator == "=<")
        parsedQuery.joinBinaryOperator = LEQ;
    else if (binaryOperator == "==")
        parsedQuery.joinBinaryOperator = EQUAL;
    else if (binaryOperator == "!=")
        parsedQuery.joinBinaryOperator = NOT_EQUAL;
    else
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    return true;
}

bool semanticParseJOIN()
{
    logger.log("semanticParseJOIN");

    if (tableCatalogue.isTable(parsedQuery.joinResultRelationName))
    {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue.isTable(parsedQuery.joinFirstRelationName) || !tableCatalogue.isTable(parsedQuery.joinSecondRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.joinFirstColumnName, parsedQuery.joinFirstRelationName) || !tableCatalogue.isColumnFromTable(parsedQuery.joinSecondColumnName, parsedQuery.joinSecondRelationName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }
    return true;
}

void executeJOIN()
{
    logger.log("executeJOIN");

    Table *firstTable = tableCatalogue.getTable(parsedQuery.joinFirstRelationName);
    string sortedfirst = firstTable->tableName + "_sortedTemporaryXYZABCALPHABET";
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

    /*
        Sorting the first Table
    */
    vector<int> columnIndicesFirstTable;
    columnIndicesFirstTable.push_back(firstTable->getColumnIndex(parsedQuery.joinFirstColumnName));
    vector<SortingStrategy> sortingStrategiesFirstTable;
    sortingStrategiesFirstTable.push_back(ASC);
    sortedFirstTable->sortTable(columnIndicesFirstTable, sortingStrategiesFirstTable);

    /*
        Sorting the second Table
    */

    Table *secondTable = tableCatalogue.getTable(parsedQuery.joinSecondRelationName);
    
    string sortedsecond = secondTable->tableName + "_sortedTemporaryXYZABCALPHABET";
    Table *sortedSecondTable = new Table(sortedsecond);
    tableCatalogue.insertTable(sortedSecondTable);

    sortedSecondTable = tableCatalogue.getTable(sortedsecond);

    sortedSecondTable->columnCount = secondTable->columnCount;
    sortedSecondTable->rowCount = secondTable->rowCount;
    sortedSecondTable->blockCount = secondTable->blockCount;
    sortedSecondTable->maxRowsPerBlock = secondTable->maxRowsPerBlock;
    sortedSecondTable->rowsPerBlockCount = secondTable->rowsPerBlockCount;

    sortedSecondTable->columns.clear();
    for (auto x : secondTable->columns)
    {
        sortedSecondTable->columns.push_back(x);
    }

    newPageIndex = 0;
    row.clear();

    if (!secondTable->indexed)
    {
        cursor = secondTable->getCursor();
        row = cursor.getNext();
    }

    while (!row.empty())
    {
        rows.push_back(row);
        if (!secondTable->indexed)
            row = cursor.getNext();
        
        if(rows.size() == sortedSecondTable->maxRowsPerBlock)
        {
            bufferManager.writeTablePage(sortedSecondTable->tableName, newPageIndex, rows, rows.size());
            newPageIndex++;
            rows.clear();
        }
    }

    if(!rows.empty())
    {
        bufferManager.writeTablePage(sortedSecondTable->tableName, newPageIndex, rows, rows.size());
        newPageIndex++;
        rows.clear();
    }


    vector<int> columnIndicesSecondTable;
    columnIndicesSecondTable.push_back(secondTable->getColumnIndex(parsedQuery.joinSecondColumnName));
    vector<SortingStrategy> sortingStrategiesSecondTable;
    sortingStrategiesSecondTable.push_back(ASC);
    sortedSecondTable->sortTable(columnIndicesSecondTable, sortingStrategiesSecondTable);

    // sortedSecondTable->print();

    /*
        now looking at all the blocks of the sorted tables and joining them
    */

    Table *joinTable = new Table(parsedQuery.joinResultRelationName, firstTable->columns);
    joinTable->blockCount = 0;
    joinTable->columnCount = firstTable->columnCount + secondTable->columnCount;
    joinTable->columns.insert(joinTable->columns.end(), secondTable->columns.begin(), secondTable->columns.end());
    joinTable->smallestInColumns = firstTable->smallestInColumns;
    joinTable->largestInColumns = firstTable->largestInColumns;
    joinTable->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (sizeof(int) * joinTable->columnCount));
    joinTable->rowCount = 0;
    joinTable->rowsPerBlockCount.resize(3, 0);
    tableCatalogue.insertTable(joinTable);

    Cursor cursor1 = sortedFirstTable->getCursor();

    vector<int> row1 = cursor1.getNext();
    vector<vector<int>> joinedRows;
    while (!row1.empty())
    {
        Cursor cursor2 = sortedSecondTable->getCursor();
        vector<int> row2 = cursor2.getNext();
        while (!row2.empty())
        {
            if (parsedQuery.joinBinaryOperator == LESS_THAN && row1[firstTable->getColumnIndex(parsedQuery.joinFirstColumnName)] < row2[secondTable->getColumnIndex(parsedQuery.joinSecondColumnName)])
            {
                vector<int> curr_row;

                for (auto i : row1)
                    curr_row.push_back(i);
                for (auto i : row2)
                    curr_row.push_back(i);

                joinTable->rowsPerBlockCount[joinTable->blockCount]++;
                joinTable->rowCount++;
                joinedRows.push_back(curr_row);
            }
            else if (parsedQuery.joinBinaryOperator == GREATER_THAN && row1[firstTable->getColumnIndex(parsedQuery.joinFirstColumnName)] > row2[secondTable->getColumnIndex(parsedQuery.joinSecondColumnName)])
            {
                vector<int> curr_row;

                for (auto i : row1)
                    curr_row.push_back(i);
                for (auto i : row2)
                    curr_row.push_back(i);

                joinTable->rowsPerBlockCount[joinTable->blockCount]++;
                joinTable->rowCount++;
                joinedRows.push_back(curr_row);
            }
            else if (parsedQuery.joinBinaryOperator == LEQ && row1[firstTable->getColumnIndex(parsedQuery.joinFirstColumnName)] <= row2[secondTable->getColumnIndex(parsedQuery.joinSecondColumnName)])
            {
                vector<int> curr_row;

                for (auto i : row1)
                    curr_row.push_back(i);
                for (auto i : row2)
                    curr_row.push_back(i);

                joinTable->rowsPerBlockCount[joinTable->blockCount]++;
                joinTable->rowCount++;
                joinedRows.push_back(curr_row);
            }
            else if (parsedQuery.joinBinaryOperator == GEQ && row1[firstTable->getColumnIndex(parsedQuery.joinFirstColumnName)] >= row2[secondTable->getColumnIndex(parsedQuery.joinSecondColumnName)])
            {
                vector<int> curr_row;

                for (auto i : row1)
                    curr_row.push_back(i);
                for (auto i : row2)
                    curr_row.push_back(i);

                joinTable->rowsPerBlockCount[joinTable->blockCount]++;
                joinTable->rowCount++;
                joinedRows.push_back(curr_row);
            }
            else if (parsedQuery.joinBinaryOperator == EQUAL && row1[firstTable->getColumnIndex(parsedQuery.joinFirstColumnName)] == row2[secondTable->getColumnIndex(parsedQuery.joinSecondColumnName)])
            {
                vector<int> curr_row;

                for (auto i : row1)
                    curr_row.push_back(i);
                for (auto i : row2)
                    curr_row.push_back(i);

                joinTable->rowsPerBlockCount[joinTable->blockCount]++;
                joinTable->rowCount++;
                joinedRows.push_back(curr_row);
            }
            else if (parsedQuery.joinBinaryOperator == NOT_EQUAL && row1[firstTable->getColumnIndex(parsedQuery.joinFirstColumnName)] != row2[secondTable->getColumnIndex(parsedQuery.joinSecondColumnName)])
            {
                vector<int> curr_row;

                for (auto i : row1)
                    curr_row.push_back(i);
                for (auto i : row2)
                    curr_row.push_back(i);

                joinTable->rowsPerBlockCount[joinTable->blockCount]++;
                joinTable->rowCount++;
                joinedRows.push_back(curr_row);
            }

            if (joinTable->rowsPerBlockCount[joinTable->blockCount] == joinTable->maxRowsPerBlock)
            {
                bufferManager.writeTablePage(joinTable->tableName, joinTable->blockCount, joinedRows, joinTable->maxRowsPerBlock);
                joinTable->blockCount++;
                joinTable->rowsPerBlockCount.push_back(0);
                joinedRows.clear();
            }

            row2 = cursor2.getNext();
        }
        row1 = cursor1.getNext();
    }

    if (joinedRows.size())
    {
        bufferManager.writeTablePage(joinTable->tableName, joinTable->blockCount, joinedRows, joinedRows.size());
        joinTable->blockCount++;
        joinTable->rowsPerBlockCount.push_back(0);
        joinedRows.clear();
    }

    // // deleting the temporary tables

    for (int i = 0; i < sortedFirstTable->blockCount; i++)
        bufferManager.deleteTableFile(sortedFirstTable->tableName, i);

    for (int i = 0; i < sortedSecondTable->blockCount; i++)
        bufferManager.deleteTableFile(sortedSecondTable->tableName, i);

    tableCatalogue.deleteTable(sortedFirstTable->tableName);
    tableCatalogue.deleteTable(sortedSecondTable->tableName);

    // to export into the csv file
    joinTable->makePermanent();

    return;
}