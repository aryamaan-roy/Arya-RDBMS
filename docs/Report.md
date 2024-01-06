# Welcome !

## Page Design and Implementation

1. <b> Block Size </b> : 8 KB. So based on our calculations, as 45*45 matrix can fit in a page. 
Formula = sqrt(Block size * 1024 / sizeof(int)) sqrt(8*1024/4)= 45.25. So we have taken 45 as the block size.

2. Matrix Page also contains Matrix Name, Number of Rows, Number of Columns, Number of Pages, as well as the 45*45 matrix (at max). 

3. getMatrix is used to read and writePage is used to write the matrix to the page.

## LOAD Command 

1. LOAD command is used to load the matrix from the file to the database.

2. The load functionality interanlly calls the blockify() function which extracts the matrix from the file matrix_name.csv in data folder.

3. The intCalc() function calculated the number of blocks that would be required.

4. Then it created a new matrix object and then writes into as many pages required sequentially using writePage() function.

## TRANSPOSE MATRIX command

1. The transpose command is used to transpose the matrix.

2. The transpose() function in matrix.cpp essentially extracts 2 blocks at a time from the matrix and then swaps the elements and then writes those 2 blocks back. Thus the operation is in-place.

## PRINT MATRIX command

1. The print matrix command takes the matrix name as the input and then sequentially accesses all blocks whose matrix name matches the input matrix name and then prints the each page in such a way that the entire matrix is printed.

2. The rows and columns and limited to 20 each.

## CHECKSYMMETRY command

1. The implementation of the CheckSymmetry() is similar to that of the transpose() function in matrix.cpp.

2. The only difference is that the elements are compared with the corresponding elements (rather than swapping them) in the transpose matrix and if they are not equal then the matrix is not symmetric.

## RENAME command 

1. A new matrix object is created with the new name. Some properties of the old matrix are copied to the new matrix object, the number of rows, the number of columns (dimension). Matrix name and source file path are changed.

2. Then page by page, all contents of the old matrix are copied to the new matrix.

3. The old matrix is then deleted from the working memory including the pages.

## EXPORT command

1. The export command is used to export the matrix to a file.

2. The export command takes the matrix name as the input and then sequentially accesses all blocks whose matrix name matches the input matrix name and then writes the matrix to the file matrix_name.csv in data folder.

3. It utilizes the makePermanent() function in matrix.cpp which opens a file in the data folder and then sequentially writes all pages of that matrix to the file.

## COMPUTE command

1. COMPUTE A basically creates a new matrix A_RESULT = A - A_TRANSPOSE.

2. A new matrix is formed with the name of the result matrix. It is intially initialed with all the properties of the first matrix (A). 

3. Then a transpose() operation is done on the result matrix. Thus now the result matrix is A_TRANSPOSE.

4. Then all pages of the A_RESULT matrix is accessed sequentially, 2 pages at a time. Each matrix element in the concerning 2 pages is replaced by A - A_TRANSPOSE.

## ERROR HANDLING

1. Syntactic error handling is done for all the commands by checking the number of arguments in the parsed query replicating the already implemented functionality.

2. Semantic error handling is done for all the commands by checking the validity of the arguments in the parsed query replicating the already implemented functionality.

3. Additional error handling was implemented wherever necessary.
