#include "global.h"
#include "matrix.h"

int num_read_blockify = 0;
int num_write_blockify = 0;

int num_read_transpose = 0;
int num_write_transpose = 0;

int num_read_copier = 0;
int num_write_copier = 0;

Matrix::Matrix() {
    logger.log("Matrix::Matrix");
}

Matrix::Matrix(const string& matrixName) {
    logger.log("Matrix::Matrix");
    this->sourceFileName = "../data/" + matrixName + ".csv";
    this->matrixName = matrixName;
}

bool Matrix::load() {
    logger.log("Matrix::load");
    bool output = this->blockify();
    cout<<"Number of blocks read: "<<num_read_blockify<<endl;
    cout<<"Number of blocks written: "<<num_write_blockify<<endl;
    cout<<"Number of blocks accessed: "<<num_read_blockify + num_write_blockify<<endl;
    return output;    
}

vector<int> Matrix::readLine(const string& line, int colNo) {
    
    stringstream s(line);
    int j = 0;
    string word;
    
    for (; j < colNo * MATRIX_PAGE_DIM; j++) {
        if (!getline(s, word, ',')) {
            return vector<int> (0);
        }
    }

    vector<int> out;

    for (; j < min(this->dimension, (long long int)(colNo + 1) * MATRIX_PAGE_DIM); j++) {
        if (!getline(s, word, ',')) {
            return vector<int> (0);
        }
        word = trim(word);
        out.push_back(stoi(word));
    }

    while(out.size() < MATRIX_PAGE_DIM) {
        out.push_back(-1);
    }

    return out;

}

bool Matrix::blockify() {
    logger.log("Matrix::blockify");
    
    ifstream fin(this->sourceFileName, ios::in);

    if (fin.peek() == ifstream::traits_type::eof()) {
        cout << "OPERATIONAL ERROR: File is empty." << endl;
        return false;
    }

    string firstline;
    getline(fin, firstline);
    this->initCalc(firstline);

    fin.close();

    // int num_read_blocks = 0;
    // int num_written_blocks = 0;

    num_read_blockify = 0;
    num_write_blockify = 0;

    for (int block_j = 0; block_j < this->blockCount; block_j++) {
        ifstream fin(this->sourceFileName, ios::in);

        int block_i = 0;
        int in_this_block = 0;
        vector<vector<int>> this_page (MATRIX_PAGE_DIM, vector<int> (MATRIX_PAGE_DIM, -1));

        string line;
        while (getline(fin, line)) {
            if (in_this_block == MATRIX_PAGE_DIM) {

                bufferManager.writeMatrixPage(this->matrixName, block_i, block_j, this_page);
                num_write_blockify++;

                this_page.assign(MATRIX_PAGE_DIM, vector<int> (MATRIX_PAGE_DIM, -1));
                block_i++;
                in_this_block = 0;
            }

            this_page[in_this_block] = this->readLine(line, block_j);
            in_this_block++;
        }

        bufferManager.writeMatrixPage(this->matrixName, block_i, block_j, this_page);


        fin.close();
    }

    // cout<<"Number of blocks read: "<<num_read_blocks<<endl;
    // cout<<"Number of blocks written: "<<num_written_blocks<<endl;
    // cout<<"Number of blocks accessed: "<<num_read_blocks + num_written_blocks<<endl;

    return true;
}

void Matrix::initCalc(const string& line) {
    logger.log("Matrix::initCalc");
    
    int count = 0;
    for (auto& c: line) {
        if (c == ',') {
            count++;
        }
    }
    if (line.back() != ',') {
        count++;
    }
    this->dimension = count;
    this->blockCount = ceil((double)this->dimension/(double)MATRIX_PAGE_DIM);
}

void Matrix::makePermanent() {
    logger.log("Matrix::makePermanent");

    int num_read_blocks = 0;
    int num_written_blocks = 0;

    ofstream fout(this->sourceFileName, ios::trunc);

    vector<vector<int>> this_page; // avoid multiple constructor and destructor calls
    for (long long int i = 0; i < this->dimension;) {

        long long int j = 0;
        
        int block_i = i / MATRIX_PAGE_DIM;
        int line_in_block = i % MATRIX_PAGE_DIM;

        int can_go_upto = min((long long int)ROWS_AT_ONCE, min((long long int)(this->dimension - i), (long long int)(MATRIX_PAGE_DIM - line_in_block)));

        vector<vector<int>> lines(can_go_upto);
        for (auto& line : lines) {
            line.reserve(this->dimension);
        }

        int block_j = 0;
        for (; block_j < this->blockCount - 1; block_j++) {
            this_page = bufferManager.getMatrixPage(this->matrixName, block_i, block_j).getMatrix();
            num_read_blocks++;
            
            for (int k = 0; k < can_go_upto; k++) {
                lines[k].insert(lines[k].end(), this_page[line_in_block + k].begin(), this_page[line_in_block + k].end());
            }

            j = j + MATRIX_PAGE_DIM;
        }

        this_page = bufferManager.getMatrixPage(this->matrixName, block_i, block_j).getMatrix();
        num_read_blocks++;

        for (int k = 0; k < can_go_upto; k++) {
            lines[k].insert(lines[k].end(), this_page[line_in_block + k].begin(), this_page[line_in_block + k].begin() + (this->dimension - j));
        }
        this->writeLine(lines, fout);
        i = i + can_go_upto;
    }

    cout<<"Number of blocks read: "<<num_read_blocks<<endl;
    cout<<"Number of blocks written: "<<num_written_blocks<<endl;
    cout<<"Number of blocks accessed: "<<num_read_blocks + num_written_blocks<<endl;

    fout.close();
}

void Matrix::writeLine(const vector<vector<int>>& lines, ofstream& fout) {
    logger.log("Matrix::writeLine");

    for (auto &line : lines) {
        for (long long int j = 0; j < line.size() - 1; j++) {
            fout << line[j] << ",";
        }
        fout << line.back() << endl;
    }
    
}

void Matrix::printMatrix()
{
    int num_read_blocks = this->blockCount * this->blockCount;
    int num_written_blocks = 0;

    int count = 0;
    for(int i = 0; i < this->blockCount; i++)
    {
        for(int k = 0; k < MATRIX_PAGE_DIM; k++)
        {
            count++;
            if(count > 20)
                break;

            int count2 = 0;
            // num_read_blocks++;

            for(int j = 0; j < this->blockCount; j++)
            {
                if(count2 > 20)
                    break;
                
                MatrixPage currentPage = bufferManager.getMatrixPage(this->matrixName, i, j);
                vector<vector<int>> currentMatrix = currentPage.getMatrix();
                
                if(currentMatrix[k][0] == -1)
                {
                    cout<<"Number of blocks read: "<<num_read_blocks<<endl;
                    cout<<"Number of blocks written: "<<num_written_blocks<<endl;
                    cout<<"Number of blocks accessed: "<<num_read_blocks + num_written_blocks<<endl;
                    return;
                }

                for(int l = 0; l < MATRIX_PAGE_DIM; l++)
                {
                    count2++;
                    if(currentMatrix[k][l] == -1)
                        break;
                    cout << currentMatrix[k][l] << " ";
                }
            }
            cout<<endl;
        }
        if(count > 20)
            break;
    }

    cout<<"Number of blocks read: "<<num_read_blocks<<endl;
    cout<<"Number of blocks written: "<<num_written_blocks<<endl;
    cout<<"Number of blocks accessed: "<<num_read_blocks + num_written_blocks<<endl;

}

void Matrix::renameMatrix(string newMatrixName)
{
    int num_read_blocks = 0;
    int num_written_blocks = 0;

    Matrix* newMatrix = new Matrix(newMatrixName);
    newMatrix->blockCount = this->blockCount;
    newMatrix->dimension = this->dimension;

    // newMatrix->sourceFileName = this->sourceFileName;
    // newMatrix->blockify();



    for(int i = 0; i < this->blockCount; i++)
    {
        for(int j = 0; j < this->blockCount; j++)
        {
            MatrixPage currentPage = bufferManager.getMatrixPage(this->matrixName, i, j);
            num_read_blocks++;
            vector<vector<int>> currentMatrix = currentPage.getMatrix();

            vector<vector<int>> matrixArr (MATRIX_PAGE_DIM, vector<int> (MATRIX_PAGE_DIM, -1));
            
            for(int k = 0; k < MATRIX_PAGE_DIM; k++)
            {
                for(int l = 0; l < MATRIX_PAGE_DIM; l++)
                {
                    matrixArr[k][l] = currentMatrix[k][l];
                }
            }

            bufferManager.writeMatrixPage(newMatrixName, i, j, matrixArr);
            num_written_blocks++;
        }
    }

    // num_read_blocks += num_read_blockify;
    // num_written_blocks += num_write_blockify;
    newMatrix->sourceFileName = "../data/" + newMatrixName + ".csv";
    matrixCatalogue.insertMatrix(newMatrix);
    matrixCatalogue.deleteMatrix(this->matrixName);

    cout<<"Number of blocks read: "<<num_read_blocks<<endl;
    cout<<"Number of blocks written: "<<num_written_blocks<<endl;
    cout<<"Number of blocks accessed: "<<num_read_blocks + num_written_blocks<<endl;


    // vector<MatrixPage*> addresses;
    // for(int i = 0; i < this->blockCount; i++)
    // {
    //     for(int j = 0; j < this->blockCount; j++)
    //     {
    //         MatrixPage* currentPage = bufferManager.getMatrixPage_Reference(this->matrixName, i, j);
    //         addresses.push_back(currentPage);
    //         // currentPage->matrixName = newMatrixName;
    //     }        
    // }

    // for(int i = 0; i<addresses.size(); i++)
    // {
    //     addresses[i]->matrixName = newMatrixName;
    // }
    // this->matrixName = newMatrixName;
}

void Matrix::transpose(bool printFlag)
{
    num_read_transpose = 0;
    num_write_transpose = 0;

    for (int rowIndex = 0; rowIndex < this->blockCount; rowIndex++)
    {
        for (int colIndex = rowIndex + 1; colIndex < this->blockCount; colIndex++)
        {
            MatrixPage currentPage = bufferManager.getMatrixPage(this->matrixName, rowIndex, colIndex);
            num_read_transpose++;
            vector<vector<int>> currentMatrix = currentPage.getMatrix();

            MatrixPage swapPage = bufferManager.getMatrixPage(this->matrixName, colIndex, rowIndex);
            num_read_transpose++;
            vector<vector<int>> swapMatrix = swapPage.getMatrix();

            for (int i = 0; i < MATRIX_PAGE_DIM; i++)
                for (int j = i + 1; j < MATRIX_PAGE_DIM; j++)
                {
                    swap(currentMatrix[i][j], currentMatrix[j][i]);
                    swap(swapMatrix[i][j], swapMatrix[j][i]);
                }

            bufferManager.writeMatrixPage(this->matrixName, rowIndex, colIndex, swapMatrix);
            num_write_transpose++;
            bufferManager.writeMatrixPage(this->matrixName, colIndex, rowIndex, currentMatrix);
            num_write_transpose++;
        }
    }

    for (int rowIndex = 0; rowIndex < this->blockCount; rowIndex++)
    {
        num_read_transpose++;
        MatrixPage currentPage = bufferManager.getMatrixPage(this->matrixName, rowIndex, rowIndex);
        vector<vector<int>> currentMatrix = currentPage.getMatrix();
        
        for (int i = 0; i < MATRIX_PAGE_DIM; i++)
            for (int j = i + 1; j < MATRIX_PAGE_DIM; j++)
                swap(currentMatrix[i][j], currentMatrix[j][i]);
        
        bufferManager.writeMatrixPage(this->matrixName, rowIndex, rowIndex, currentMatrix);
        num_write_transpose++;
    }

    if(printFlag)
    {
        cout<<"Number of blocks read: "<<num_read_transpose<<endl;
        cout<<"Number of blocks written: "<<num_write_transpose<<endl;
        cout<<"Number of blocks accessed: "<<num_read_transpose + num_write_transpose<<endl;
    }
}

void Matrix::checksymmetry()
{
    int num_read_blocks = 0;
    int num_written_blocks = 0;
    for (int rowIndex = 0; rowIndex < this->blockCount; rowIndex++)
    {
        for (int colIndex = rowIndex + 1; colIndex < this->blockCount; colIndex++)
        {
            num_read_blocks++;
            MatrixPage currentPage = bufferManager.getMatrixPage(this->matrixName, rowIndex, colIndex);
            vector<vector<int>> currentMatrix = currentPage.getMatrix();

            MatrixPage swapPage = bufferManager.getMatrixPage(this->matrixName, colIndex, rowIndex);
            num_read_blocks++;
            vector<vector<int>> swapMatrix = swapPage.getMatrix();

            for (int i = 0; i < MATRIX_PAGE_DIM; i++)
                for (int j = i + 1; j < MATRIX_PAGE_DIM; j++)
                {
                    if(currentMatrix[i][j] != swapMatrix[j][i] || currentMatrix[j][i] != swapMatrix[i][j])
                    {
                        cout<<"FALSE"<<endl;
                        return;
                    }
                }
        }

        cout<<"Number of blocks read: "<<num_read_blocks<<endl;
        cout<<"Number of blocks written: "<<num_written_blocks<<endl;
        cout<<"Number of blocks accessed: "<<num_read_blocks + num_written_blocks<<endl;
        cout<<"TRUE"<<endl;
        return;
    }
}

// Matrix* Matrix::matrixCopier(string finalName)
// {
//     for (int block_j = 0; block_j < this->blockCount; block_j++) {
//         int block_i = 0;
//         int in_this_block = 0;
//         vector<vector<int>> this_page (MATRIX_PAGE_DIM, vector<int> (MATRIX_PAGE_DIM, -1));



//         while (getline(fin, line)) {
//             if (in_this_block == MATRIX_PAGE_DIM) {

//                 bufferManager.writeMatrixPage(this->matrixName, block_i, block_j, this_page);
//                 num_write_blockify++;

//                 this_page.assign(MATRIX_PAGE_DIM, vector<int> (MATRIX_PAGE_DIM, -1));
//                 block_i++;
//                 in_this_block = 0;
//             }

//             this_page[in_this_block] = this->readLine(line, block_j);
//             in_this_block++;
//         }

//         bufferManager.writeMatrixPage(this->matrixName, block_i, block_j, this_page);


//         fin.close();
//     }

//     return true;



//     num_read_copier = 0;
//     num_write_copier = 0;

//     Matrix* newMatrix = new Matrix(finalName);
//     newMatrix->sourceFileName = "../data/" + finalName + ".csv";

//     for(int i = 0; i < this->blockCount; i++)
//     {
//         for(int j = 0; j < this->blockCount; j++)
//         {
//             MatrixPage currentPage = bufferManager.getMatrixPage(this->matrixName, i, j);
//             num_read_copier++;
//             vector<vector<int>> currentMatrix = currentPage.getMatrix();



//             bufferManager.writeMatrixPage(newMatrix->matrixName, i, j, currentMatrix);
//             num_write_copier++;
//         }
//     }

//     return newMatrix;
// }

void Matrix::compute()
{
    int num_read_blocks = 0;
    int num_written_blocks = 0;

    string newMatrixName = this->matrixName + "_RESULT";
    // Matrix* newMatrix = this;

    Matrix* newMatrix = new Matrix(newMatrixName);
    newMatrix->blockCount = this->blockCount;
    newMatrix->dimension = this->dimension;
    
    // newMatrix->sourceFileName = this->sourceFileName;
    // newMatrix->blockify();



    for(int i = 0; i < this->blockCount; i++)
    {
        for(int j = 0; j < this->blockCount; j++)
        {
            MatrixPage currentPage = bufferManager.getMatrixPage(this->matrixName, i, j);
            num_read_blocks++;
            vector<vector<int>> currentMatrix = currentPage.getMatrix();

            vector<vector<int>> matrixArr (MATRIX_PAGE_DIM, vector<int> (MATRIX_PAGE_DIM, -1));
            
            for(int k = 0; k < MATRIX_PAGE_DIM; k++)
            {
                for(int l = 0; l < MATRIX_PAGE_DIM; l++)
                {
                    matrixArr[k][l] = currentMatrix[k][l];
                }
            }

            bufferManager.writeMatrixPage(newMatrixName, i, j, matrixArr);
            num_written_blocks++;
        }
    }



    // Matrix* newMatrix = new Matrix(newMatrixName);
    // newMatrix->sourceFileName = this->sourceFileName;
    // newMatrix->blockify();

    // num_read_blocks += num_read_blockify;
    // num_written_blocks += num_write_blockify;
    
    // for(int i = 0; i < this->blockCount; i++)
    // {
    //     for(int j = 0; j < this->blockCount; j++)
    //     {
    //         MatrixPage currentPage = bufferManager.getMatrixPage(this->matrixName, i, j);
    //         num_read_blocks++;
    //         vector<vector<int>> currentMatrix = currentPage.getMatrix();

    //         bufferManager.writeMatrixPage(newMatrixName, i, j, currentMatrix);
    //     }
    // }


    newMatrix->sourceFileName = "../data/" + newMatrixName + ".csv";
    
    matrixCatalogue.insertMatrix(newMatrix);
    
    newMatrix->transpose(false);
    
    num_read_blocks += num_read_transpose;
    num_written_blocks += num_write_transpose;

   
    for(int rowIndex = 0; rowIndex < this->blockCount; rowIndex++)
    {
        for(int colIndex = 0; colIndex < this->blockCount; colIndex++)
        {
            MatrixPage currentPage = bufferManager.getMatrixPage(this->matrixName, rowIndex, colIndex);
            vector<vector<int>> currentMatrix = currentPage.getMatrix();
            num_read_blocks++;

            MatrixPage finalPage = bufferManager.getMatrixPage(newMatrixName, rowIndex, colIndex);
            vector<vector<int>> finalMatrix = finalPage.getMatrix();
            num_read_blocks++;

            for(int i = 0; i<MATRIX_PAGE_DIM; i++)
            {
                for(int j = 0; j<MATRIX_PAGE_DIM; j++)
                {
                    if(currentMatrix[i][j] == -1)
                        continue;
                    finalMatrix[i][j] = currentMatrix[i][j] - finalMatrix[i][j];
                }
            }

            bufferManager.writeMatrixPage(newMatrixName, rowIndex, colIndex, finalMatrix);
            num_written_blocks++;
        }
    }

    cout<<"Number of blocks read: "<<num_read_blocks<<endl;
    cout<<"Number of blocks written: "<<num_written_blocks<<endl;
    cout<<"Number of blocks accessed: "<<num_read_blocks + num_written_blocks<<endl;
}

// void Matrix::renameMatrix(string newMatrixName)
// {
//     this->matrixName = newMatrixName;
//     this->sourceFileName = "../data/" + newMatrixName + ".csv";
//     cout<<"Maujja hi maujja"<<endl;
// }

void Matrix::unload() {
    for (long long int i = 0; i < this->blockCount; i++) {
        for (long long int j = 0; j < this->blockCount; j++) {
            bufferManager.deleteMatrixFile(this->matrixName, i, j);
        }
    }
}