#include "JPEG.h"
#include "MS3D_ASCII.h"

#include <string>

/////////////////////////////////////////////////////////////////////////////////////////////////
//										The Shape Class
/////////////////////////////////////////////////////////////////////////////////////////////////
Shape::Shape():
bb_l(1E20,1E20,1E20),
bb_h(-bb_l)
{
	num_vertices = 0;
	vertices = NULL;
	num_triangles = 0;
	triangles = NULL;
	num_normals = 0;
	normals = NULL;
}

Shape::~Shape()
{
	if (vertices != NULL)
		delete[] vertices;
	if (triangles != NULL)
		delete[] triangles;
	if (normals != NULL)
		delete[] normals;
}

/*
bool Shape::loadFromFile( const char *filename )
{
	FILE	*fp;
	int	i;

	fp = fopen(filename, "r");
	if (fp == NULL)
		return false;

	fscanf(fp, "%d\n", &num_vertices );

	vertices = new Vec[num_vertices];

	for (i = 0 ; i < num_vertices; i++)
	{
		fscanf(fp, "%f %f %f %f %f\n",
					 &vertices[i].x,
					 &vertices[i].y,
					 &vertices[i].z,
					 &vertices[i].u,
					 &vertices[i].v );
	}

	fscanf(fp, "%d\n", &num_triangles );

	triangles = new Tri[num_triangles];
	for (i = 0 ; i < num_triangles; i++)
	{
		fscanf(fp, "%d %d %d\n",
					 &triangles[i].v[0],
					 &triangles[i].v[1],
					 &triangles[i].v[2] );
	}

	fclose(fp);

	return true;
}


bool Shape::saveToFile( const char *filename )
{
	FILE	*fp;
	int	i;

	fp = fopen(filename, "w");
	if (fp == NULL)
		return false;

	fprintf(fp, "%d\n", num_vertices );

	for (i = 0 ; i < num_vertices; i++)
	{
		fprintf(fp, "%f %f %f %f %f\n",
						vertices[i].x,
						vertices[i].y,
						vertices[i].z,
						vertices[i].u,
						vertices[i].v );
	}

	fprintf(fp, "%d\n", num_triangles );

	for (i = 0 ; i < num_triangles; i++)
	{
		fprintf(fp, "%d %d %d\n",
						triangles[i].v[0],
						triangles[i].v[1],
						triangles[i].v[2] );
	}
	fclose(fp);
	return true;
}
*/

void Shape::render( void )
{
	int	i,j;
	Tri	*tri;
	Vec	*vec;
	Normal *N;

	glBegin(GL_TRIANGLES);
	for (i = 0; i < num_triangles; i++)				// for each triangle
	{
		tri = triangles + i;						// pointer to triangle

		for (j = 0; j < 3; j++)						// 3 vertices of the triangle
		{
			N = normals + tri->n[j];
			glNormal3f(N->x, N->y, N->z);			// set normal vector  (object space)

			vec = vertices + tri->v[j];				// pointer to vertex
			glTexCoord2f (vec->u, vec->v);			// texture coordinate

			glVertex3f( vec->x, vec->y, vec->z );	// 3d coordinate (object space)
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

	char szLine[256];
	for (int i=0;i<256;++i)szLine[i]=0;

	int nFlags, nIndex, j;


	// vertices

	if (!fgets (szLine, 256, file))
	{
		return false;
	}

	if (sscanf (szLine, "%d", &num_vertices) != 1)
	{
		return false;
	}
	vertices = new Vec[num_vertices];

	for (j = 0; j < num_vertices; j++)
	{
		if (!fgets (szLine, 256, file))
		{
			return false;
		}
		float f;
		if (sscanf (szLine, "%d %f %f %f %f %f %f",
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

	if (!fgets (szLine, 256, file))
	{
		return false;
	}

	if (sscanf (szLine, "%d", &num_normals) != 1)
	{
		return false;
	}
	normals = new Normal[num_normals];

	for (j = 0; j < num_normals; j++)
	{
		if (!fgets (szLine, 256, file))
		{
			return false;
		}

		if (sscanf (szLine, "%f %f %f",
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

	if (!fgets (szLine, 256, file))
	{
		return false;
	}

	if (sscanf (szLine, "%d", &num_triangles) != 1)
	{
		return false;
	}
	triangles = new Tri[num_triangles];

	for (j = 0; j < num_triangles; j++)
	{
		if (!fgets (szLine, 256, file))
		{
			return false;
		}

		if (sscanf (szLine, "%d %d %d %d %d %d %d %d",
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



/////////////////////////////////////////////////////////////////////////////////////////////////
//										The Material Class
/////////////////////////////////////////////////////////////////////////////////////////////////
Material::Material()
{
}

Material::~Material()
{
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
	char szLine[256];

	// name
	if (!fgets (szLine, 256, file))
		return false;
	if (sscanf (szLine, "\"%[^\"]\"", Name) != 1)
		return false;

	// ambient
	if (!fgets (szLine, 256, file))
		return false;

	if (sscanf (szLine, "%f %f %f %f", &Ambient[0], &Ambient[1], &Ambient[2], &Ambient[3]) != 4)
		return false;

	// diffuse
	if (!fgets (szLine, 256, file))
		return false;
	if (sscanf (szLine, "%f %f %f %f", &Diffuse[0], &Diffuse[1], &Diffuse[2], &Diffuse[3]) != 4)
		return false;

	// specular
	if (!fgets (szLine, 256, file))
		return false;
	if (sscanf (szLine, "%f %f %f %f", &Specular[0], &Specular[1], &Specular[2], &Specular[3]) != 4)
		return false;

	// emissive
	if (!fgets (szLine, 256, file))
		return false;
	if (sscanf (szLine, "%f %f %f %f", &Emissive[0], &Emissive[1], &Emissive[2], &Emissive[3]) != 4)
		return false;

	// shininess
	if (!fgets (szLine, 256, file))
		return false;
	if (sscanf (szLine, "%f", &Shininess) != 1)
		return false;

	// transparency
	if (!fgets (szLine, 256, file))
		return false;
	if (sscanf (szLine, "%f", &Transparency) != 1)
		return false;

	// diffuse texture
	if (!fgets (szLine, 256, file))
		return false;
	strcpy(DiffuseTexture, "");
	sscanf (szLine, "\"%[^\"]\"", DiffuseTexture);

	// alpha texture
	if (!fgets (szLine, 256, file))
		return false;
	strcpy(AlphaTexture, "");
	sscanf (szLine, "\"%[^\"]\"", AlphaTexture);

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
	bb_h(0, 0, 0),
	num_shapes(0),
	shapes(NULL),
	material_indices(NULL),
	num_materials(0),
	materials(NULL)
{
}

Model::~Model()
{
	delete [] shapes;
	delete [] materials;
	delete [] material_indices;
}

bool Model::loadFromMs3dAsciiFile( const char *filename, const math3::Matrix4x4f &transform )
{

	path=filename;
	/// i'd use rfind but duh it cant search for both \\ and /
	int i;
	for(i=path.size()-1; i>=0; --i){
		if(path[i]=='/' || path[i]=='\\'){
			break;
		}
	}
	path.resize(i+1);/// downsize the string to cut out name from path

	bb_l=Vec3d(1E20,1E20,1E20);
	bb_h=-bb_l;

	bool	bError = false;
	char	szLine[256];
	int		nFlags, nIndex;

	FILE *file = fopen (filename, "rt");
	if (!file)
		return false;

	while (fgets (szLine, 256, file) != NULL  && !bError)
	{
		if (!strncmp (szLine, "//", 2))
			continue;

		if (sscanf (szLine, "Meshes: %d", &num_shapes) == 1)
		{
			char	szName[MS_MAX_NAME];

			shapes = new Shape[num_shapes];
			material_indices = new int[num_shapes];

			for (i = 0; i < num_shapes && !bError; i++)
			{

				if (!fgets (szLine, 256, file))
				{
					bError = true;
					break;
				}

				// mesh: name, flags, material index
				if (sscanf (szLine, "\"%[^\"]\" %d %d",szName, &nFlags, &nIndex) != 3)
				{
					bError = true;
					break;
				}
				material_indices[i] = nIndex;

				if ( ! shapes[i].loadFromMs3dAsciiSegment(file, transform) )
				{
					bError = true;
					break;
				}

				if(shapes[i].bb_l.x<bb_l.x)bb_l.x=shapes[i].bb_l.x;
				if(shapes[i].bb_l.y<bb_l.y)bb_l.y=shapes[i].bb_l.y;
				if(shapes[i].bb_l.z<bb_l.z)bb_l.z=shapes[i].bb_l.z;

				if(shapes[i].bb_h.x>bb_h.x)bb_h.x=shapes[i].bb_h.x;
				if(shapes[i].bb_h.y>bb_h.y)bb_h.y=shapes[i].bb_h.y;
				if(shapes[i].bb_h.z>bb_h.z)bb_h.z=shapes[i].bb_h.z;

			}
			continue;
		}


		// materials

		if (sscanf (szLine, "Materials: %d", &num_materials) == 1)
		{
			int i;

			materials = new Material[num_materials];

			for (i = 0; i < num_materials && !bError; i++)
			{
				if ( ! materials[i].loadFromMs3dAsciiSegment(file, path) )
				{
					bError = true;
					break;
				}
			}
			continue;
		}

	}

	fclose (file);


	return true;
}




void Model::reloadTextures( void )
{
	int	i;

	for (i = 0; i < num_materials; i++)	// for each shape
	{
		materials[i].reloadTexture();
	}
}

void Model::render(const math3::Vec3f &wiggle_freq, const math3::Vec3f &wiggle_dir, double wiggle_phase, double turn)
{
	int	k;
	int	i, j;
	Tri	*tri;
	Vec	*vec;
	Normal *N;

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
	for (k = 0; k < num_shapes; k++)	// for each shape
	{
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
		for (i = 0; i < shapes[k].num_triangles; i++)	// for each triangle
		{
			tri = shapes[k].triangles + i;				// pointer to triangle

			for (j = 0; j < 3; j++)						// 3 vertices of the triangle
			{
				N = shapes[k].normals + tri->n[j];
				/// todo: also apply wiggle to normals? (thats much harder.)
				glNormal3f(N->x, N->y, N->z);			// set normal vector  (object space)

				vec = shapes[k].vertices + tri->v[j];	// pointer to vertex
				glTexCoord2f (vec->u, vec->v);			// texture coordinate

				/// wiggle position
				Vec3f pos(vec->x , vec->y, vec->z);
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
