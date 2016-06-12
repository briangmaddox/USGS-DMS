// Defining class for the Plate object.  This object will be a wrapper for 
// the image actually.  It will contain a TIFFImageIFile object for the
// image data, and two objects based on the StudHole class.  It also contains
// all of the data necessary for normalizing and translating the image

#include <iostream.h>
#include "Image/TIFFImageIFile.h"
#include "DMS/StudHole.h"
