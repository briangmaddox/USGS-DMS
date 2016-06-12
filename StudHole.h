// $Id: StudHole.h,v 1.1.1.1 2002-02-19 19:05:09 bmaddox Exp $
// Class StudHole - Brian Maddox - bmaddox@usgs.gov
// Last Modified by $Author: bmaddox $ on $Date: 2002-02-19 19:05:09 $
// This defines the StudHole class.  It's purpose is to determine information
// for the calling class for items such as angle of skew, center of the 
// studhole, ect.  Many of these variables are based on this model
//
//         Z       |       /\
//           \     |     C    B
//             \   |   /        \
//             d)\ | /a)        d)\
// ----------------/--------A-------
//               / |\
//             /   |  \
               
#include <iostream.h>
#include "Image/TIFFImageIFile.h"
#include <math.h>

#define BLACKCOUNT 5 // Number of black pixels which must be present before
                     // the software determines it's in the studhole 

class StudHole
{
private:
  double skew_angle;   // Skew angle at that studhole(angle d)
  double slope;        // slope of the side line (slope of Z)
  double up_angle;     // Angle of the center normal line of the hole (angle a)
  double stud_width;   // True, normalized stud width (line C)
  long int flat_width; // Line A

  long int temp_centerX, temp_centerY; 
  long int centerX, centerY;  // True (x,y) of the studhole
  
  TIFFImageIFile* infile;
  long int ulx, uly;   // upper left (x,y) of the region to scan
  long int brx, bry;   // bottom right (x,y) of the region to scan
  long int upperrow, lowerrow; // Rows where the stud is actually between
  long int leftcolumn, rightcolumn;  // Columns the stud is actually between

  bool findStud(void);    // Searches the image for the true region of the stud
                          // Stores the top/bottom rows and columns
  bool findSlope(void);   // Calculates the angle of skew of the stud
  bool findFlatWidth(void);    // Calculate distance of A inside the studhole
  bool findStudWidth(void); // Calculates true stud width based on flat_width, 
                            // skew_angle, and up_angle;



  // These are here since I don't really want it to be called this way


public:
  StudHole(); // Default constructor
  StudHole(TIFFImageIFile* inputtiff, // So we can access the image
	   long int leftcol,     // Side column to "
	   long int toprow,      // Upper row to begin scanning at
	   long int rightcol,    // "
	   long int bottomrow);   // Bottom "


 
  bool findCenter(void); // Finally, go find the actual center of the stud
  long int getX(void);    // Return the calculated X Coordinate
  long int getY(void);    // Return the calculated Y Coordinate
};
