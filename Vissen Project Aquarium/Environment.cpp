#include "Environment.h"
#include "AquariumController.h"
#include "JPEG.h"

using namespace math3;
using namespace std;

Environment::Environment(const math3::Vec3d &pos1, const math3::Vec3d &pos2, const math3::Vec3d &pos3, const math3::Vec3d &pos4, const std::string &texturename):
texture_id(0)
{
	//maak de posities eigen
	this->pos1 = pos1;
	this->pos2 = pos2;
	this->pos3 = pos3;
	this->pos4 = pos4;
	//er is een texture naam
	if(!texturename.empty())
	{
	  tImageJPG *img=LoadJPG(texturename.c_str(),true);
	  if (pos1.z < 0 && pos2.z < 0 && pos3.z < 0 && pos4.z < 0)
		AquariumController::InitialiseComponents(img);
		JPEG_Texture(&texture_id, img, 0);
	}
}

Environment::~Environment(void)
{
}

void Environment::Draw()
{
	if (texture_id)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture_id);
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);

		//kleur wit maken
		glColor3f(1,1,1);

		glBegin(GL_QUADS);

		//teken het gebeuren
		glTexCoord2f(1, 0);
		glVertex3f(pos1.x, pos1.y, pos1.z);

		glTexCoord2f(1, 1);
		glVertex3f(pos2.x, pos2.y, pos2.z);

		glTexCoord2f(0, 1);
		glVertex3f(pos3.x, pos3.y, pos3.z);

		glTexCoord2f(0, 0);
		glVertex3f(pos4.x, pos4.y, pos4.z);

		glEnd();
		glDisable(GL_TEXTURE_2D);
	}
}
