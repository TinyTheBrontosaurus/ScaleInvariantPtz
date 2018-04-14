#ifndef MATRIX_H
#define MATRIX_H

#include "types.h"

#define MATRIX_MAX_SIZE 10

/**************************************************************
 *
 * Matrix class
 * Description: A class that serves as a matrix.
 *
 **************************************************************
 */

class Matrix {

  public:
   /******************************************
    * ctor
    * Description: Sets defaults. 
    ******************************************
    */ 
    Matrix();
    
   /******************************************
    * ctor
    * Description: Sets numbers of rows and columns of the matrix.
    * Parameter:   myRows - the number of rows in the Matrix
    *              myColumns - the number of columns in the matrix
    ****************************************** 
    */
    Matrix( UINT32 myRows, UINT32 myColumns );

   /******************************************
    * ctor
    * Description: Sets numbers of rows and columns of the matrix and sets 
    *              the initial values of the elements of the matrix.
    * Parameter:   myValues - the values [column][row]
    *              myRows - the number of rows in the Matrix
    *              myColumns - the number of columns in the matrix
    ****************************************** 
    */    
    Matrix( FLOAT *myValues, UINT32 myRows, UINT32 myColumns );
    
   /******************************************
    * dtor
    * Description: Releases memory
    ******************************************
    */     
    ~Matrix();
  
   /******************************************
    * init
    * Description: Sets numbers of rows and columns of the matrix.
    * Parameter:   myRows - the number of rows in the Matrix
    *              myColumns - the number of columns in the matrix
    ****************************************** 
    */
    void init( UINT32 myColumns, UINT32 myRows );

   /******************************************
    * init
    * Description: Sets numbers of rows and columns of the matrix and sets 
    *              the initial values of the elements of the matrix.
    * Parameter:   myValues - the values [column][row]
    *              myRows - the number of rows in the Matrix
    *              myColumns - the number of columns in the matrix
    ****************************************** 
    */    
    void init( FLOAT *myValues, UINT32 myColumns, UINT32 myRows );    

   /******************************************
    * operator()
    * Description: Retrieves the value of an element in the matrix
    * Parameter:   myRow - the row of the element to retrieve
    *              myColumn - the column of the element to retrieve
    * Returns:     The value of that element
    ****************************************** 
    */            
    FLOAT& operator() (UINT32 myRow, UINT32 myCol);       //For setting
    FLOAT  operator() (UINT32 myRow, UINT32 myCol) const; //For getting
    
   /******************************************
    * operator+
    * Description: Adds two matrices together
    * Parameter:   that - The matrix to be added to this matrix
    * Returns:     The elementwise sum of the matrices
    ****************************************** 
    */          
    Matrix operator+ (const Matrix& that);

   /******************************************
    * operator-
    * Description: Subtracts one matrix from another
    * Parameter:   that - The matrix to be subtracted from this matrix
    * Returns:     The elementwise difference of the matrices
    ****************************************** 
    */          
    Matrix operator- (const Matrix& that);    
        
   /******************************************
    * operator*
    * Description: Multiplies two matrices together. Complains to the screen if the
    *              matrices sizes are illegal
    * Parameter:   that - The matrix to be multiplied against this matrix
    * Returns:     The dot product of the matrices
    ****************************************** 
    */              
    Matrix operator* (const Matrix& that);

   /******************************************
    * operator*
    * Description: Multiplies a matrix by a scalar
    * Parameter:   scalar - The scalar to be multiplied against this matrix
    * Returns:     This matrix scaled by the scalar
    ****************************************** 
    */              
    Matrix operator* (const FLOAT scalar);
            
   /******************************************
    * operator/
    * Description: Divides a matrix by a scalar
    * Parameter:   scalar - The scalar to be multiplied against this matrix
    * Returns:     This matrix scaled by the scalar
    ****************************************** 
    */              
    Matrix operator/ (const FLOAT scalar);
       
   /******************************************
    * operator~
    * Description: Transposes the matrix
    * Returns:     The transpose of the matrix
    ****************************************** 
    */              
    Matrix operator~ ();
   
   /******************************************
    * inv
    * Description: Inverts the matrix, if the matrix is square and has
    *              dimension of less than 3.
    * Returns:     The inverse of the matrix
    ****************************************** 
    */                
    Matrix inv();
   
   /******************************************
    * eigVal
    * Description: Finds the eigenvalues of the matrix.
    * Returns:     The eigenvalues in a column vector
    ****************************************** 
    */                
    Matrix eigVal();
       
   /******************************************
    * print
    * Description: Prints the values of the matrix
    ****************************************** 
    */            
    void print();
       
  private:  
  
    //The values
    FLOAT data[MATRIX_MAX_SIZE][MATRIX_MAX_SIZE];
    
    //The total number of columns
    UINT32 cols;
    
    //The total number of rows
    UINT32 rows;


};

#endif

// File: $Id: Matrix.h,v 1.5 2005/08/02 19:26:11 edn2065 Exp $
// Author: Eric D Nelson
// Revisions:
// $Log: Matrix.h,v $
// Revision 1.5  2005/08/02 19:26:11  edn2065
// Added eigenvalues to Matrix
//
// Revision 1.4  2005/07/28 01:42:18  edn2065
// Made it so fixation was only calculated when p/t/z values were available
//
// Revision 1.3  2005/07/27 22:29:12  edn2065
// Added multiplication and division by scalars. Added inversion
//
// Revision 1.2  2005/07/27 01:15:20  edn2065
// Added tranpose function to matrix
//
//
//
