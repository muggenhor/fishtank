#ifndef _MS3D_ASCII_H
#define _MS3D_ASCII_H

#include "GL/GLee.h"
#include <GL/glfw.h>

#include <stdio.h>
#include <GL/gl.h>
#include <string>
#include <vector>
#include <Eigen/Core>

#include "textures.hpp"
#include "vertexarrays/trianglearray.hpp"

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
		Shape();
		~Shape();

		//bool loadFromFile( const char *filename );
		bool loadFromMs3dAsciiSegment(FILE* file, const Eigen::Matrix4f& transform = Eigen::Matrix4f::Identity());/// identity transform by default.

		//bool saveToFile( const char *filename );
		void render() const;

		size_t vertex_count() const;
		size_t index_count() const;

		Eigen::Vector3f bb_l, bb_h;

	private:
		friend class boost::serialization::access;

		template <class Archive>
		void serialize(Archive & ar, const unsigned int /* version */)
		{
			ar & triangles;
		}

	private:
		TriangleArray<unsigned int, float, float, float>* triangles;

		/**
		 * Don't optimize (by trimming duplicate vertices) models that
		 * exceed this amount of triangles.
		 */
		static const unsigned int max_optimizing_triangles;
};


//////////////////////////////////////
//The Material Class
//////////////////////////////////////
class Material
{
	public:
		Material();
		~Material();

		bool loadFromMs3dAsciiSegment(FILE* file, std::string path_);
		void activate() const;
		void reloadTexture();

	private:
		friend class boost::serialization::access;

		template <class Archive>
		void serialize(Archive & ar, const unsigned int /* version */)
		{
			ar & Name;
			ar & Ambient;
			ar & Diffuse;
			ar & Specular;
			ar & Emissive;
			ar & Shininess;
			ar & Transparency;
			ar & DiffuseTexture;
			ar & AlphaTexture;
			ar & texture;
			ar & path;
		}

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
		Texture* texture;
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
		void render() const;

		Eigen::Vector3f bb_l;/// bounding box for the model, low and high
		Eigen::Vector3f bb_h;

		std::string path;

	private:
		friend class boost::serialization::access;

		template <class Archive>
		void serialize(Archive & ar, const unsigned int /* version */)
		{
			ar & shapes;
			ar & material_indices;
			ar & materials;
		}

	private:
		std::vector<Shape>      shapes;
		std::vector<int>        material_indices;
		std::vector<Material>   materials;
};

#endif
