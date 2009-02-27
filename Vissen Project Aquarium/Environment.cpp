#include "Environment.h"
#include "AquariumController.h"
#include "JPEG.h"

using namespace math3;
using namespace std;

Environment::Environment(const math3::Vec3d &pos1, const math3::Vec3d &pos2, const math3::Vec3d &pos3, const math3::Vec3d &pos4, const std::string &texturename) :
	pos1(pos1),
	pos2(pos2),
	pos3(pos3),
	pos4(pos4)
{
	//er is een texture naam
	if (!texturename.empty())
	{
		Image img = Image::LoadJPG(texturename.c_str(), true);

		if (pos1.z < 0 && pos2.z < 0 && pos3.z < 0 && pos4.z < 0)
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
		glVertex3f(pos1.x, pos1.y, pos1.z);

		glTexCoord2f(1.f, 1.f);
		glVertex3f(pos2.x, pos2.y, pos2.z);

		glTexCoord2f(0.f, 1.f);
		glVertex3f(pos3.x, pos3.y, pos3.z);

		glTexCoord2f(0.f, 0.f);
		glVertex3f(pos4.x, pos4.y, pos4.z);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}
