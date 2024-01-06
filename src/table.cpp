#include "global.h"
// #include "syntacticParser.h"

/**
 * @brief Construct a new Table:: Table object
 *
 */
Table::Table()
{
	logger.log("Table::Table");
}

/**
 * @brief Construct a new Table:: Table object used in the case where the data
 * file is available and LOAD command has been called. This command should be
 * followed by calling the load function;
 *
 * @param tableName
 */
Table::Table(string tableName)
{
	logger.log("Table::Table");
	this->sourceFileName = "../data/" + tableName + ".csv";
	this->tableName = tableName;
}

/**
 * @brief Construct a new Table:: Table object used when an assignment command
 * is encountered. To create the table object both the table name and the
 * columns the table holds should be specified.
 *
 * @param tableName
 * @param columns
 */
Table::Table(string tableName, vector<string> columns)
{
	logger.log("Table::Table");
	this->sourceFileName = "../data/temp/" + tableName + ".csv";
	this->tableName = tableName;
	this->columns = columns;
	this->columnCount = columns.size();
	this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (sizeof(int) * columnCount));
	// this->maxRowsPerBlock = 3; // DEBUG PURPOSE ONLY
	this->smallestInColumns = vector<int>(this->columnCount);
	this->largestInColumns = vector<int>(this->columnCount);
}

/**
 * @brief The load function is used when the LOAD command is encountered. It
 * reads data from the source file, splits it into blocks and updates table
 * statistics.
 *
 * @return true if the table has been successfully loaded
 * @return false if an error occurred
 */
bool Table::load()
{
	logger.log("Table::load");
	fstream fin(this->sourceFileName, ios::in);
	string line;
	if (getline(fin, line))
	{
		fin.close();
		if (this->extractColumnNames(line))
			if (this->blockify())
				return true;
	}
	fin.close();
	return false;
}

/**
 * @brief Function extracts column names from the header line of the .csv data
 * file.
 *
 * @param line
 * @return true if column names successfully extracted (i.e. no column name
 * repeats)
 * @return false otherwise
 */
bool Table::extractColumnNames(string firstLine)
{
	logger.log("Table::extractColumnNames");
	unordered_set<string> columnNames;
	string word;
	stringstream s(firstLine);
	while (getline(s, word, ','))
	{
		word.erase(std::remove_if(word.begin(), word.end(), ::isspace), word.end());
		if (columnNames.count(word))
			return false;
		columnNames.insert(word);
		this->columns.emplace_back(word);
	}
	this->columnCount = this->columns.size();
	this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (sizeof(int) * this->columnCount));
	// this->maxRowsPerBlock = 3; // DEBUG
	return true;
}

/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size.
 *
 * @return true if successfully blockified
 * @return false otherwise
 */
bool Table::blockify()
{
	logger.log("Table::blockify");
	ifstream fin(this->sourceFileName, ios::in);
	string line, word;
	vector<int> row(this->columnCount, 0);
	vector<vector<int>> rowsInPage(this->maxRowsPerBlock, row);
	int pageCounter = 0;
	this->smallestInColumns = vector<int>(this->columnCount);
	this->largestInColumns = vector<int>(this->columnCount);
	getline(fin, line);
	while (getline(fin, line))
	{
		stringstream s(line);
		for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
		{
			if (!getline(s, word, ','))
				return false;
			row[columnCounter] = stoi(word);
			rowsInPage[pageCounter][columnCounter] = row[columnCounter];
		}
		pageCounter++;
		this->updateStatistics(row);
		if (pageCounter == this->maxRowsPerBlock)
		{
			bufferManager.writeTablePage(this->tableName, this->blockCount, rowsInPage, pageCounter);
			this->blockCount++;
			this->rowsPerBlockCount.emplace_back(pageCounter);
			pageCounter = 0;
		}
	}
	if (pageCounter)
	{
		bufferManager.writeTablePage(this->tableName, this->blockCount, rowsInPage, pageCounter);
		this->blockCount++;
		this->rowsPerBlockCount.emplace_back(pageCounter);
		pageCounter = 0;
	}

	if (this->rowCount == 0)
		return false;

	return true;
}

/**
 * @brief Given a row of values, this function will update the statistics it
 * stores i.e. it updates the number of rows that are present in the column and
 * the number of distinct values present in each column. These statistics are to
 * be used during optimisation.
 *
 * @param row
 */
void Table::updateStatistics(vector<int> row)
{
	logger.log("Table::updateStatistics");
	this->rowCount++;
	for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
	{
		if (row[columnCounter] < smallestInColumns[columnCounter])
			smallestInColumns[columnCounter] = row[columnCounter];
		if (row[columnCounter] > largestInColumns[columnCounter])
			largestInColumns[columnCounter] = row[columnCounter];
	}
}

/**
 * @brief Checks if the given column is present in this table.
 *
 * @param columnName
 * @return true
 * @return false
 */
bool Table::isColumn(string columnName)
{
	logger.log("Table::isColumn");
	for (auto col : this->columns)
	{
		if (col == columnName)
		{
			return true;
		}
	}
	return false;
}

/**
 * @brief Renames the column indicated by fromColumnName to toColumnName. It is
 * assumed that checks such as the existence of fromColumnName and the non prior
 * existence of toColumnName are done.
 *
 * @param fromColumnName
 * @param toColumnName
 */
void Table::renameColumn(string fromColumnName, string toColumnName)
{
	logger.log("Table::renameColumn");
	for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
	{
		if (columns[columnCounter] == fromColumnName)
		{
			columns[columnCounter] = toColumnName;
			break;
		}
	}
	return;
}

/**
 * @brief Returns the name of the indexed column. For external use, member methods are
 * advised to directly access the internal data.
 *
 * @return indexed column name
 */
string Table::getIndexedColumn()
{
	return "";
}

/**
 * @brief Function prints the first few rows of the table. If the table contains
 * more rows than PRINT_COUNT, exactly PRINT_COUNT rows are printed, else all
 * the rows are printed.
 *
 */
void Table::print()
{
	logger.log("Table::print");
	uint count = min((long long)PRINT_COUNT, this->rowCount);

	this->writeRow(this->columns, cout);

	if (this->indexingStrategy == NOTHING)
	{
		Cursor cursor(this->tableName, 0);
		vector<int> row;
		for (int rowCounter = 0; rowCounter < count; rowCounter++)
		{
			row = cursor.getNext();
			this->writeRow(row, cout);
		}
	}

	printRowCount(this->rowCount);
}

/**
 * @brief This function checks if cursor pageIndex is less than the table pageIndex,
 * and calls the nextPage function of the cursor with incremented pageIndex.
 *
 * @param cursor
 * @return vector<int>
 */
void Table::getNextPage(Cursor *cursor)
{
	logger.log("Table::getNextPage");

	if (cursor->pageIndex < (int)this->blockCount - 1)
	{
		cursor->nextPage(cursor->pageIndex + 1);
	}
}

/**
 * @brief called when EXPORT command is invoked to move source file to "data"
 * folder.
 *
 */
void Table::makePermanent()
{
	logger.log("Table::makePermanent");
	if (!this->isPermanent())
		bufferManager.deleteFile(this->sourceFileName);
	string newSourceFile = "../data/" + this->tableName + ".csv";
	ofstream fout(newSourceFile, ios::out);

	this->writeRow(this->columns, fout);
	Cursor cursor;

	if (!this->indexed)
		cursor = this->getCursor();

	vector<int> row;
	for (long long rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
	{
		if (!this->indexed)
			row = cursor.getNext();
		this->writeRow(row, fout);
	}
	fout.close();
}

/**
 * @brief Function to check if table is already exported
 *
 * @return true if exported
 * @return false otherwise
 */
bool Table::isPermanent()
{
	logger.log("Table::isPermanent");
	if (this->sourceFileName == "../data/" + this->tableName + ".csv")
		return true;
	return false;
}

/**
 * @brief The unload function removes the table from the database by deleting
 * all temporary files created as part of this table
 *
 */
void Table::unload()
{
	logger.log("Table::~unload");
	if (this->indexingStrategy == NOTHING)
	{
		for (int pageCounter = 0; pageCounter < this->blockCount; pageCounter++)
			bufferManager.deleteTableFile(this->tableName, pageCounter);
		if (!isPermanent())
			bufferManager.deleteFile(this->sourceFileName);
	}
}

/**
 * @brief Function that returns a cursor that reads rows from this table
 *
 * @return Cursor
 */
Cursor Table::getCursor()
{
	logger.log("Table::getCursor");
	Cursor cursor(this->tableName, 0);
	return cursor;
}

/**
 * @brief Function that returns the index of column indicated by columnName
 *
 * @param columnName
 * @return int
 */
int Table::getColumnIndex(string columnName)
{
	logger.log("Table::getColumnIndex");
	for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
	{
		if (this->columns[columnCounter] == columnName)
			return columnCounter;
	}
}

// bool sortComparator1(const vector<int> &a, const vector<int> &b)
// {
// 	for (int i = 0; i < parsedQuery.sortColumnIndexList.size(); i++)
// 	{
// 		if (a[parsedQuery.sortColumnIndexList[i]] != b[parsedQuery.sortColumnIndexList[i]])
// 		{
// 			if (parsedQuery.sortingStrategyList[i] == ASC)
// 				return a[parsedQuery.sortColumnIndexList[i]] < b[parsedQuery.sortColumnIndexList[i]];
// 			else
// 				return a[parsedQuery.sortColumnIndexList[i]] >= b[parsedQuery.sortColumnIndexList[i]];
// 		}
// 	}

// 	return false;
// }

// bool sortComparator2(const pair<vector<int>, int> &a, const pair<vector<int>, int> &b)
// {
// 	for (int i = 0; i < parsedQuery.sortColumnIndexList.size(); i++)
// 	{
// 		if (a.first[parsedQuery.sortColumnIndexList[i]] != b.first[parsedQuery.sortColumnIndexList[i]])
// 		{
// 			if (parsedQuery.sortingStrategyList[i] == ASC)
// 				return a.first[parsedQuery.sortColumnIndexList[i]] >= b.first[parsedQuery.sortColumnIndexList[i]];
// 			else
// 				return a.first[parsedQuery.sortColumnIndexList[i]] < b.first[parsedQuery.sortColumnIndexList[i]];
// 		}
// 	}
// 	return false;
// }

void Table::sortTable(vector<int> columnIndexes, vector<SortingStrategy> sortingStrategies, int bufferSize)
{
	this->rowsPerBlockCount.resize(this->blockCount * 4, 0);

	int runSize = this->maxRowsPerBlock * bufferSize;

	vector<int> row;
	Cursor cursor;

	if (!this->indexed)
	{
		cursor = this->getCursor();
		row = cursor.getNext();
	}

	int totalRuns = 0;
	int numNewBlocks = 0;

	auto sortComp1 = [&](const vector<int> &a, const vector<int> &b)
	{
		for (int i = 0; i < columnIndexes.size(); i++)
		{
			if (a[columnIndexes[i]] != b[columnIndexes[i]])
			{
				if (sortingStrategies[i] == ASC)
					return a[columnIndexes[i]] < b[columnIndexes[i]];
				else
					return a[columnIndexes[i]] >= b[columnIndexes[i]];
			}
		}
		return false;
	};

	auto sortComp2 = [&](const pair<vector<int>, int> &a, const pair<vector<int>, int> &b)
	{
		for (int i = 0; i < columnIndexes.size(); i++)
		{
			if (a.first[columnIndexes[i]] != b.first[columnIndexes[i]])
			{
				if (sortingStrategies[i] == ASC)
					return a.first[columnIndexes[i]] >= b.first[columnIndexes[i]];
				else
					return a.first[columnIndexes[i]] < b.first[columnIndexes[i]];
			}
		}
		return false;
	};

	while (!row.empty())
	{
		int numRowsRead = 0;
		vector<vector<int>> rows;
		while (!row.empty() && numRowsRead < runSize)
		{
			rows.push_back(row);
			row = cursor.getNext();
			numRowsRead++;
		}

		if (rows.size())
		{
			sort(rows.begin(), rows.end(), sortComp1);
		}

		int numRowsWritten = 0;
		vector<vector<int>> sortedRows;
		for (int i = 0; i < rows.size(); i++)
		{
			sortedRows.push_back(rows[i]);

			if (sortedRows.size() == this->maxRowsPerBlock)
			{
				this->rowsPerBlockCount[this->blockCount + numNewBlocks] = sortedRows.size();
				bufferManager.writeTablePage(this->tableName, this->blockCount + numNewBlocks, sortedRows, sortedRows.size());
				numNewBlocks++;
				sortedRows.clear();
			}
		}

		if (!sortedRows.empty())
		{
			this->rowsPerBlockCount[this->blockCount + numNewBlocks] = sortedRows.size();
			bufferManager.writeTablePage(this->tableName, this->blockCount + numNewBlocks, sortedRows, sortedRows.size());
			numNewBlocks++;
		}

		totalRuns++;
	}

	for (int i = 0; i < numNewBlocks; i++)
	{
		vector<vector<int>> rows = bufferManager.getTablePage(this->tableName, this->blockCount + i).data;
		bufferManager.writeTablePage(this->tableName, i, rows, rows.size());
		bufferManager.deleteTableFile(this->tableName, this->blockCount + i);
	}

	this->blockCount = numNewBlocks;
	this->rowsPerBlockCount.erase(this->rowsPerBlockCount.begin() + numNewBlocks, this->rowsPerBlockCount.end());
	this->rowsPerBlockCount.resize(this->blockCount * 20, 0);

	/*
		Sorting Done
		Now, the table has individually sorted pages in place of the original pages.
	*/

	/*
		Merging starts
	*/

	// cout<<"Number of blocks after internal sorting "<<table->blockCount<<endl;

	// m == totalRuns
	// p == totalPasses
	uint totalPasses = ceil(log(this->blockCount) / log(bufferSize - 1));

	// j == newSubFilesCount
	uint newSubFilesCount = this->blockCount;

	int mergeDegree = min(bufferSize - 1, (int)this->blockCount);
	// cout<<"Merge Degree "<<mergeDegree<<endl;

	int currPass = 1;

	// q (number of subfiles to write in this path)
	int numSubfileToWrite = ceil((double)newSubFilesCount / (bufferSize - 1));

	int maxSubfileSize = 1;

	int numBlocksTillNow = this->blockCount;
	int blocksAdded = 0;

	while (currPass <= totalPasses)
	{
		blocksAdded = 0;
		// cout<<"Number of runs (j) "<<newSubFilesCount<<endl;
		// cout<<"Q "<<numSubfileToWrite<<endl;
		// cout<<"Max subfile size "<<maxSubfileSize<<endl;

		int subfileLastBlockIndex = 0;
		// cout<<"Pass "<<currPass<<endl;
		for (int subFileIndex = 0; subFileIndex < numSubfileToWrite; subFileIndex++)
		{
			// cout<<"New day"<<endl;
			// Read next buffersize - 1 subfiles one block at a time
			// Merge and write as new subfile one block at a time

			int numSubfilesToRead = min(mergeDegree, (int)newSubFilesCount - subFileIndex);

			vector<vector<vector<int>>> currentBuffer(numSubfilesToRead);

			vector<pair<int, int>> subfileFirstLastBlockIndex(numSubfilesToRead);

			vector<vector<int>> mergedData;

			priority_queue<pair<vector<int>, int>, vector<pair<vector<int>, int>>, function<bool(const pair<vector<int>, int> &a, const pair<vector<int>, int> &b)>> heap(sortComp2);

			vector<int> nextRowToBePushedFromSubFile(numSubfilesToRead, 1);

			// Loading the buffer
			for (int j = 0; j < numSubfilesToRead; j++)
			{
				int currBlockIndex = j + subfileLastBlockIndex;
				subfileLastBlockIndex = min(currBlockIndex + maxSubfileSize - 1, (int)this->blockCount - 1);

				if (currBlockIndex < this->blockCount)
				{
					currentBuffer[j] = bufferManager.getTablePage(this->tableName, currBlockIndex).data;
					subfileFirstLastBlockIndex[j] = {currBlockIndex, min(currBlockIndex + maxSubfileSize - 1, (int)this->blockCount - 1)};
					heap.push({currentBuffer[j][0], j});
				}
			}
			subfileLastBlockIndex++;

			// cout<<endl<<endl;
			// cout<<"Heap starts"<<endl;

			while (!heap.empty())
			{
				pair<vector<int>, int> top = heap.top();

				heap.pop();

				mergedData.push_back(top.first);

				if (mergedData.size() == this->maxRowsPerBlock)
				{
					// cout<<"Ghusa 1"<<endl;
					this->rowsPerBlockCount[this->blockCount + blocksAdded] = mergedData.size();
					bufferManager.writeTablePage(this->tableName, this->blockCount + blocksAdded, mergedData, mergedData.size());
					// cout<<"Okay"<<endl;
					mergedData.clear();
					blocksAdded++;
					// cout<<"nhi fata 1"<<endl;
				}

				if (nextRowToBePushedFromSubFile[top.second] != this->maxRowsPerBlock && nextRowToBePushedFromSubFile[top.second] < currentBuffer[top.second].size())
				{
					// cout<<"Ghusa 2"<<endl;
					// cout<<currentBuffer[top.second].size()<<" "<<nextRowToBePushedFromSubFile[top.second]<<endl;
					// cout<<"Pushing from 1st if: ";
					// for(auto x: currentBuffer[top.second][nextRowToBePushedFromSubFile[top.second]]) cout<<x<<" ";
					// cout<<endl;
					heap.push({currentBuffer[top.second][nextRowToBePushedFromSubFile[top.second]], top.second});
					nextRowToBePushedFromSubFile[top.second]++;

					// cout<<"nhi fata 2"<<endl;
				}
				else if (subfileFirstLastBlockIndex[top.second].first != subfileFirstLastBlockIndex[top.second].second)
				{
					// cout<<"Ghusa 3"<<endl;
					subfileFirstLastBlockIndex[top.second].first++;
					currentBuffer[top.second] = bufferManager.getTablePage(this->tableName, subfileFirstLastBlockIndex[top.second].first).data;

					// cout<<"Pushing from 1nd if: ";
					// for(auto x: currentBuffer[top.second][0])cout<<x<<" ";
					// cout<<endl;

					heap.push({currentBuffer[top.second][0], top.second});
					nextRowToBePushedFromSubFile[top.second] = 1;
					// cout<<"nhi fata 3"<<endl;
				}
			}

			if (!mergedData.empty())
			{
				this->rowsPerBlockCount[this->blockCount + blocksAdded] = mergedData.size();
				bufferManager.writeTablePage(this->tableName, this->blockCount + blocksAdded, mergedData, mergedData.size());
				mergedData.clear();
				blocksAdded++;
			}
		}

		// cout<<"Blocks added: "<<blocksAdded<<endl;

		// Update maximum subfile size
		newSubFilesCount = numSubfileToWrite;
		// update number of subfiles to write
		numSubfileToWrite = ceil((double)newSubFilesCount / (bufferSize - 1));

		maxSubfileSize = min(((int)bufferSize - 1) * maxSubfileSize, (int)this->blockCount);
		
		for (int i = 0; i < blocksAdded; i++)
		{
			vector<vector<int>> currRows = bufferManager.getTablePage(this->tableName, this->blockCount + i).data;
			bufferManager.writeTablePage(this->tableName, i, currRows, currRows.size());
			bufferManager.deleteTableFile(this->tableName, this->blockCount + i);
		}

		this->blockCount = blocksAdded;
		this->rowsPerBlockCount.erase(this->rowsPerBlockCount.begin() + blocksAdded, this->rowsPerBlockCount.end());

		currPass++;
	}

	// this->print();

	return;
}
