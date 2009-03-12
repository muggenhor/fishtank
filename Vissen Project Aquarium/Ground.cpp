#include <Eigen/Geometry>
#include "Ground.h"
#include "AquariumController.h"
#include "JPEG.h"
#include "math-helpers.hpp"

using namespace std;

Ground::Ground(const string &filename, int maxHeight, const std::string &texturename) :
	maxHeight(maxHeight),
	file(filename)
{
	//GenerateGroundFromImage(filename);

	if (!texturename.empty())
	{/// if got texture name
		texture = Texture(Image::LoadJPG(texturename.c_str(), true));
	}
}

void Ground::GenerateGroundFromImage(const string &filename)
{
	Image image = Image::LoadJPG(filename.c_str());
#if 0
	if (error_loading_image)
	{
		widthAmount = 2;
		lengthAmount = 2;
		ground.resize(widthAmount * lengthAmount, -aquariumSize.y() / 2);
		return;
	}
#endif

	widthAmount = image.width;
	lengthAmount = image.height;
	ground.resize(widthAmount * lengthAmount);
	normals.resize(widthAmount * lengthAmount);

	for (int y = 0; y < lengthAmount; y++)
	{
 		for (int x = 0; x < widthAmount; x++)
 		{
			ground[x + y * widthAmount] = -aquariumSize.y() / 2 + ((unsigned char)(image.data[x * 3 + y * image.rowSpan])) / 255.0 * maxHeight;

			// Generate a cache of normals
			normals[x + y * widthAmount] = -(PosAt(x + 1, y) - PosAt(x - 1, y)).cross(PosAt(x, y + 1) - PosAt(x, y - 1)).normalized();
 		}
	}
}

int Ground::HeightAt(int x, int y) const
{
	x = clip(x, 0, widthAmount - 1);
	y = clip(y, 0, lengthAmount - 1);

	return ground[x + y * widthAmount];
}

Eigen::Vector3f Ground::PosAt(int x, int y)
{
	const float relativeX = x * aquariumSize.x() / float(widthAmount - 1) - 0.5 * aquariumSize.x();
	const float relativeY = y * aquariumSize.z() / float(lengthAmount - 1) - 0.5 * aquariumSize.z();

	return Eigen::Vector3f(relativeX, HeightAt(x, y), relativeY);
}

const Eigen::Vector3f& Ground::NormalAt(int x, int y) const
{
	x = clip(x, 0, widthAmount - 1);
	y = clip(y, 0, lengthAmount - 1);

	return normals[x + y * widthAmount];
}

void Ground::Draw()
{
	glDisable(GL_LIGHT1);

	glDisable(GL_LIGHT2);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	static const float lightpos[]={0,200,0,1};

	static const float lightcol[]={1,1,1,1};
	static const float black[]={0,0,0,1};

	glLightfv(GL_LIGHT0,GL_POSITION, lightpos);
	glLightfv(GL_LIGHT0,GL_DIFFUSE, lightcol);
	glLightfv(GL_LIGHT0,GL_AMBIENT, black);
	glLightfv(GL_LIGHT0,GL_SPECULAR, black);

	glLightf(GL_LIGHT0,GL_CONSTANT_ATTENUATION,1.0);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0);

	glEnable ( GL_COLOR_MATERIAL ) ;
	glMaterialfv ( GL_FRONT_AND_BACK, GL_EMISSION, black ) ;
	glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;

	if (texture.is_null_texture())
	{
		glColor3f(1,0.8,0.1);
	}
	else
	{
		texture.bind();
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glScalef(1.0/widthAmount,1.0/lengthAmount,1.0);
		glMatrixMode(GL_MODELVIEW);
		glColor3f(1,1,1);
		//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_MODULATE);
	}

	for (int y = 1; y < lengthAmount; ++y)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for (int x = 0; x < widthAmount; ++x)
		{
			glTexCoord2i(x, y);
			glNormal3fv(NormalAt(x, y).data());
			glVertex3fv(PosAt(x, y).data());

			glTexCoord2i(x, y - 1);
			glNormal3fv(NormalAt(x, y - 1).data());
			glVertex3fv(PosAt(x, y - 1).data());
		}
		glEnd();
	}
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_COLOR_MATERIAL) ;

	if (!texture.is_null_texture())
	{
		glDisable(GL_TEXTURE_2D);
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
	}

	glColor3f(1,1,1);
}
