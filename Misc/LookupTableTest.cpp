#include "LookupTable.h"
#include "Matrix.h"

#define TABLE_LENGTH  (10)
#define OFFSET (3)

#define TEST_MATRIX
//#define TEST_LKP_TBL

void testLookupTable();
void testMatrix();

int main() {
#ifdef TEST_LKP_TBL
  testLookupTable();
#endif

#ifdef TEST_MATRIX
  testMatrix();
#endif
  
}

void testMatrix()
{

  Matrix a;
  Matrix b;
  Matrix c;
  Matrix d;
  Matrix e;
  Matrix f;
  Matrix g;
  Matrix h;
  
  FLOAT foo[] = {1.0, 2.0, 3.0,4.0, 5.0, 6.0,7.0, 8.0, 9.0};
  FLOAT eye[] = {1, 0, 0,0, 1, 0,0, 0, 1};
  FLOAT oth[] = {10, 100, 1000};
  FLOAT deuce[] = {1, 2, 6, 7};
  
  a.init(foo, 3, 3);
  b.init(eye, 3, 3);
  c = a + b;
  d = a * b;
  e = a * a;
  f.init(oth, 3, 1);    
  g = a * f;
  h.init(deuce,2,2);
  
  printf( "A =" CRLF);
  a.print();
  
  printf( "B =" CRLF);
  b.print();
    
  printf( "C =" CRLF);
  c.print();
  
  printf( "D =" CRLF);
  d.print();
    
  printf( "E =" CRLF);
  e.print();
  
  printf( "F =" CRLF);
  f.print();
    
  printf( "G =" CRLF);
  g.print();
  
  printf( "A' =" CRLF);
  (~a).print();
  
  printf( "B' =" CRLF);
  (~b).print();
    
  printf( "C' =" CRLF);
  (~c).print();
  
  printf( "D' =" CRLF);
  (~d).print();
    
  printf( "E' =" CRLF);
  (~e).print();
  
  printf( "F' =" CRLF);
  (~f).print();
    
  printf( "G' =" CRLF);
  (~g).print();

  printf( "H =" CRLF);
  (h).print(); 
  printf( "inv(H) =" CRLF);
  (h.inv()).print();
  printf( "eigVal(H) =" CRLF);
  (h.eigVal()).print();
  
       
}

void testLookupTable()
{
  float table[TABLE_LENGTH];
  int length = TABLE_LENGTH;
  int tablePos;
  LookupTable lookupTable;
  LookupTable lookupTable2;
  
  //Create a parabolic table
  for( tablePos = 0; tablePos < length; tablePos++) {
    table[tablePos] = (tablePos+OFFSET) * (tablePos+OFFSET) ;
  }

  //Set the table
  lookupTable.load( table, length, OFFSET, (OFFSET+length-1), true);

  //Test printing, saving, and looking up the forward table
  cout << "Printing..." << endl;
  lookupTable.print();
  cout << "Saving..." << endl;
  lookupTable.save( "foo.tbl" );
  cout << "Testing elements..." << endl;
  cout << ".2: " << lookupTable.lookup(.2) << endl;
  cout << "3: " << lookupTable.lookup(3) << endl;
  cout << "3.2: " << lookupTable.lookup(3.2) << endl;
  cout << "4.2: " << lookupTable.lookup(4.2) << endl;
  cout << "9: " << lookupTable.lookup(9) << endl;
  cout << "12: " << lookupTable.lookup(12) << endl;
  
  //Test loading and looking up the reverse table
  cout << "Loading the table in a new file..." << endl;

  lookupTable2.load( "foo.tbl" );
  lookupTable2.print();

  cout << "10: " << lookupTable.lookupReverse(10) << endl;
  cout << "20: " << lookupTable.lookupReverse(20) << endl;
  cout << "30: " << lookupTable.lookupReverse(30) << endl;
  cout << "40: " << lookupTable.lookupReverse(40) << endl;
  cout << "64: " << lookupTable.lookupReverse(64) << endl;
  cout << "80: " << lookupTable.lookupReverse(80) << endl;
}

// File: $Id: LookupTableTest.cpp,v 1.7 2005/08/02 19:26:11 edn2065 Exp $
// Author: Eric D Nelson
// Revisions:
// $Log: LookupTableTest.cpp,v $
// Revision 1.7  2005/08/02 19:26:11  edn2065
// Added eigenvalues to Matrix
//
// Revision 1.6  2005/07/27 22:29:12  edn2065
// Added multiplication and division by scalars. Added inversion
//
// Revision 1.5  2005/07/27 01:15:20  edn2065
// Added tranpose function to matrix
//
// Revision 1.4  2005/07/27 00:31:34  edn2065
// Create Matrix
//
// Revision 1.3  2005/06/02 18:11:42  edn2065
// Tested ZoomOptical
//
// Revision 1.2  2005/06/02 13:55:41  edn2065
// Cleaned LookupTable and ran test program
//
// Revision 1.1  2005/05/27 16:27:22  edn2065
// Added to repository from OldProject
//
// Revision 1.2  2005/03/11 04:04:31  edn2065
// Does basic test.
//
// Revision 1.1  2005/03/11 00:28:37  edn2065
// Initial revision
//
