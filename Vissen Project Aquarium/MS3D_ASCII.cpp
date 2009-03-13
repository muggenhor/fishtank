#include <algorithm>
#include <boost/array.hpp>
#include <boost/foreach.hpp>
#include <Eigen/Geometry>
#include <Eigen/LU>
#include "JPEG.h"
#include "math-helpers.hpp"
#include "main.hpp"
#include "MS3D_ASCII.h"
#include <string>

#define foreach BOOST_FOREACH

using namespace std;
using namespace Eigen;

const unsigned int Shape::max_optimizing_triangles = 3500;

/////////////////////////////////////////////////////////////////////////////////////////////////
//										The Shape Class
/////////////////////////////////////////////////////////////////////////////////////////////////
Shape::Shape() :
	bb_l(1E20, 1E20, 1E20),
	bb_h(-bb_l),
	triangles(0)
{
}

Shape::~Shape()
{
	delete triangles;
}

#if 0
bool Shape::loadFromFile( const char *filename )
{
	FileWrap const fp(fopen(filename, "r"));
	if (fp == NULL)
		return false;

	unsigned int num_vertices;
	fscanf(fp, "%u\n", &num_vertices );

	vertices.resize(num_vertices);
	foreach (Vec& vertex, vertices)
	{
		fscanf(fp, "%f %f %f %f %f\n",
					 &vertex.x,
					 &vertex.y,
					 &vertex.z,
					 &vertex.u,
					 &vertex.v);
	}

	unsigned int num_triangles;
	fscanf(fp, "%u\n", &num_triangles );

	triangles.resize(num_triangles);
	foreach (Tri& triangle, triangles)
	{
		fscanf(fp, "%d %d %d\n",
					 &triangle.v[0],
					 &triangle.v[1],
					 &triangle.v[2]);
	}

	return true;
}

bool Shape::saveToFile(const char* filename)
{
	FileWrap const fp(fopen(filename, "w"));
	if (fp == NULL)
		return false;

	fprintf(fp, "%u\n", static_cast<unsigned int>(vertices.size()));

	foreach (const Vec& vertex, vertices)
	{
		fprintf(fp, "%f %f %f %f %f\n",
						vertex.x,
						vertex.y,
						vertex.z,
						vertex.u,
						vertex.v);
	}

	fprintf(fp, "%u\n", static_cast<unsigned int>(triangles.size()));

	foreach (const Tri& triangle, triangles)
	{
		fprintf(fp, "%d %d %d\n",
						triangle.v[0],
						triangle.v[1],
						triangle.v[2] );
	}

	return true;
}
#endif

void Shape::render() const
{
	if (triangles)
		triangles->draw();
}

size_t Shape::vertex_count() const
{
	return triangles ? triangles->uniqueVertices() : 0;
}

size_t Shape::index_count() const
{
	return triangles ? triangles->drawnVertices() : 0;
}

bool Shape::loadFromMs3dAsciiSegment(FILE* file, const Eigen::Matrix4f& transform)
{
	Eigen::Transform3f normals_transform;
	{
		Eigen::Matrix4f normals_transform_matrix;
		transform.computeInverse(&normals_transform_matrix);
		normals_transform_matrix.transposeInPlace();

		normals_transform = normals_transform_matrix;
	}
	Eigen::Transform3f vertex_transform(transform);

	bb_l = Vector3f(1E20,1E20,1E20);
	bb_h = -bb_l;

	char szLine[256] = { 0 };

	int nFlags, nIndex;


	// vertices

	if (!fgets(szLine, sizeof(szLine), file))
	{
		return false;
	}

	unsigned int num_vertices;
	if (sscanf(szLine, "%u", &num_vertices) != 1)
	{
		return false;
	}

	std::vector<Eigen::Vector3f> parsingVertices;
	std::vector<Eigen::Vector2f> parsingTexcoords;

	for (size_t j = 0; j < num_vertices; ++j)
	{
		Eigen::Vector3f vertex;
		Eigen::Vector2f texcoord;

		if (!fgets(szLine, sizeof(szLine), file))
		{
			return false;
		}
		float f;
		if (sscanf(szLine, "%d %f %f %f %f %f %f",
								&nFlags,
								&vertex.x(), &vertex.y(), &vertex.z(),
								&texcoord.x(), &texcoord.y(), &f
							 ) != 7)
		{
			return false;
		}
		/// dizekat: apply vertice transform
		vertex = vertex_transform * vertex;
		/// done transform.

		bb_l.x() = min(bb_l.x(), vertex.x());
		bb_l.y() = min(bb_l.y(), vertex.y());
		bb_l.z() = min(bb_l.z(), vertex.z());

		bb_h.x() = max(bb_h.x(), vertex.x());
		bb_h.y() = max(bb_h.y(), vertex.y());
		bb_h.z() = max(bb_h.z(), vertex.z());

		// adjust the y direction of the texture coordinate
		texcoord.y() = 1.f - texcoord.y();

		parsingVertices.push_back(vertex);
		parsingTexcoords.push_back(texcoord);
	}


	// normals

	if (!fgets(szLine, sizeof(szLine), file))
	{
		return false;
	}

	unsigned int num_normals;
	if (sscanf(szLine, "%u", &num_normals) != 1)
	{
		return false;
	}

	std::vector<Eigen::Vector3f> parsingNormals;

	for (size_t j = 0; j < num_normals; ++j)
	{
		Eigen::Vector3f normal;

		if (!fgets(szLine, sizeof(szLine), file))
		{
			return false;
		}

		if (sscanf(szLine, "%f %f %f",
								&normal.x(), &normal.y(), &normal.z()) != 3)
		{
			return false;
		}

		/// dizekat: apply vertice transform
		normal = normals_transform * normal;
		normal.normalize();
		/// done transform.

		parsingNormals.push_back(normal);
	}

	// triangles

	if (!fgets(szLine, sizeof(szLine), file))
	{
		return false;
	}

	unsigned int num_triangles;
	if (sscanf(szLine, "%u", &num_triangles) != 1)
	{
		return false;
	}

	delete triangles;
	/* Make sure triangles doesn't point to invalid memory if an exception
	 * gets thrown.
	 */
	triangles = 0;

	const bool optimize_triangles = (num_triangles <= max_optimizing_triangles);
	triangles = new TriangleArray<unsigned int, float, float, float>(optimize_triangles);
	triangles->UseVBOs(use_vbos);

	if (!optimize_triangles)
	{
		std::cerr << "warning: will not optimize this model for faster rendering because it exceeds the maximum amount of optimisable triangles (" << max_optimizing_triangles << ").\n";
	}

	for (size_t j = 0; j < num_triangles; ++j)
	{
		struct
		{
			unsigned int v[3], n[3];
		} triangle;

		if (!fgets(szLine, sizeof(szLine), file))
		{
			return false;
		}

		if (sscanf(szLine, "%d %d %d %d %d %d %d %d",
								&nFlags,
								&triangle.v[0], &triangle.v[1], &triangle.v[2],
								&triangle.n[0], &triangle.n[1], &triangle.n[2],
								&nIndex
							 ) != 8)
		{
			return false;
		}
		assert(triangle.v[0] < parsingVertices.size());
		assert(triangle.v[1] < parsingVertices.size());
		assert(triangle.v[2] < parsingVertices.size());
		assert(triangle.n[0] < parsingNormals.size());
		assert(triangle.n[1] < parsingNormals.size());
		assert(triangle.n[2] < parsingNormals.size());

		// Add all three triangle's corners
		boost::array<Eigen::Vector3f, 3> vertices =
		{{
			parsingVertices[triangle.v[0]],
			parsingVertices[triangle.v[1]],
			parsingVertices[triangle.v[2]],
		}};
		boost::array<Eigen::Vector2f, 3> texcoords =
		{{
			parsingTexcoords[triangle.v[0]],
			parsingTexcoords[triangle.v[1]],
			parsingTexcoords[triangle.v[2]],
		}};
		boost::array<Eigen::Vector3f, 3> normals =
		{{
			parsingNormals[triangle.n[0]],
			parsingNormals[triangle.n[1]],
			parsingNormals[triangle.n[2]],
		}};

		triangles->AddTriangle(vertices.data(), texcoords.data(), normals.data());
	}

	return true;
}

void Material::activate() const
{
	glMaterialfv( GL_FRONT, GL_AMBIENT, Ambient );
	glMaterialfv( GL_FRONT, GL_DIFFUSE, Diffuse );
	glMaterialfv( GL_FRONT, GL_SPECULAR, Specular );
	glMaterialfv( GL_FRONT, GL_EMISSION, Emissive );
	glMaterialf( GL_FRONT, GL_SHININESS, Shininess );

	texture.bind();
}

bool Material::loadFromMs3dAsciiSegment( FILE *file, std::string path_ )
{
	path=path_;
	char szLine[256];

	// name
	if (!fgets(szLine, sizeof(szLine), file))
		return false;
	if (sscanf(szLine, "\"%[^\"]\"", Name) != 1)
		return false;

	// ambient
	if (!fgets(szLine, sizeof(szLine), file))
		return false;

	if (sscanf(szLine, "%f %f %f %f", &Ambient[0], &Ambient[1], &Ambient[2], &Ambient[3]) != 4)
		return false;

	// diffuse
	if (!fgets(szLine, sizeof(szLine), file))
		return false;
	if (sscanf(szLine, "%f %f %f %f", &Diffuse[0], &Diffuse[1], &Diffuse[2], &Diffuse[3]) != 4)
		return false;

	// specular
	if (!fgets(szLine, sizeof(szLine), file))
		return false;
	if (sscanf(szLine, "%f %f %f %f", &Specular[0], &Specular[1], &Specular[2], &Specular[3]) != 4)
		return false;

	// emissive
	if (!fgets(szLine, sizeof(szLine), file))
		return false;
	if (sscanf(szLine, "%f %f %f %f", &Emissive[0], &Emissive[1], &Emissive[2], &Emissive[3]) != 4)
		return false;

	// shininess
	if (!fgets(szLine, sizeof(szLine), file))
		return false;
	if (sscanf(szLine, "%f", &Shininess) != 1)
		return false;

	// transparency
	if (!fgets(szLine, sizeof(szLine), file))
		return false;
	if (sscanf(szLine, "%f", &Transparency) != 1)
		return false;

	// diffuse texture
	if (!fgets(szLine, sizeof(szLine), file))
		return false;
	strcpy(DiffuseTexture, "");
	sscanf(szLine, "\"%[^\"]\"", DiffuseTexture);

	// alpha texture
	if (!fgets(szLine, sizeof(szLine), file))
		return false;
	strcpy(AlphaTexture, "");
	sscanf(szLine, "\"%[^\"]\"", AlphaTexture);

	reloadTexture();

	return true;
}

void Material::reloadTexture( void )
{
	if ( strlen(DiffuseTexture) > 0 )
	{
		std::string tmp(path+DiffuseTexture);
		texture = Texture(Image::LoadJPG(tmp.c_str(), true));
	}
	else
	{
		texture = Texture();
	}
}

Model::Model():
	bb_l(0, 0, 0),
	bb_h(0, 0, 0)
{
}

bool Model::loadFromMs3dAsciiFile(const char* filename, const Eigen::Matrix4f& transform)
{
	path = filename;
	/// i'd use rfind but duh it cant search for both \\ and /
	int path_length;
	for (path_length = path.size() - 1; path_length >= 0; --path_length)
	{
		if (path[path_length]=='/'
		 || path[path_length]=='\\')
		{
			break;
		}
	}
	path.resize(path_length + 1);/// downsize the string to cut out name from path

	bb_l = Vector3f(1E20,1E20,1E20);
	bb_h = -bb_l;

	char	szLine[256];
	int		nFlags;

	FileWrap const file(fopen(filename, "rt"));
	if (!file)
		return false;

	while (fgets(szLine, sizeof(szLine), file) != NULL)
	{
		if (!strncmp (szLine, "//", 2))
			continue;

		unsigned int num_shapes;
		if (sscanf(szLine, "Meshes: %u", &num_shapes) == 1)
		{
			char	szName[MS_MAX_NAME];

			shapes.resize(num_shapes);
			material_indices.resize(num_shapes);

			for (size_t i = 0; i < num_shapes; ++i)
			{
				Shape& shape = shapes[i];
				int& material_index = material_indices[i];

				if (!fgets(szLine, sizeof(szLine), file))
				{
					return false;
				}

				// mesh: name, flags, material index
				if (sscanf(szLine, "\"%[^\"]\" %d %d",szName, &nFlags, &material_index) != 3)
				{
					return false;
				}

				if (!shape.loadFromMs3dAsciiSegment(file, transform))
				{
					return false;
				}

				bb_l.x() = min(bb_l.x(), shape.bb_l.x());
				bb_l.y() = min(bb_l.y(), shape.bb_l.y());
				bb_l.z() = min(bb_l.z(), shape.bb_l.z());

				bb_h.x() = max(bb_h.x(), shape.bb_h.x());
				bb_h.y() = max(bb_h.y(), shape.bb_h.y());
				bb_h.z() = max(bb_h.z(), shape.bb_h.z());
			}
			continue;
		}


		// materials
		unsigned int num_materials;
		if (sscanf(szLine, "Materials: %u", &num_materials) == 1)
		{
			materials.resize(num_materials);

			foreach (Material& material, materials)
			{
				if (!material.loadFromMs3dAsciiSegment(file, path))
				{
					return false;
				}
			}
		}
	}

#ifdef DEBUG
	size_t vertices = 0, indices = 0;
	cerr << "Finished loading model " << filename << '\n';
	foreach (const Shape& shape, shapes)
	{
		cerr << shape.vertex_count() << " vertices\n"
		     << shape.index_count() << " indices\n";
		vertices += shape.vertex_count();
		indices += shape.index_count();
	}
	cerr << "------------------------\n"
	     << vertices << " vertices\n"
	     << indices << " indices\n\n";
#endif

	return true;
}

void Model::reloadTextures()
{
	foreach (Material& material, materials)
		material.reloadTexture();
}

void Model::render() const
{
	for (size_t k = 0; k < shapes.size(); ++k)	// for each shape
	{
		const Shape& shape = shapes[k];
		int materialIndex = material_indices[k];
		if ( materialIndex >= 0 )
		{
			materials[materialIndex].activate();
		}
		else
		{
			// Material properties?
			glDisable( GL_TEXTURE_2D );
		}

		shape.render();
	}
}
