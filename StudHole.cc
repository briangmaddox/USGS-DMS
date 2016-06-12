// $Id: StudHole.cc,v 1.1.1.1 2002-02-19 19:05:09 bmaddox Exp $
// Implementation for Class StudHole Brian Maddox - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-02-19 19:05:09 $
#include "StudHole.h"
#include <math.h>
#include <iostream.h>

// This is just so it would compile before I decide how to do this ;)
#define BLACKINDEX 0
#define BLACKCOUNT 5
#define SLOPEDIFF 10 // number to add and subtract from middle row
#define UNDEFINED 999.0 // in case the stud is straight up/down

StudHole::StudHole(TIFFImageIFile* inputtiff, long int leftcol, 
		   long int toprow, long int rightcol, long int bottomrow)
{
  infile = inputtiff;
  uly = toprow;
  bry = bottomrow;
  ulx = leftcol;
  brx = rightcol;
  
 // Now just make sure everything is initialized
  skew_angle = slope = up_angle = stud_width = 0.0;
  flat_width = 0;
  temp_centerX = temp_centerY = centerX = centerY = 0;
  upperrow = lowerrow = leftcolumn = rightcolumn = 0;

}


// Heh heh heh, heh heh heh
bool StudHole::findStud(void)
{
  int blackfound = 0; // number of black pixels found
  unsigned char* buffer;
  bool foundtop, foundbottom; // Flag if we've found them

  foundtop = foundbottom = false;
  // go find the top 
  for (int rowcount = uly; rowcount <= bry; rowcount++)
  {
    buffer = (unsigned char*)infile->getRawScanline(rowcount);
    for (int colcount = ulx; colcount <= brx; colcount++)
    {
      if (buffer[colcount] == BLACKINDEX)
      {
	blackfound++;
	if (blackfound == BLACKCOUNT)
	{
	  if (!foundtop)
	  {
	    upperrow = rowcount;
	    leftcolumn = colcount - 200; // this is arbitrary, change as needed
	  }
	  foundtop = true;
	  break;
	}
      }
      else
       // must have found some random noise in the image
	blackfound = 0; // reset blackcount
    }

    delete [] buffer;
    if (foundtop)
      break;
  }

  // If we couldn't find the top stud, there's no sense in trying to find the
  // bottom one
  if (!foundtop)
    return false;

  // Now go find the bottom
  blackfound = 0;
  for (int rowcount = bry; rowcount >= uly; rowcount--)
  {
    buffer = (unsigned char*)infile->getRawScanline(rowcount);
    for (int colcount = ulx; colcount <= brx; colcount++)
    {
      if (buffer[colcount] == BLACKINDEX)
      {
	blackfound++;
	if (blackfound == BLACKCOUNT)
	{
	  if (!foundbottom)
	  {
	    lowerrow = rowcount;
	    rightcolumn = colcount + 200;
	  }
	  foundbottom = true;
	  break;
	}
      }
      else
	blackfound = 0; // reset cause we were on noise ?
    }
    delete [] buffer;
    if (foundbottom)
      break;
  }

  return (foundtop && foundbottom);
}



bool StudHole::findSlope(void)
{
  bool foundtop, foundbottom;
  int blackfound = 0;
  unsigned char* buffer;
  long int middlerow;
  long int tx1 = 0, ty1 = 0, tx2 = 0, ty2 = 0; // store the temp coords here

  foundtop = foundbottom = false;
  middlerow = upperrow + ( (lowerrow - upperrow) / 2);

  // find the (x.y) of the first coord
  buffer = (unsigned char*)infile->getRawScanline(middlerow - SLOPEDIFF);
  for (int colcount = leftcolumn; colcount <= rightcolumn; colcount++)
  {
    if (buffer[colcount] == BLACKINDEX)
    {
      blackfound++;
      if (blackfound == BLACKCOUNT)
      {
	tx1 = (colcount - blackfound); // go to the first pixel we found
	ty1 = (middlerow - SLOPEDIFF);
	foundtop = true;
	break;
      }
      if (colcount == rightcolumn) // got here and didn't find it
      {
	delete [] buffer;
	return false;
      }
    }
    else // Found a white pixel, may have been in noise so reset count
      blackfound = 0;
  }

  delete [] buffer;

  // if we couldn't find the top pixel for the slope, no sense in trying to
  // find the bottom one!
  if (!foundtop)
    return false;

  blackfound = 0;
  
  buffer = (unsigned char*)infile->getRawScanline(middlerow + SLOPEDIFF);
  for (int colcount = leftcolumn; colcount <= rightcolumn; colcount++)
  {
    if (buffer[colcount] == BLACKINDEX)
    {
      blackfound++;
      if (blackfound == BLACKCOUNT)
      {
	tx2 = colcount - BLACKCOUNT; // go to the first pixel we found
	ty2 = (middlerow + SLOPEDIFF);
	foundbottom = true;
	break;
      }
      if (colcount == rightcolumn) // got here and didn't find it
      {
	delete [] buffer;
	return false;
      }
    }
    else
      blackfound = 0;
  }
  delete [] buffer;
  
  if (foundbottom)
  {
    if ( (tx2 - tx1) == 0 )
    {
      slope = UNDEFINED;
      cout << "findSlope(): Slope is: " << slope << endl;
      return true;
    }
    slope = (ty2 - ty1) / (tx2 - tx1);
    cout << "findSlope(): Slope is: " << slope << endl;
    return true;
  }
  else
    return false;
}


bool StudHole::findFlatWidth(void)
{
  long int middlerow;
  unsigned char* inbuffer;
  int blackfound = 0;
  bool okflag = false; // set true if think we've found the distance
  bool firstblack = false; // set on finding first black pixel

  middlerow = upperrow + ( (lowerrow - upperrow) / 2);
  temp_centerY = middlerow; // store in case we find it

  inbuffer = (unsigned char*)infile->getRawScanline(middlerow);

  for (int colcount = leftcolumn; colcount <= rightcolumn; colcount++)
  {
    if (inbuffer[colcount] == BLACKINDEX)
    {
      blackfound++;
      if (!firstblack)
      {
	firstblack = true; // set in case we found the real first pixel
	temp_centerX = colcount; // record the column it was at here
      }
    }
    else
    {
      if (blackfound < BLACKCOUNT) // must have just been noise in the image
      {
	blackfound = 0;
	firstblack = false; // reset so we can try to find again
      }
      else
      {    // probably found it
	flat_width = blackfound;
	cout << "findFlatWidth(): Width is: " << flat_width << endl;
	okflag = true;
	break;
      }
    }
  }

  return okflag;
}


bool StudHole::findCenter(void)
{
  double tempangle;
  double temp_x, temp_y; // x & y segments of vector ending at C/2

  if (!findStud()) return false;
  cout << "findStud() worked!" << endl;
  if (!findSlope()) return false;
  cout << "findSlope() worked!" << endl;
  if (!findFlatWidth()) return false;
  cout << "findFlatWidth() worked!" << endl;

  if (slope == UNDEFINED) // easy to calculate this way! Stud slope is straight
                          // up and down
  {
    centerX = temp_centerX + (flat_width / 2);
    centerY = temp_centerY;
    return true;
  }

  skew_angle = atan(slope); // calculate d
  
  up_angle = 90.0 - skew_angle; // calculate a

  stud_width = flat_width * sin(skew_angle);

  tempangle = 90 - up_angle;
  
  temp_x = (2.0 * sin (tempangle) ) / stud_width;
  temp_y = (2.0 * sin (up_angle) ) / stud_width;

  centerX = temp_centerX + ceil(temp_x);
  centerY = temp_centerY + ceil(temp_y);

  return true;
}

long int StudHole::getX(void)
{
  return centerX;
}

long int StudHole::getY(void)
{
  return centerY;
}

