

#include "JPEG.h"
#include "MS3D_ASCII.h"


/////////////////////////////////////////////////////////////////////////////////////////////////
//										The Shape Class
/////////////////////////////////////////////////////////////////////////////////////////////////
Shape::Shape()
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
	if(vertices != NULL)
		delete[] vertices;
	if(triangles != NULL)
		delete[] triangles;
	if(normals != NULL)
		delete[] normals;
}


bool Shape::loadFromFile( const char *filename )
{
	FILE	*fp;
	int	i;

	fp = fopen(filename, "r");
	if(fp == NULL)
		return false;

	fscanf(fp, "%d\n", &num_vertices );

	vertices = new Vec[num_vertices];

	for(i = 0 ; i < num_vertices; i++)
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
	for(i = 0 ; i < num_triangles; i++)
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
	if(fp == NULL)
		return false;

	fprintf(fp, "%d\n", num_vertices );

	for(i = 0 ; i < num_vertices; i++)
	{
		fprintf(fp, "%f %f %f %f %f\n",
			vertices[i].x,
			vertices[i].y,
			vertices[i].z,
			vertices[i].u,
			vertices[i].v );
	}

	fprintf(fp, "%d\n", num_triangles );

	for(i = 0 ; i < num_triangles; i++)
	{
		fprintf(fp, "%d %d %d\n",
			triangles[i].v[0],
			triangles[i].v[1],
			triangles[i].v[2] );
	}
	fclose(fp);
	return true;
}


void Shape::render( void )
{
	int	i,j;
	Tri	*tri;
	Vec	*vec;
	Normal *N;

	glBegin(GL_TRIANGLES);
	for(i = 0; i < num_triangles; i++)				// for each triangle
	{
		tri = triangles + i;						// pointer to triangle

		for(j = 0; j < 3; j++)						// 3 vertices of the triangle
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



bool Shape::loadFromMs3dAsciiSegment( FILE *file )
{
    bool bError = false;
    char szLine[256];
    for(int i=0;i<256;++i)szLine[i]=0;

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
                    if (sscanf (szLine, "%d %f %f %f %f %f %d",
                        &nFlags,
                        &vertices[j].x, &vertices[j].y, &vertices[j].z,
                        &vertices[j].u, &vertices[j].v, &f
                        ) != 7)
					{
						return false;
					}
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

	if ( texture > 0 )
	{
		glBindTexture( GL_TEXTURE_2D, texture );
		glEnable( GL_TEXTURE_2D );
	}
	else
		glDisable( GL_TEXTURE_2D );
}

bool Material::loadFromMs3dAsciiSegment( FILE *file )
{
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
	if( strlen(DiffuseTexture) > 0 )
	{
		JPEG_Texture(&texture,DiffuseTexture,0);
	}
	else	texture = 0;
}




/////////////////////////////////////////////////////////////////////////////////////////////////
//										The Model Class
/////////////////////////////////////////////////////////////////////////////////////////////////
Model::Model()
{
	num_shapes = 0;
	shapes = NULL;
	num_materials = 0;
	materials = NULL;
}

Model::~Model()
{

	if(shapes != NULL)
	{
		delete[] shapes;
		shapes = NULL;
	}
	if(materials != NULL)
	{
		delete[] materials;
		materials = NULL;
	}
	if(material_indices != NULL)
	{
		delete[] material_indices;
		material_indices = NULL;
	}
}

bool Model::loadFromMs3dAsciiFile( const char *filename )
{
    bool	bError = false;
    char	szLine[256];
    int		nFlags, nIndex, i;

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

				if( ! shapes[i].loadFromMs3dAsciiSegment(file) )
                {
                    bError = true;
                    break;
                }
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
				if( ! materials[i].loadFromMs3dAsciiSegment(file) )
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

	for(i = 0; i < num_materials; i++)	// for each shape
	{
		materials[i].reloadTexture();
	}
}

#if 0
void Model::render( void )
{
	int	i;

	for(i = 0; i < num_shapes; i++)	// for each shape
	{
		int materialIndex = material_indices[i];
		if ( materialIndex >= 0 )
		{
			materials[materialIndex].activate();
		}
		else
		{
			glDisable( GL_TEXTURE_2D );
		}
		shapes[i].render();
	}
}
#endif

void Model::render( void )
{
	int	k;
	int	i, j;
	Tri	*tri;
	Vec	*vec;
	Normal *N;

	for(k = 0; k < num_shapes; k++)	// for each shape
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
		for(i = 0; i < shapes[k].num_triangles; i++)	// for each triangle
		{
			tri = shapes[k].triangles + i;				// pointer to triangle

			for(j = 0; j < 3; j++)						// 3 vertices of the triangle
			{
				N = shapes[k].normals + tri->n[j];
				glNormal3f(N->x, N->y, N->z);			// set normal vector  (object space)

				vec = shapes[k].vertices + tri->v[j];	// pointer to vertex
				glTexCoord2f (vec->u, vec->v);			// texture coordinate

				glVertex3f( vec->x , vec->y, vec->z);

			}

		}
		glEnd();
	}
}

