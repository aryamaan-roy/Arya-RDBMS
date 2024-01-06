#ifndef __TABLE_H
#define __TABLE_H

#include "cursor.h"
// #include "syntacticParser.h"


enum SortingStrategy
{
    ASC,
    DESC,
    NO_SORT_CLAUSE
};


enum IndexingStrategy {
    NOTHING
};

#define INIT_INDEXED_CAPACITY 0.7
#define REINDEX_MIN_THRESH 0.3
#define DEFAULT_INDEX_RESERVE 10

/**
 * @brief The Table class holds all information related to a loaded table. It
 * also implements methods that interact with the parsers, executors, cursors
 * and the buffer manager. There are typically 2 ways a table object gets
 * created through the course of the workflow - the first is by using the LOAD
 * command and the second is to use assignment statements (SELECT, PROJECT,
 * JOIN, SORT, CROSS and DISTINCT). 
 *
 */
class Table {

   public:
    vector<int> smallestInColumns;
    vector<int> largestInColumns;

    string sourceFileName = "";
    string tableName = "";
    vector<string> columns;
    uint columnCount = 0;
    long long int rowCount = 0;
    uint blockCount = 0;
    uint maxRowsPerBlock = 0;
    vector<uint> rowsPerBlockCount;
    bool indexed = false;
    int indexedColumn = -1;
    IndexingStrategy indexingStrategy = NOTHING;

    bool extractColumnNames(string firstLine);
    bool blockify();
    void updateStatistics(vector<int> row);
    Table();
    Table(string tableName);
    Table(string tableName, vector<string> columns);
    bool load();
    bool isColumn(string columnName);
    void renameColumn(string fromColumnName, string toColumnName);
    void print();
    void makePermanent();
    bool isPermanent();
    void getNextPage(Cursor *cursor);
    void sortTable(vector<int> columnIndexes, vector<SortingStrategy> sortingStrategies, int bufferSize = 3);

    Cursor getCursor();
    int getColumnIndex(string columnName);
    string getIndexedColumn();
    void unload();


    /**
 * @brief Static function that takes a vector of valued and prints them out in a
 * comma seperated format.
 *
 * @tparam T current usaages include int and string
 * @param row 
 */
    template <typename T>
    void writeRow(vector<T> row, ostream &fout) {
        logger.log("Table::printRow");
        for (int columnCounter = 0; columnCounter < row.size(); columnCounter++) {
            if (columnCounter != 0)
                fout << ", ";
            fout << row[columnCounter];
        }
        fout << endl;
    }

    /**
 * @brief Static function that takes a vector of valued and prints them out in a
 * comma seperated format.
 *
 * @tparam T current usaages include int and string
 * @param row 
 */
    template <typename T>
    void writeRow(vector<T> row) {
        logger.log("Table::printRow");
        ofstream fout(this->sourceFileName, ios::app);
        this->writeRow(row, fout);
        fout.close();
    }
};

#endif