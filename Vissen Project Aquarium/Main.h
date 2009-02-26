#ifndef _MAIN_H
#define _MAIN_H

#define GLFW_BUILD_DLL
#include <GL/glfw.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <fstream>
#include <vector>

using namespace std;

//////////////////////////////////////
//The CVector Class
//////////////////////////////////////
class CVector
{
	public:
		float x, y, z;
};

//////////////////////////////////////
//The CVector3 Struct
//////////////////////////////////////
class CVector3
{
	public:

		CVector3() {}

		CVector3(float X, float Y, float Z)
		{
			x = X; y = Y; z = Z;
		}

		CVector3 operator+(CVector3 vVector)
		{
			return CVector3(vVector.x + x, vVector.y + y, vVector.z + z);
		}

		CVector3 operator-(CVector3 vVector)
		{
			return CVector3(x - vVector.x, y - vVector.y, z - vVector.z);
		}

		CVector3 operator*(float num)
		{
			return CVector3(x * num, y * num, z * num);
		}

		float x, y, z;
};

#endif
