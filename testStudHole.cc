// $Id: testStudHole.cc,v 1.1.1.1 2002-02-19 19:05:09 bmaddox Exp $
// Driver program to test the StudHole object
// Last modified by $Author: bmaddox $ on $Date: 2002-02-19 19:05:09 $

#include "Image/TIFFImageIFile.h"
#include "StudHole.h"

int main (int argc, char* argv[])
{
  TIFFImageIFile* intiff;
  StudHole* topstud;
  StudHole* bottomstud;

  if (argc < 2)
  {
    cerr << "Usage: " << argv[0] << " inplate" << endl;
    exit(-1);
  }

  
  intiff = new TIFFImageIFile(argv[1]);
  if (!intiff->good())
  {
    cerr << "Error opening tiff " << argv[1] << ". Exiting...." << endl;
    delete intiff;
    exit(-1);
  }

  // These numbers are arbitrary.  Change once I actually get some damned 
  // images of some seperates
  topstud = new StudHole(intiff, 8650, 37, 9010, 500);
  
  if (!topstud->findCenter())
    cerr << "Error.  Couldn't find coordinates of the Top Mesemex Stud" 
	 << endl;
  else
  {
    cout << "X Coordinate of Top Stud: " << topstud->getX() << endl;
    cout << "Y Coordinate of Top Stud: " << topstud->getY() << endl;
  }

  bottomstud = new StudHole(intiff, 8550, 29025, 8880, 29450);

  if (!bottomstud->findCenter())
    cerr << "Error.  Couldn't find coordinates of the Bottom Mesemex Stud"
	 << endl;
  else
  {
    cout << "X Coordinate of Bottom Stud: " << bottomstud->getX() << endl;
    cout << "Y Coordinate of Bottom Stud: " << bottomstud->getY() << endl;
  }

  cout << "Finished." << endl;
  
  delete intiff;
  delete topstud;
  delete bottomstud;

  return 1;
}


