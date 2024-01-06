#ifndef __MATRIX_H
#define __MATRIX_H

#include "bufferManager.h"

#define ROWS_AT_ONCE 5

class Matrix {
   private:
    void initCalc(const string& line);
    vector<int> readLine(const string& line, int colNo);
    void writeLine(const vector<vector<int>>& line, ofstream& fout);

   public:
    string sourceFileName = "";
    string matrixName = "";
    long long int dimension; 
    long long int blockCount; 

    bool blockify();
    Matrix();
    Matrix(const string& tableName);
    
    bool load();
    void makePermanent();
    void unload();
    void transpose(bool printFlag);
    void printMatrix();
    void renameMatrix(string newMatrixName);
    void checksymmetry();
    void compute();
    Matrix* matrixCopier(string finalName);
};

#endif