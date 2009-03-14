#include "Environment.h"
#include "AquariumController.h"
#include "JPEG.h"

using namespace std;

Environment::Environment(const Eigen::Vector3d& pos1, const Eigen::Vector3d& pos2, const Eigen::Vector3d& pos3, const Eigen::Vector3d& pos4, const std::string &texturename) :
	pos1(pos1),
	pos2(pos2),
	pos3(pos3),
	pos4(pos4)
{
	//er is een texture naam
	if (!texturename.empty())
	{
		Image img = Image::LoadJPG(texturename.c_str(), FLIP_Y);

		if (pos1.z() < 0
		 && pos2.z() < 0
		 && pos3.z() < 0
		 && pos4.z() < 0)
			AquariumController::InitialiseComponents(img);

		texture = Texture(img);
	}
}

void Environment::Draw()
{
	if (texture.is_null_texture())
		return;

	texture.bind();

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	//kleur wit maken
	glColor3f(1.f, 1.f, 1.f);

	glBegin(GL_QUADS);
		//teken het gebeuren
		glTexCoord2f(1.f, 0.f);
		glVertex3dv(pos1.data());

		glTexCoord2f(1.f, 1.f);
		glVertex3dv(pos2.data());

		glTexCoord2f(0.f, 1.f);
		glVertex3dv(pos3.data());

		glTexCoord2f(0.f, 0.f);
		glVertex3dv(pos4.data());
	glEnd();

	glDisable(GL_TEXTURE_2D);
}
