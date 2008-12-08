#ifndef _MS3D_ASCII_H
#define _MS3D_ASCII_H

#define GLFW_BUILD_DLL
#include <GL/glfw.h>

#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <assert.h>
#include "Matrix.h"


#define MS_MAX_NAME 32
#define MS_MAX_PATH 256

#include "math3.h"


class Vec
{
	public:
		void	transform( const Matrix& m );
		void	transform3( const Matrix& m );
		float	x,y,z;
		float	w;
		float	u,v;							// Texture position
		int		bone;
};


class Tri
{
	public:
		int		v[3];
		int		n[3];
};


class Normal
{
	public:
		float	x,y,z;
};


//////////////////////////////////////
//The Shape Class
//////////////////////////////////////
class Shape
{
	public:
		Shape();
		~Shape();

		//bool loadFromFile( const char *filename );
		bool loadFromMs3dAsciiSegment( FILE *file, const math3::Matrix4x4f &transform=math3::Matrix4x4f(1,0,0,0 ,0,1,0,0, 0,0,1,0, 0,0,0,1));/// identity transform by default.

		//bool saveToFile( const char *filename );
		void render( void );

		int	num_vertices;
		Vec	*vertices;

		int	num_triangles;
		Tri	*triangles;

		int	num_normals;
		Normal	*normals;
};


//////////////////////////////////////
//The Material Class
//////////////////////////////////////
class Material
{
	public:
		Material();
		~Material();

		bool loadFromMs3dAsciiSegment( FILE *file );
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
		GLuint texture;
};


//////////////////////////////////////
//The Model Class
//////////////////////////////////////
class Model
{
	public:
		Model();
		~Model();

		bool loadFromMs3dAsciiFile( const char *filename, const math3::Matrix4x4f &transform=math3::Matrix4x4f(1,0,0,0 ,0,1,0,0, 0,0,1,0, 0,0,0,1));
		void reloadTextures( void );
		void render(const math3::Vec3f &wiggle_freq=math3::Vec3d(1,0,0), const math3::Vec3f &wiggle_dir=math3::Vec3d(0,0,0), double wiggle_phase=0);

	protected:
		float x,y,z;

	private:
		int	num_shapes;
		Shape *shapes;
		int	*material_indices;

		int	num_materials;
		Material *materials;
};




#endif
