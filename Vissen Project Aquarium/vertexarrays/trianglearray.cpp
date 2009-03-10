#include "trianglearray.hpp"
#include <GL/gl.h>
#include <GL/glext.h>

bool extglDrawRangeElementsChecked = false;
PFNGLDRAWRANGEELEMENTSEXTPROC glDrawRangeElementsEXT = 0;
