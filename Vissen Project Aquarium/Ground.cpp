#include "Ground.h"
#include "AquariumController.h"
#include "JPEG.h"

using namespace math3;
using namespace std;

Ground::Ground(const string &filename, int maxHeight, const std::string &texturename) :
	maxHeight(maxHeight),
	file(filename)
{
	//GenerateGroundFromImage(filename);

	if (!texturename.empty())
	{/// if got texture name
		texture = Texture(Image::LoadJPG(texturename.c_str()));
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
		ground.resize(widthAmount * lengthAmount, -aquariumSize.y / 2);
		return;
	}
#endif

	widthAmount = image.width;
	lengthAmount = image.height;
	ground.resize(widthAmount * lengthAmount, -aquariumSize.y / 2);

	for (int y = 0; y < lengthAmount; y++)
	{
 		for (int x = 0; x < widthAmount; x++)
 		{
 	 		ground[x + y*widthAmount] = -aquariumSize.y / 2 + ((unsigned char)(image.data[x * 3 + y * image.rowSpan])) / 255.0 * maxHeight;
 		}
	}
}

double Ground::HeightAt(int x, int y){
	if(x<0)x=0;
	if(x>=widthAmount)x=widthAmount-1;
	if(y<0)y=0;
	if(y>=lengthAmount)y=lengthAmount-1;
	return ground[x+y*widthAmount];
}
Vec3d Ground::PosAt(int x, int y){
	double h=HeightAt(x,y);
	return Vec3d(x*aquariumSize.x/float(widthAmount - 1)-0.5*aquariumSize.x, h, y*aquariumSize.z/float(lengthAmount - 1)-0.5*aquariumSize.z);
}

Vec3d Ground::NormalAt(int x, int y){
	return -Normalized(CrossProd(PosAt(x+1,y)-PosAt(x-1,y),PosAt(x,y+1)-PosAt(x,y-1)));
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

	for(int j=1; j<lengthAmount; ++j)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for(int i=0; i<widthAmount; ++i)
		{
			//Vec2d p=pos(i,j);
			Vec3d p=PosAt(i,j);
			Vec3d n=NormalAt(i,j);

			glTexCoord2i(i,j);
			glNormal3f(n.x,n.y,n.z);
			glVertex3f(p.x,p.y,p.z);
			p=PosAt(i,j-1);
			n=NormalAt(i,j-1);
			glTexCoord2i(i,j-1);
			glNormal3f(n.x,n.y,n.z);
			glVertex3f(p.x,p.y,p.z);
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
