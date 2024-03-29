#include "global.h"

BufferManager::BufferManager() {
    logger.log("BufferManager::BufferManager");
}

/**
 * @brief Function called to read a page from the buffer manager. If the page is
 * not present in the pool, the page is read and then inserted into the pool.
 *
 * @param tableName 
 * @param pageIndex 
 * @return TablePage 
 */
TablePage BufferManager::getTablePage(string tableName, int pageIndex) {
    logger.log("BufferManager::getTablePage");
    string pageName = TABLE_PAGE_NAME(tableName, pageIndex);
    if (this->inPool(pageName))
        return get<TablePage>(this->getFromPool(pageName));
    else
        return this->insertTablePageIntoPool(tableName, pageIndex);
}

/**
 * @brief Function called to read a matrix page from the buffer manager. If the page is
 * not present in the pool, the page is read and then inserted into the pool.
 *
 * @param tableName 
 * @param pageIndex 
 * @return TablePage 
 */
MatrixPage BufferManager::getMatrixPage(const string& matrixName, int rowIndex, int colIndex) {
    logger.log("BufferManager::getMatrixPage");

    string pageName = MATRIX_PAGE_NAME(matrixName, rowIndex, colIndex);
    if (this->inPool(pageName)) {
        return get<MatrixPage>(this->getFromPool(pageName));
    } else {
        return this->insertMatrixPageIntoPool(matrixName, rowIndex, colIndex);
    }
}


/**
 * @brief Checks to see if a page exists in the pool
 *
 * @param pageName 
 * @return true 
 * @return false 
 */
bool BufferManager::inPool(string pageName) {
    logger.log("BufferManager::inPool");
    for (auto page : this->pages) {
        if (pageName == getPageName(page))
            return true;
    }
    return false;
}

/**
 * @brief If the page is present in the pool, then this function returns the
 * page. Note that this function will fail if the page is not present in the
 * pool.
 *
 * @param pageName 
 * @return TablePage 
 */
Pages BufferManager::getFromPool(string pageName) {
    logger.log("BufferManager::getFromPool");
    for (auto page : this->pages)
        if (pageName == getPageName(page))
            return page;
}



/**
 * @brief Inserts page indicated by tableName and pageIndex into pool. If the
 * pool is full, the pool ejects the oldest inserted page from the pool and adds
 * the current page at the end. It naturally follows a queue data structure. 
 *
 * @param tableName 
 * @param pageIndex 
 * @return TablePage 
 */
TablePage BufferManager::insertTablePageIntoPool(string tableName, int pageIndex) {
    logger.log("BufferManager::insertIntoPool");
    TablePage page(tableName, pageIndex);

#ifdef DEFER_WRITE
    this->push(page);
#else
    this->push(page, false);
#endif

    return page;
}

/**
 * @brief Inserts linear hashed page indicated by matrixName, rowIndex and colIndex into pool.
 * If the pool is full, the pool ejects the oldest inserted page from the pool and adds
 * the current page at the end. It naturally follows a queue data structure. 
 *
 * @param matrixName
 * @param rowIndex
 * @param colIndex
 *  
 * @return TablePage 
 */
MatrixPage BufferManager::insertMatrixPageIntoPool(const string& matrixName, int rowIndex, int colIndex) {
    logger.log("BufferManager::insertMatrixPageIntoPool");

    MatrixPage page(matrixName, rowIndex, colIndex);

#ifdef DEFER_WRITE
    this->push(page);
#else
    this->push(page, false);
#endif

    return page;
}

/**
 * @brief The buffer manager is also responsible for writing pages. This is
 * called when new tables are created using assignment statements.
 *
 * @param tableName 
 * @param pageIndex 
 * @param rows 
 * @param rowCount 
 */
void BufferManager::writeTablePage(string tableName, int pageIndex, vector<vector<int>> rows, int rowCount) {
    logger.log("BufferManager::writeTablePage");
    TablePage page(tableName, pageIndex, rows, rowCount);

#ifdef DEFER_WRITE
    this->push(page);
#else
    this->push(page, false);
#endif
}


void BufferManager::writeMatrixPage(const string& matrixName, int rowIndex, int colIndex, const vector<vector<int>>& data) {
    logger.log("BufferManager::writeMatrixPage");
    MatrixPage page(matrixName, rowIndex, colIndex, data);

#ifdef DEFER_WRITE
    this->push(page);
#else
    this->push(page, false);
#endif
}

/**
 * @brief Deletes file names fileName
 *
 * @param fileName 
 */
void BufferManager::deleteFile(string fileName) {
    if (remove(fileName.c_str()))
        logger.log("BufferManager::deleteFile: Err");
    else
        logger.log("BufferManager::deleteFile: Success");
}

/**
 * @brief Overloaded function that calls deleteFile(fileName) by constructing
 * the fileName from the tableName and pageIndex.
 *
 * @param tableName 
 * @param pageIndex 
 */
void BufferManager::deleteTableFile(string tableName, int pageIndex) {
    logger.log("BufferManager::deleteTableFile");
    string fileName = TABLE_PAGE_NAME(tableName, pageIndex);
    this->pop(fileName);
    this->deleteFile(fileName);
}
/**
 * @brief Overloaded function that calls deleteFile(fileName) by constructing
 * the fileName from the matrixName and rowIndex and colIndex.
 *
 * @param matrixName 
 * @param rowIndex 
 * @param colIndex
 */
void BufferManager::deleteMatrixFile(const string& matrixName, int rowIndex, int colIndex) {
    logger.log("BufferManager::deleteMatrixFile");

    string fileName = MATRIX_PAGE_NAME(matrixName, rowIndex, colIndex);
    this->pop(fileName);
    this->deleteFile(fileName);
}
/**
 * @brief Overloaded function that calls deleteFile(fileName) by constructing
 * the fileName from the tableName and bucket and chainCount.
 *
 * @param tableName 
 * @param bucket
 * @param chainCount 
 */

/**
 * @brief Pushes the page into bufferManager
 *
 * @param page 
 * @param deferWrite 
 */
void BufferManager::push(Pages page, bool deferWrite) {
    if (!deferWrite) {
        writePage(page);  
    }

    for (auto &p : this->pages) {
        if (getPageName(p) == getPageName(page)) {
            p = page;
            return;
        }
    }

    if (this->pages.size() >= BLOCK_COUNT && !this->pages.empty()) { 
        if (deferWrite) {
            writePage(this->pages.front()); // wasn't written earlier, because deferred
        }
        this->pages.pop_front();
    }

    this->pages.emplace_back(page);
}
/**
 * @brief Pops the page from bufferManager
 *
 * @param page 
 * @param deferWrite 
 */
void BufferManager::pop(const string& fileName) {
       
    for (int i = 0; i < this->pages.size(); i++) {
        if (getPageName(this->pages[i]) == fileName) {
            this->pages.erase(this->pages.begin() + i);
            return;
        }
    }
}