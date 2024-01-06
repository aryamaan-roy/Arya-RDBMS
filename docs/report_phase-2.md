# PROJECT PHASE 2

## Team 53
- Divyansh Tiwari (2020111002)
- Aryamaan Basu Roy (2020101128)

## Introduction
This phase of the project was aimed towards implementing the 'sort' functionality of the database system making use of the external sorting algorithm. The external sorting algorithm is a variation of the merge sort algorithm which is used to sort data that is too large to fit into the main memory of the system. The external sorting algorithm makes use of the disk to store the data that cannot fit into the main memory. The data is divided into chunks that can fit into the main memory and then sorted using the merge sort algorithm. The sorted chunks are then merged to form the final sorted data. The external sorting algorithm is used in the database system to sort the data in the tables. The data in the tables is stored in the form of records. The records are sorted based on the values of the fields in the records. The external sorting algorithm is used to sort the records based on the values of the fields in the records. Later, we used this 'sort' functionality to implement the 'join', 'order by', and 'group by' functionalities of the same (details of which can be found down below).

## Assumptions:
The following are some of the assumptions that we made while developing this phase of the project:
- The data in the tables is integer only.
- Sorting can only be performed on fields that are of integer type.
- The buffer size was set to 10 (the limit provided in the project description).
- As per the doubt clarification document, the size of the row is strictly less than the page size.
- The page size is kept to be 8 kB.
- No two coloumns have the same name (neither in a table, nor in both tables in case of JOIN).
- We assumed that all of the functionalities in this phase needs to be reflecting in actual data files, that is, we need to make use of the table->makePermanent() function to store the result of the query in a dedicated file.
- Each block contains only 1 page. 

## SORT Functionality:
Syntax: 
    `SORT table_name BY col1, col2, col3, ... IN ASC/DESC, ASC/DESC, ASC/DESC, ...`

The SORT functionality was implementing by using the external sorting algorithm with k-way merge. The following is the external sorting algorithm:
- We have the data stored in the main memory in the form of blocks.
- First up, we perform internal sorting. We take all the data blocks and we sort them internally using the sort function provided by the C++ STL and a custom comparator function. This makes the all the blocks contain the sorted data among themselves.
- These sorted blocks are written to the disk in place of the original blocks.
- Now that we have the blocks internally sorted, we need to merge them to get the overall sorted data. We use the k-way merge algorithm to merge the blocks. The following are the steps in the k-way merge algorithm:
    - First up, we calculate the number of passes required to merge all the blocks. This is done by calculating the ceiling of the log of the number of blocks to the base k.
    - Therefore, in each pass, we merge all the subfiles into larger subfiles where each larger subfile is made by merging k smaller subfiles at max.
    - During each merge pass, we take k subfiles and merge them into a larger subfile. We do this for all the subfiles in the pass. The following are the steps in the merge pass:
        - We take k subfiles and read the first block from each of them and maintain k pointers pointing to the next block to be read in the kth subfile.
        - We push these blocks into a priority queue. The priority queue is a min heap and the comparator function is such that the block with the smallest value in the first record is at the top of the heap.
        - We keep popping the top element from the heap and push it to a vector of records. We then read the next row from the kth subfile from which the data was written to the output file. If the row happens to be the last row of the current block of the kth subfile, then using the kth pointer, we read the next block from the kth subfile and update the kth pointer to point to the next block to be read in the kth subfile. 
        - The row which is read next is pushed into the heap.
        - We repeat the above steps until the vector of records is full. Once the vector of records is full, we write the records to the output file and clear the vector of records.
        - We repeat the above steps until all the blocks in all the k subfiles are read.
    - The above steps merges k-subfiles into a larger subfile. We repeat the above steps for all the groups of k subfiles in the pass.

The following is the explanation of the k-way merge algorithm as per our implementation:
- We calculate the number of passes equal to the ceil of log of the number of blocks to the base k.
- We then perform the merge pass for each pass.
- During each merge pass, we calculate the number of groups of k subfiles. We then perform the merge pass for each group of k subfiles. The number of groups also denote the number of resulting larger subfiles that we will obtain at the end of this pass. We denote this by the variable `numSubfileToWrite.`
- We keep track of the the size of the subfiles that we are merging in this pass so as to facilitate the reading and merging of the blocks in the subfiles. We denote this by the variable `maxSubfileSize`. Initially, this variable is set to 1 and with each pass, it is multiplied by k (maximum limit is the blockCount).
- We then iterate over all the subfiles taking k-subfiles at a time and perform the merge pass for each group of k subfiles. The number of subfiles to merge for the ith larger subfile is obtained by taking the minimum of mergeDegree and the number of subfiles left to be merged. We denote this number by the varibale `numSubfilesToRead.`
- We create the buffer for the subfiles called as `currentBuffer` which stores the current block of the subfile that is being read. We also create a vector of pair to store the current block number and the last block number of the kth subfile. We denote this by the variable `subfileFirstLastBlockIndex`.
- We make a priority queue of the type `priority_queue<pair<Record, int>, vector<pair<Record, int>>, Comparator>`. The comparator function is such that the record with the smallest value in the first field is at the top of the heap. We denote this by the variable `heap`.
- We read and pop from the heap so as to get the next record to be written to the output file and push it to the `mergedData` vector. We push the next row to the heap from the block of the kth subfile from which the record has written to the `mergedData` vector. If the row happens to be the last row of the current block of the kth subfile, then using the kth pointer, we read the next block from the kth subfile and update the kth pointer to point to the next block to be read in the kth subfile. The row which is read next is pushed into the heap.
- When the `mergedData` vector is full, we write the records to the output file and clear the vector of records. 
- We repeat the above steps until all the blocks in all the k subfiles are read.
- At the end of each pass, we write the larger subfiles to the disk in place of the original files and clear the subfiles from the main memory.

**NOTE:**
    The formulas and the logic flow used for the implementation of the external sorting algorithm were taken from the book.

## JOIN Functionality:
Syntax: 
    `Result <- JOIN table1_name, table2_name ON col1 binop col2`

The JOIN functionality was implemented by using the external sorting algorithm with k-way merge. The following are the steps in the JOIN functionality:
- We first sort the data in both the tables using the external sorting algorithm.
- Then we loop over the tables and check the records in both the tables for the join condition. We continue/break if the join condition is falsified. If the join condition is satisfied, then we push the record to the `result` vector.
- We then write the `result` vector to the output file when full.

## ORDER BY Functionality:
Syntax: 
    `Result <- ORDER BY col_name ASC/DESC ON table_name`

The ORDER BY functionality was implemented by using the external sorting algorithm with k-way merge. The following are the steps in the ORDER BY functionality:
- We first make a copy of the source table.
- We sort the data in the copy table using the external sorting algorithm given the sorting condition.
- We then write the sorted data to the output file and make it permanent.

## GROUP BY Functionality:
Syntax: 
    `Result <- GROUP BY col_name FROM table_name HAVING aggregate(col_name) binop value RETURN aggregate_func(col_name)`

The GROUP BY functionality was implemented by using the external sorting algorithm with k-way merge. The following are the steps in the GROUP BY functionality:
- We first sort the data in the table using the external sorting algorithm.
- We keep the variables, `min`, `max`, `sum`, `count`, and `total_count` to keep track of all the possible aggregate function values.
- As we iterate over the table, we keep updating these variables as and when required.
- When the value of the group by column changes, we write the aggregate function value to the output file and clear the variables.


## Learning Outcomes:
- We learnt about the external sorting algorithm and its implementation.
- We learnt about the k-way merge algorithm and its implementation.
- We learnt about regex and its usage in query parsing.
- This phase also helped us understand the working of the database system and the associated dynamics in a better way.
