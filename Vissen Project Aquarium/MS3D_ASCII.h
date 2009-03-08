#ifndef _MS3D_ASCII_H
#define _MS3D_ASCII_H

#include <boost/function.hpp>
#include "GL/GLee.h"
#include <GL/glfw.h>

#include <stdio.h>
#include <GL/gl.h>
#include <string>
#include <vector>
#include <Eigen/Core>

#include "JPEG.h"

#define MS_MAX_NAME 128
#define MS_MAX_PATH 256

class FileWrap
{
	public:
		FileWrap(FILE* const file) :
			file(file)
		{
		}

		~FileWrap()
		{
			if (file)
			{
				fclose(file);
			}
		}

		operator FILE*() const
		{
			return file;
		}

	private:
		// Disallow copying
		FileWrap(const FileWrap&);
		FileWrap& operator=(const FileWrap&);

	private:
		FILE* const file;
};

class Shape
{
	public:
		typedef boost::function<boost::function<void ()> (const std::vector<Eigen::Vector3f>& vertices,
		                                                  const std::vector<Eigen::Vector2f>& texcoords,
		                                                  const std::vector<Eigen::Vector3f>& normals,
		                                                  const std::vector<unsigned int>& indices)> transform_function;

		Shape();

		//bool loadFromFile( const char *filename );
		bool loadFromMs3dAsciiSegment(FILE* file, const Eigen::Matrix4f& transform = Eigen::Matrix4f::Identity());/// identity transform by default.

		//bool saveToFile( const char *filename );
		void render(const transform_function& function = transform_function()) const;

		std::vector<Eigen::Vector3f>    vertices;
		std::vector<Eigen::Vector2f>    texcoords;
		std::vector<Eigen::Vector3f>    normals;
		std::vector<unsigned int>       indices;

		Eigen::Vector3f bb_l, bb_h;

	private:
		void AddPoint(const Eigen::Vector3f& vertex, const Eigen::Vector2f& texcoord, const Eigen::Vector3f& normal);
};


//////////////////////////////////////
//The Material Class
//////////////////////////////////////
class Material
{
	public:
		bool loadFromMs3dAsciiSegment(FILE* file, std::string path_);
		void activate() const;
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

		bool loadFromMs3dAsciiFile(const char* filename, const Eigen::Matrix4f& transform = Eigen::Matrix4f::Identity());
		void reloadTextures();
		void render(const Shape::transform_function& function = Shape::transform_function()) const;

		Eigen::Vector3f bb_l;/// bounding box for the model, low and high
		Eigen::Vector3f bb_h;

		std::string path;

	private:
		std::vector<Shape>      shapes;
		std::vector<int>        material_indices;
		std::vector<Material>   materials;
};

#endif
