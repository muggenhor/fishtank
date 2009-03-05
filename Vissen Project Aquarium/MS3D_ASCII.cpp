#include <boost/foreach.hpp>
#include "JPEG.h"
#include "MS3D_ASCII.h"
#include <string>

#define foreach BOOST_FOREACH

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
	FILE* const fp = fopen(filename, "r");
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

	fclose(fp);

	return true;
}

bool Shape::saveToFile(const char* filename)
{
	FILE* const fp = fopen(filename, "w");
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

	fclose(fp);
	return true;
}
#endif

void Shape::render( void )
{
	glBegin(GL_TRIANGLES);
	foreach (const Tri& tri, triangles)
	{
		// Draw all 3 vertices of the triangle
		for (unsigned int j = 0; j < 3; ++j)
		{
			const Normal& N = normals[tri.n[j]];
			glNormal3f(N.x, N.y, N.z);

			const Vec& vec = vertices[tri.v[j]];
			glTexCoord2f(vec.u, vec.v);

			glVertex3f(vec.x, vec.y, vec.z);
		}
	}
	glEnd();
}
using namespace math3;


bool Shape::loadFromMs3dAsciiSegment( FILE *file , const math3::Matrix4x4f &transform)
{
	Matrix4x4f normals_transform=Transpose(Inverse(transform));

	bb_l=Vec3d(1E20,1E20,1E20);
	bb_h=-bb_l;

	int nFlags, nIndex;


	// vertices

	size_t num_vertices;
	if (fscanf(file, "%zu", &num_vertices) != 1)
	{
		return false;
	}
	vertices.resize(num_vertices);

	for (size_t j = 0; j < num_vertices; ++j)
	{
		float f;
		if (fscanf(file, "%d %f %f %f %f %f %f",
								&nFlags,
								&vertices[j].x, &vertices[j].y, &vertices[j].z,
								&vertices[j].u, &vertices[j].v, &f
							 ) != 7)
		{
			return false;
		}
		/// dizekat: apply vertice transform
		Vec3f tmp1(vertices[j].x, vertices[j].y, vertices[j].z);
		Vec3f tmp2=Mulw1(transform,tmp1);
		/// done transform.

		vertices[j].x=tmp2.x;
		vertices[j].y=tmp2.y;
		vertices[j].z=tmp2.z;

		if(tmp2.x<bb_l.x)bb_l.x=tmp2.x;
		if(tmp2.y<bb_l.y)bb_l.y=tmp2.y;
		if(tmp2.z<bb_l.z)bb_l.z=tmp2.z;

		if(tmp2.x>bb_h.x)bb_h.x=tmp2.x;
		if(tmp2.y>bb_h.y)bb_h.y=tmp2.y;
		if(tmp2.z>bb_h.z)bb_h.z=tmp2.z;



		// adjust the y direction of the texture coordinate
		vertices[j].v = 1.0f - vertices[j].v;
	}


	// normals

	size_t num_normals;
	if (fscanf(file, "%zu", &num_normals) != 1)
	{
		return false;
	}
	normals.resize(num_normals);

	for (size_t j = 0; j < num_normals; ++j)
	{
		if (fscanf(file, "%f %f %f",
								&normals[j].x, &normals[j].y, &normals[j].z) != 3)
		{
			return false;
		}

				/// dizekat: apply vertice transform
		Vec3f tmp1(normals[j].x, normals[j].y, normals[j].z);
		Vec3f tmp2=Mulw0(normals_transform,tmp1);
		Normalize(tmp2);
		normals[j].x=tmp2.x;
		normals[j].y=tmp2.y;
		normals[j].z=tmp2.z;
		/// done transform.

	}

	// triangles

	size_t num_triangles;
	if (fscanf(file, "%zu", &num_triangles) != 1)
	{
		return false;
	}
	triangles.resize(num_triangles);

	for (size_t j = 0; j < num_triangles; ++j)
	{
		if (fscanf(file, "%d %d %d %d %d %d %d %d",
								&nFlags,
								&triangles[j].v[0], &triangles[j].v[1], &triangles[j].v[2],
								&triangles[j].n[0], &triangles[j].n[1], &triangles[j].n[2],
								&nIndex
							 ) != 8)
		{
			return false;
		}
		assert(triangles[j].v[0] >= 0);
		assert(triangles[j].v[0] < num_vertices);
		assert(triangles[j].v[1] >= 0);
		assert(triangles[j].v[1] < num_vertices);
		assert(triangles[j].v[2] >= 0);
		assert(triangles[j].v[2] < num_vertices);
	}

	return true;
}

void Material::activate( void )
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

	// name
	if (fscanf(file, "\"%[^\"]\"", Name) != 1)
		return false;

	// ambient
	if (fscanf(file, "%f %f %f %f", &Ambient[0], &Ambient[1], &Ambient[2], &Ambient[3]) != 4)
		return false;

	// diffuse
	if (fscanf(file, "%f %f %f %f", &Diffuse[0], &Diffuse[1], &Diffuse[2], &Diffuse[3]) != 4)
		return false;

	// specular
	if (fscanf(file, "%f %f %f %f", &Specular[0], &Specular[1], &Specular[2], &Specular[3]) != 4)
		return false;

	// emissive
	if (fscanf(file, "%f %f %f %f", &Emissive[0], &Emissive[1], &Emissive[2], &Emissive[3]) != 4)
		return false;

	// shininess
	if (fscanf(file, "%f", &Shininess) != 1)
		return false;

	// transparency
	if (fscanf(file, "%f", &Transparency) != 1)
		return false;

	// diffuse texture
	strcpy(DiffuseTexture, "");
	fscanf(file, "\"%[^\"]\"", DiffuseTexture);

	// alpha texture
	strcpy(AlphaTexture, "");
	fscanf(file, "\"%[^\"]\"", AlphaTexture);

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




/////////////////////////////////////////////////////////////////////////////////////////////////
//										The Model Class
/////////////////////////////////////////////////////////////////////////////////////////////////
Model::Model():
	bb_l(0, 0, 0),
	bb_h(0, 0, 0)
{
}

bool Model::loadFromMs3dAsciiFile( const char *filename, const math3::Matrix4x4f &transform )
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

	bb_l=Vec3d(1E20,1E20,1E20);
	bb_h=-bb_l;

	bool	bError = false;
	char	szLine[256];
	int		nFlags;

	FILE *file = fopen (filename, "rt");
	if (!file)
		return false;

	while (fgets (szLine, 256, file) != NULL  && !bError)
	{
		if (!strncmp (szLine, "//", 2))
			continue;

		size_t num_shapes;
		if (sscanf (szLine, "Meshes: %zu", &num_shapes) == 1)
		{
			char	szName[MS_MAX_NAME];

			shapes.resize(num_shapes);
			material_indices.resize(num_shapes);

			for (size_t i = 0; i < num_shapes && !bError; ++i)
			{
				Shape& shape = shapes[i];
				int& material_index = material_indices[i];

				if (!fgets (szLine, 256, file))
				{
					bError = true;
					break;
				}

				// mesh: name, flags, material index
				if (sscanf(szLine, "\"%[^\"]\" %d %d",szName, &nFlags, &material_index) != 3)
				{
					bError = true;
					break;
				}

				if (!shape.loadFromMs3dAsciiSegment(file, transform))
				{
					bError = true;
					break;
				}

				if (shape.bb_l.x < bb_l.x)
					bb_l.x = shape.bb_l.x;
				if (shape.bb_l.y < bb_l.y)
					bb_l.y = shape.bb_l.y;
				if (shape.bb_l.z < bb_l.z)
					bb_l.z = shape.bb_l.z;

				if (shape.bb_h.x > bb_h.x)
					bb_h.x = shape.bb_h.x;
				if (shape.bb_h.y > bb_h.y)
					bb_h.y = shape.bb_h.y;
				if (shape.bb_h.z > bb_h.z)
					bb_h.z = shape.bb_h.z;

			}
			continue;
		}


		// materials
		size_t num_materials;
		if (sscanf (szLine, "Materials: %zu", &num_materials) == 1)
		{
			materials.resize(num_materials);

			foreach (Material& material, materials)
			{
				if (!material.loadFromMs3dAsciiSegment(file, path))
				{
					bError = true;
					break;
				}
			}
		}
	}

	fclose (file);


	return true;
}

void Model::reloadTextures()
{
	foreach (Material& material, materials)
		material.reloadTexture();
}

void Model::render(const math3::Vec3f &wiggle_freq, const math3::Vec3f &wiggle_dir, double wiggle_phase, double turn)
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


	double a=2.0*Length(wiggle_dir)+(1E-20);/// prevent divisions by zero
	double b=Length(wiggle_freq)+(1E-20);
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

		glBegin(GL_TRIANGLES);
		foreach (const Tri& tri, shape.triangles)
		{
			for (unsigned int j = 0; j < 3; ++j)						// 3 vertices of the triangle
			{
				const Normal& N = shape.normals[tri.n[j]];
				/// todo: also apply wiggle to normals? (thats much harder.)
				glNormal3f(N.x, N.y, N.z);			// set normal vector  (object space)

				const Vec& vec = shape.vertices[tri.v[j]];	// pointer to vertex
				glTexCoord2f (vec.u, vec.v);			// texture coordinate

				/// wiggle position
				Vec3f pos(vec.x , vec.y, vec.z);
				float alpha=DotProd(pos, wiggle_freq);

				Vec3f wiggled_pos=pos + wiggle_dir*float(sin(alpha+wiggle_phase)) ;

				/// approximate the elliptic integral.
				wiggled_pos.x=wiggled_pos.x*p+sin(2*alpha+wiggle_phase)*s_a;
				/// approximate the elliptic integral, weirder but better looking (?).
				//wiggled_pos+=wiggle_freq*float(sin(2*alpha+wiggle_phase)*s_a/b);

				/// turns:
				if(fabs(turn)>1E-5){
					double i_turn=1.0/turn;
					double turn_a=wiggled_pos.x*turn;
					if(turn>0){
						wiggled_pos.x=sin(turn_a)*(i_turn+wiggled_pos.z);
						wiggled_pos.z=cos(turn_a)*(i_turn+wiggled_pos.z)-i_turn;
					}else{
						turn_a=-turn_a;
						i_turn=-i_turn;
						wiggled_pos.x=sin(turn_a)*(i_turn-wiggled_pos.z);
						wiggled_pos.z=i_turn-cos(turn_a)*(i_turn-wiggled_pos.z);
					}
				}


				glVertex3f( wiggled_pos.x , wiggled_pos.y, wiggled_pos.z);

			}
		}
		glEnd();
	}
}
