#include "global.h"
/**
 * @brief File contains method to process SORT commands.
 *
 * syntax:
 * SORT relation_name BY column_name1, column_name2, and so on IN sorting_order1, sorting_order2, and so on
 *
 * sorting_order = ASC | DESC
 */

bool syntacticParseSORT()
{
	logger.log("syntacticParseSORT");

	vector<string> colNames;
	int numColsInQuery = 0;
	int InPos = -1;

	for (int i = 3; i < tokenizedQuery.size(); i++)
	{
		if (tokenizedQuery[i] == "IN")
		{
			InPos = i;
			break;
		}
		colNames.push_back(tokenizedQuery[i]);
		numColsInQuery++;
	}

	if (tokenizedQuery.size() != 3 + numColsInQuery * 2 + 1 || tokenizedQuery[2] != "BY" || tokenizedQuery[InPos] != "IN")
	{
		cout << "SYNTAX ERROR" << endl;
		return false;
	}

	parsedQuery.queryType = SORT;

	parsedQuery.sortRelationName = tokenizedQuery[1];

	for (int i = 3; i < InPos; i++)
	{
		parsedQuery.sortColumnNameList.push_back(tokenizedQuery[i]);
		string sortingStrategy = tokenizedQuery[InPos + 1 + i - 3];
		if (sortingStrategy == "ASC")
			parsedQuery.sortingStrategyList.push_back(ASC);
		else if (sortingStrategy == "DESC")
			parsedQuery.sortingStrategyList.push_back(DESC);
		else
		{
			cout << "SYNTAX ERROR" << endl;
			return false;
		}
	}

	return true;
}

bool semanticParseSORT()
{
	logger.log("semanticParseSORT");

	if (!tableCatalogue.isTable(parsedQuery.sortRelationName))
	{
		cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
		return false;
	}

	for (int i = 0; i < parsedQuery.sortColumnNameList.size(); i++)
	{
		if (!tableCatalogue.isColumnFromTable(parsedQuery.sortColumnNameList[i], parsedQuery.sortRelationName))
		{
			cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
			return false;
		}
	}

	return true;
}

bool sortComparator1(const vector<int> &a, const vector<int> &b)
{
	for (int i = 0; i < parsedQuery.sortColumnIndexList.size(); i++)
	{
		if (a[parsedQuery.sortColumnIndexList[i]] != b[parsedQuery.sortColumnIndexList[i]])
		{
			if (parsedQuery.sortingStrategyList[i] == ASC)
				return a[parsedQuery.sortColumnIndexList[i]] < b[parsedQuery.sortColumnIndexList[i]];
			else
				return a[parsedQuery.sortColumnIndexList[i]] >= b[parsedQuery.sortColumnIndexList[i]];
		}
	}

	return false;
}

bool sortComparator2(const pair<vector<int>, int> &a, const pair<vector<int>, int> &b)
{
	for (int i = 0; i < parsedQuery.sortColumnIndexList.size(); i++)
	{
		if (a.first[parsedQuery.sortColumnIndexList[i]] != b.first[parsedQuery.sortColumnIndexList[i]])
		{
			if (parsedQuery.sortingStrategyList[i] == ASC)
				return a.first[parsedQuery.sortColumnIndexList[i]] >= b.first[parsedQuery.sortColumnIndexList[i]];
			else
				return a.first[parsedQuery.sortColumnIndexList[i]] < b.first[parsedQuery.sortColumnIndexList[i]];
		}
	}
	return false;
}

void executeSORT()
{
	logger.log("executeSORT");

	Table *table = tableCatalogue.getTable(parsedQuery.sortRelationName);

	vector<int> columnIndices;

	for (int i = 0; i < parsedQuery.sortColumnNameList.size(); i++)
		columnIndices.push_back(table->getColumnIndex(parsedQuery.sortColumnNameList[i]));

	table->sortTable(columnIndices, parsedQuery.sortingStrategyList);
	table->makePermanent();
	return;
}
