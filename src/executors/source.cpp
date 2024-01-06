#include "../global.h"
/**
 * @brief 
 * SYNTAX: SOURCE filename
 */
bool syntacticParseSOURCE()
{
    logger.log("syntacticParseSOURCE");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR (in SOURCE)" << endl;
        return false;
    }
    parsedQuery.queryType = SOURCE;
    parsedQuery.sourceFileName = tokenizedQuery[1];
    return true;
}

bool semanticParseSOURCE()
{
    logger.log("semanticParseSOURCE");
    if (!isQueryFile(parsedQuery.sourceFileName))
    {
        cout << "SEMANTIC ERROR (in SOURCE): File doesn't exist" << endl;
        return false;
    }
    return true;
}

void doCommand2()
{
    logger.log("doCommand");
    if (syntacticParse() && semanticParse())
        executeCommand();
    return;
}

void executeSOURCE()
{
    logger.log("executeSOURCE");

    regex delim("[^\\s,]+");

    fstream file;
    string file_name_with_ext = "../data/" + parsedQuery.sourceFileName + ".ra";

    file.open(file_name_with_ext, ios::in);

    if (file.is_open())
    {
        string curr_command;
        while(getline(file, curr_command))
        {
            tokenizedQuery.clear();
            parsedQuery.clear();
            logger.log("\nReading New Command: ");
            logger.log(curr_command);

            auto words_begin = std::sregex_iterator(curr_command.begin(), curr_command.end(), delim);
            auto words_end = std::sregex_iterator();

            for (std::sregex_iterator i = words_begin; i != words_end; ++i)
                tokenizedQuery.emplace_back((*i).str());

            if (tokenizedQuery.size() == 1 && tokenizedQuery.front() == "QUIT")
            {
                break;
            }

            if (tokenizedQuery.empty())
            {
                continue;
            }

            if (tokenizedQuery.size() == 1)
            {
                cout << "SYNTAX ERROR" << endl;
                continue;
            }

            doCommand2();

        }


        tokenizedQuery.clear();
        parsedQuery.clear();
        file.close();
    }
    else
    {
        cout << "SEMANTIC ERROR (in SOURCE): File doesn't exist" << endl;
    }

    return;
}
