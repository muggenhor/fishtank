#ifndef _MS3D_ASCII_H
#define _MS3D_ASCII_H

#include "GL/GLee.h"
#define GLFW_BUILD_DLL
#include <GL/glfw.h>

#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <assert.h>
#include <string>
#include <vector>

#include "Matrix.h"
#include "JPEG.h"



#define MS_MAX_NAME 128
#define MS_MAX_PATH 256

#include "math3.h"


struct Vec
{
	float   x,y,z;

	/// Texture coordinates
	float   u,v;
};

struct Tri
{
	int     v[3];
	int     n[3];
};

typedef math3::Vec3<float> Normal;

class Shape
{
	public:
		Shape();

		//bool loadFromFile( const char *filename );
		bool loadFromMs3dAsciiSegment( FILE *file, const math3::Matrix4x4f &transform=math3::Matrix4x4f(1,0,0,0 ,0,1,0,0, 0,0,1,0, 0,0,0,1));/// identity transform by default.

		//bool saveToFile( const char *filename );
		void render();

		std::vector<Vec>        vertices;
		std::vector<Tri>        triangles;
		std::vector<Normal>     normals;

		math3::Vec3d bb_l, bb_h;
};


//////////////////////////////////////
//The Material Class
//////////////////////////////////////
class Material
{
	public:
		bool loadFromMs3dAsciiSegment( FILE *file, std::string path_ );
		void activate( void );
		void reloadTexture( void );

	private:
		char  Name[MS_MAX_NAME];
		float Ambient[4];
		float Diffuse[4];
		float Specular[4];
		float Emissive[4];
		float Shininess;
		float Transparency;
		char  DiffuseTexture[MS_MAX_NAME];
		char  AlphaTexture[MS_MAX_NAME];
		Texture texture;
		std::string path;
};


//////////////////////////////////////
//The Model Class
//////////////////////////////////////
class Model
{
	public:
		Model();

		bool loadFromMs3dAsciiFile( const char *filename, const math3::Matrix4x4f &transform=math3::Matrix4x4f(1,0,0,0 ,0,1,0,0, 0,0,1,0, 0,0,0,1));
		void reloadTextures();
		/// note: internally, it will be subtly incorrect when wiggle is not x axis.
		void render(const math3::Vec3f &wiggle_freq=math3::Vec3d(1,0,0), const math3::Vec3f &wiggle_dir=math3::Vec3d(0,0,0), double wiggle_phase=0, double turn=0);/// turn is inverse radius

		math3::Vec3d bb_l;/// bounding box for the model, low and high
		math3::Vec3d bb_h;

		std::string path;

	protected:
		float x,y,z;

	private:
		std::vector<Shape>      shapes;
		std::vector<int>        material_indices;
		std::vector<Material>   materials;
};

#endif
