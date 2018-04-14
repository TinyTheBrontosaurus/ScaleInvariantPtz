#include "Matrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
/*****************************************************************
 * Matrix ctor
 *****************************************************************
 */
Matrix::Matrix()
{
  //Set as default
  init(1,1);
}   

/*****************************************************************
 * Matrix ctor
 *****************************************************************
 */
Matrix::Matrix( UINT32 myRows, UINT32 myColumns )
{
  //Set values
  init(myRows, myColumns);
}

/*****************************************************************
 * Matrix ctor
 *****************************************************************
 */
Matrix::Matrix( FLOAT *myValues, UINT32 myRows, UINT32 myColumns )  
{
  init(myValues, myRows, myColumns);  
}

/*****************************************************************
 * Matrix dtor
 *****************************************************************
 */
Matrix::~Matrix()
{
  //Do nothing
}

/*****************************************************************
 * Matrix::init
 *****************************************************************
 */
void Matrix::init( UINT32 myRows, UINT32 myColumns )
{
  rows = myRows;
  cols = myColumns;
}

/*****************************************************************
 * Matrix::init
 *****************************************************************
 */ 
void Matrix::init( FLOAT *myValues, UINT32 myRows, UINT32 myColumns )
{
  UINT32 curRow;
  UINT32 curCol;
  
  //Save the columns and rows
  init( myRows, myColumns );
     
  //Copy the values over  
  for( curRow = 0; curRow < rows; curRow++ )
  {
    for( curCol = 0; curCol < cols; curCol++ )
    {
      (*this)(curRow,curCol) = myValues[curRow*cols + curCol];
    }    
  }
  
}

/*****************************************************************
 * Matrix::operator()
 *****************************************************************
 */ 
FLOAT& Matrix::operator() (UINT32 myRow, UINT32 myCol)
{
  if( myRow >= rows || myCol >= cols )
  {
    printf( "Matrix Error! Bad size row(%d/%d) or col(%d/%d) in &operator() (%s @ %d)"CRLF,
      myRow, rows, myCol, cols, __FILE__, __LINE__ );         
   
    //Crash the system for debug purposes
    exit(1);
  }
  else
  {
    return data[myRow][myCol];
  }
}

/*****************************************************************
 * Matrix::operator()
 *****************************************************************
 */ 
FLOAT  Matrix::operator() (UINT32 myRow, UINT32 myCol) const
{
  if( myRow >= rows || myCol >= cols )
  {
    printf( "Matrix Error! Bad size row(%d/%d) or col(%d/%d) in operator() (%s @ %d)"CRLF,
      myRow, rows, myCol, cols, __FILE__, __LINE__ );         
   
    //Crash the system for debug purposes
    exit(1);
  }
  else
  {
    return data[myRow][myCol];
  }
}

/*****************************************************************
 * Matrix::operator+
 *****************************************************************
 */ 
Matrix Matrix::operator+ (const Matrix& that)
{
  if( rows != that.rows || cols != that.cols)
  {
    printf( "Matrix Error! Non-matching rows(%d/%d) or cols(%d/%d) in add (%s @ %d)"CRLF,
      rows, that.rows, cols, that.cols, __FILE__, __LINE__ );         
   
    //Crash the system for debug purposes
    exit(1);
  }
  else
  {
    UINT32 curRow, curCol;
    Matrix temp(rows,cols);
    for( curRow = 0; curRow < rows; curRow++ )
    {
      for( curCol = 0; curCol < cols; curCol++ )
      {
        temp(curRow,curCol) = (*this)(curRow, curCol) + that(curRow,curCol);
      }    
    }
  
    return temp;
  }
}

/*****************************************************************
 * Matrix::operator-
 *****************************************************************
 */ 
Matrix Matrix::operator- (const Matrix& that)
{
  if( rows != that.rows || cols != that.cols)
  {
    printf( "Matrix Error! Non-matching rows(%d/%d) or cols(%d/%d) in sub (%s @ %d)"CRLF,
      rows, that.rows, cols, that.cols, __FILE__, __LINE__ );         
   
    //Crash the system for debug purposes
    exit(1);
  }
  else
  {
    UINT32 curRow, curCol;
    Matrix temp(rows,cols);
    for( curRow = 0; curRow < rows; curRow++ )
    {
      for( curCol = 0; curCol < cols; curCol++ )
      {
        temp(curRow,curCol) = (*this)(curRow, curCol) - that(curRow,curCol);
      }    
    }
  
    return temp;
  }
}

/*****************************************************************
 * Matrix::operator*
 *****************************************************************
 */ 
Matrix Matrix::operator* (const Matrix& that)
{
  if( cols != that.rows)
  {
    printf( "Matrix Error! Non-matching rows(%d) to cols(%d) in add (%s @ %d)"CRLF,
      cols, that.rows, __FILE__, __LINE__ );         
   
    //Crash the system for debug purposes
    exit(1);
  }
  else
  {
    UINT32 curRow, curCol, curAdd;    
    FLOAT sum;
    Matrix temp(rows, that.cols );
    for( curRow = 0; curRow < temp.rows; curRow++ )
    {
      for( curCol = 0; curCol < temp.cols; curCol++ )
      {
        sum = 0;
        for( curAdd = 0; curAdd < cols; curAdd++ )
        {         
          sum += (*this)(curRow, curAdd) * that(curAdd,curCol);
        }
        temp(curRow,curCol) = sum;
      }    
    }
  
    return temp;
  }
  
}

/*****************************************************************
 * Matrix::operator*
 *****************************************************************
 */ 
Matrix Matrix::operator* (const FLOAT scalar)
{

  UINT32 curRow, curCol;
  Matrix temp(rows, cols );
  for( curRow = 0; curRow < temp.rows; curRow++ )
  {
    for( curCol = 0; curCol < temp.cols; curCol++ )
    {
      temp(curRow,curCol) = scalar * (*this)(curRow,curCol);      
    }    
  }

  return temp;
  
}

/*****************************************************************
 * Matrix::operator/
 *****************************************************************
 */ 
Matrix Matrix::operator/ (const FLOAT scalar)
{

  UINT32 curRow, curCol;
  Matrix temp(rows, cols );
  for( curRow = 0; curRow < temp.rows; curRow++ )
  {
    for( curCol = 0; curCol < temp.cols; curCol++ )
    {
      temp(curRow,curCol) = (*this)(curRow,curCol) / scalar;      
    }    
  }

  return temp;
  
}


/*****************************************************************
 * Matrix::operator~
 *****************************************************************
 */ 
Matrix Matrix::operator~ ()
{   
  UINT32 curRow, curCol, curAdd;    
  FLOAT sum;
  Matrix temp(cols, rows );
  for( curRow = 0; curRow < rows; curRow++ )
  {
    for( curCol = 0; curCol < cols; curCol++ )
    {
      temp(curCol,curRow) = (*this)(curRow,curCol);
    }    
  }
  return temp;    
}

/*****************************************************************
 * Matrix::inv
 *****************************************************************
 */ 
Matrix Matrix::inv()
{

  if( rows != cols )
  {
    printf( "Error! Matrix must be square to be inverted! (%d, %d) (%s @ %d)"CRLF,
      rows, cols, __FILE__, __LINE__ ); 
    //Crash the system for debug purposes
    exit(1);   
  }
  else if( rows > 2 )
  {
    printf( "Error! Matrix must be of dimension 1 or 2 to be inverted! (%d, %d) (%s @ %d)"CRLF,
      rows, cols, __FILE__, __LINE__ ); 
    //Crash the system for debug purposes
    exit(1);
  }
  else
  {  
    Matrix temp(rows, cols);
    if( rows == 1 )
    {
      temp(0,0) = 1/(*this)(0,0);
    }
    else
    {
      FLOAT det;
      temp(0,0) = (*this)(1,1);
      temp(0,1) = -((*this)(0,1));
      temp(1,0) = -((*this)(1,0));
      temp(1,1) = (*this)(0,0);
      det = ( ((*this)(0,0)*(*this)(1,1) - (*this)(1,0)*(*this)(0,1)));
      temp = temp /det;
    }
    return temp;     
  }

}


/*****************************************************************
 * Matrix::eigVal
 *****************************************************************
 */ 
Matrix Matrix::eigVal()
{

  if( rows != cols )
  {
    printf( "Error! Matrix must be square to find eigenvalues! (%d, %d) (%s @ %d)"CRLF,
      rows, cols, __FILE__, __LINE__ ); 
    //Crash the system for debug purposes
    exit(1);     
  }
  else if( rows > 2 )
  {
    printf( "Error! Matrix must be of dimension 1 or 2 to find eigenvalues! (%d, %d) (%s @ %d)"CRLF,
      rows, cols, __FILE__, __LINE__ ); 
    //Crash the system for debug purposes
    exit(1);
  }
  else
  {  
    Matrix temp(rows, 1);
    if( rows == 1 )
    {
      temp(0,0) = (*this)(0,0);
    }
    else
    {
      temp(0,0) = 0.5*(((*this)(0,0)+(*this)(1,1)) - 
               sqrt(4* (*this)(0,1)*(*this)(1,0)+pow((*this)(0,0)-(*this)(1,1),2)));
      temp(1,0) = 0.5*(((*this)(0,0)+(*this)(1,1)) + 
               sqrt(4* (*this)(0,1)*(*this)(1,0)+pow((*this)(0,0)-(*this)(1,1),2)));
    
    }
    return temp;     
  }

}
/*****************************************************************
 * Matrix::print
 *****************************************************************
 */ 
void Matrix::print()
{
  CHAR text[100];
  UINT32 curRow, curCol;
  
  strcpy(text,"");
  
  for( curRow = 0; curRow < rows; curRow++ )
  {
    for( curCol = 0; curCol < cols; curCol++ )
    {
      sprintf( text, "%s%5.2lf ", text, (*this)(curRow,curCol));
    } 
    sprintf( text, "%s"CRLF, text );   
  }
  printf( "%s" CRLF, text);
} 
 
 
// File: $Id: Matrix.cpp,v 1.6 2005/08/10 02:48:03 edn2065 Exp $
// Author: Eric D Nelson
// Revisions:
// $Log: Matrix.cpp,v $
// Revision 1.6  2005/08/10 02:48:03  edn2065
// Changed to allow warning and error free ICC compile
//
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
