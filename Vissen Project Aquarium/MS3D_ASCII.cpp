#include <algorithm>
#include <boost/foreach.hpp>
#include <Eigen/Geometry>
#include <Eigen/LU>
#include "JPEG.h"
#include "math-helpers.hpp"
#include "MS3D_ASCII.h"
#include <string>

#define foreach BOOST_FOREACH

using namespace std;
using namespace Eigen;

/////////////////////////////////////////////////////////////////////////////////////////////////
//										The Shape Class
/////////////////////////////////////////////////////////////////////////////////////////////////
Shape::Shape() :
	bb_l(1E20, 1E20, 1E20),
	bb_h(-bb_l)
{
}

#if 0
bool Shape::loadFromFile( const char *filename )
{
	FileWrap const fp(fopen(filename, "r"));
	if (fp == NULL)
		return false;

	size_t num_vertices;
	fscanf(fp, "%zu\n", &num_vertices );

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

	size_t num_triangles;
	fscanf(fp, "%zu\n", &num_triangles );

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

	fprintf(fp, "%zu\n", vertices.size());

	foreach (const Vec& vertex, vertices)
	{
		fprintf(fp, "%f %f %f %f %f\n",
						vertex.x,
						vertex.y,
						vertex.z,
						vertex.u,
						vertex.v);
	}

	fprintf(fp, "%zu\n", triangles.size());

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
	if (indices.empty())
		return;

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, vertices[0].data());
	glNormalPointer(GL_FLOAT, 0, normals[0].data());
	glTexCoordPointer(2, GL_FLOAT, 0, texcoords[0].data());

	if (GLEE_VERSION_2_1)
		glDrawRangeElements(GL_TRIANGLES, 0, vertices.size() - 1, indices.size(), GL_UNSIGNED_INT, &indices[0]);
	else
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, &indices[0]);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
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

	size_t num_vertices;
	if (sscanf(szLine, "%zu", &num_vertices) != 1)
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

	size_t num_normals;
	if (sscanf(szLine, "%zu", &num_normals) != 1)
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

	size_t num_triangles;
	if (sscanf(szLine, "%zu", &num_triangles) != 1)
	{
		return false;
	}

	vertices.clear();
	texcoords.clear();
	normals.clear();
	indices.clear();

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
		for (unsigned int i = 0; i < 3; ++i)
			AddPoint(parsingVertices[triangle.v[i]],
			         parsingTexcoords[triangle.v[i]],
			         parsingNormals[triangle.n[i]]);
	}

	return true;
}

void Shape::AddPoint(const Eigen::Vector3f& vertex, const Eigen::Vector2f& texcoord, const Eigen::Vector3f& normal)
{
	unsigned int index = 0;

	// Try to find an instance for the given vertex
	for (; index < vertices.size(); ++index)
	{
		if (vertices[index]  == vertex
		 && texcoords[index] == texcoord
		 && normals[index]   == normal)
		{
			/* If we found a vertex that's the same then add the
			 * index to the index table and bail out.
			 */
			indices.push_back(index);
			return;
		}
	}

	vertices.push_back(vertex);
	texcoords.push_back(texcoord);
	normals.push_back(normal);

	assert(vertices.size()  - 1 == index);
	assert(texcoords.size() - 1 == index);
	assert(normals.size()   - 1 == index);

	indices.push_back(index);
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
		texture = Texture(Image::LoadJPG(tmp.c_str()));
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

		size_t num_shapes;
		if (sscanf(szLine, "Meshes: %zu", &num_shapes) == 1)
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
		size_t num_materials;
		if (sscanf(szLine, "Materials: %zu", &num_materials) == 1)
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

	return true;
}

void Model::reloadTextures()
{
	foreach (Material& material, materials)
		material.reloadTexture();
}

void Model::render(const Eigen::Vector3f& wiggle_freq, const Eigen::Vector3f& wiggle_dir, double wiggle_phase, double turn) const
{
	/*
	// approximate elliptic integral with sine... just an approximation, not correct formula

	a*sin(l*b) ' = a*b*cos(l*b)

dl=sqrt((a*b*cos(x*b))^2 + 1)*dt

dl min =dx
dl max = sqrt((a*b)^2 + 1)*dx

dx/dl max = 1  (x*b=pi/2)
dx/dl min = 1/sqrt((a*b)^2 + 1)  (l*b=0)
q=(1-1/sqrt((a*b)^2 + 1)))/2
p=(1/sqrt((a*b)^2 + 1)+1)/2
dx/dl=q*cos(2*l*b)+p
x=q*sin(2*b*l)/(2*b) + p*l
	*/

	const double i_turn = 1. / turn;

	double a = 2.0 * wiggle_dir.norm() + 1E-20; /// prevent divisions by zero
	double b = wiggle_freq.norm() + 1E-20;
	double c=1.0/sqrt(a*a*b*b+1);
	double q=0.5*(1-c);
	double p=0.5*(c+1);
	double s_a=0.5*q/b;

	// FIXME: O(n^3) performance
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

		std::vector<Eigen::Vector3f> wiggledVertices;
		wiggledVertices.reserve(shape.vertices.size());
		foreach (const Eigen::Vector3f& vertex, shape.vertices)
		{
			// Wiggle position
			const float alpha = vertex.dot(wiggle_freq);
			Eigen::Vector3f wiggled_pos = vertex + wiggle_dir * float(sin(alpha + wiggle_phase));

			// approximate the elliptic integral.
			wiggled_pos.x() += p * sin(2 * alpha + wiggle_phase) * s_a;

			/// approximate the elliptic integral, weirder but better looking (?).
			//wiggled_pos += wiggle_freq * float(sin(2 * alpha + wiggle_phase) * s_a / b);

			// turns:
			if (fabs(turn) > 1E-5)
			{
				double turn_a = wiggled_pos.x() * turn;

				if (turn > 0)
				{
					wiggled_pos.x() = sin(turn_a) * (i_turn + wiggled_pos.z());
					wiggled_pos.z() = cos(turn_a) * (i_turn + wiggled_pos.z()) - i_turn;
				}
				else
				{
					wiggled_pos.x() = sin(-turn_a) * (-i_turn - wiggled_pos.z());
					wiggled_pos.z() = -i_turn - cos(-turn_a) * (-i_turn - wiggled_pos.z());
				}
			}

			wiggledVertices.push_back(wiggled_pos);
		}

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glVertexPointer(3, GL_FLOAT, 0, wiggledVertices[0].data());
		/// todo: also apply wiggle to normals? (thats much harder.)
		glNormalPointer(GL_FLOAT, 0, shape.normals[0].data());
		glTexCoordPointer(2, GL_FLOAT, 0, shape.texcoords[0].data());

		glDrawElements(GL_TRIANGLES, shape.indices.size(), GL_UNSIGNED_INT, &shape.indices[0]);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
}
