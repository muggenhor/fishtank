#include "Environment.h"
#include "AquariumController.h"

#ifdef WIN32
extern "C"
{
# include "include/jpeglib.h"
}
#else
# include <jpeglib.h>
#endif

/* Needs to be included after jpeglib.h to ensure that the platform workarounds
 * in "include/jpeglib.h" will be used before <jpeglib.h> gets included by
 * jpeg_io.h.
 */
#include <boost/gil/extension/io/jpeg_io.hpp>

using namespace boost::gil;
using namespace std;

Environment::Environment(const Eigen::Vector3d& pos1, const Eigen::Vector3d& pos2, const Eigen::Vector3d& pos3, const Eigen::Vector3d& pos4, const std::string &texturename) :
	texture(0),
	pos1(pos1),
	pos2(pos2),
	pos3(pos3),
	pos4(pos4)
{
	//er is een texture naam
	if (!texturename.empty())
	{
		rgb8_image_t img;
		jpeg_read_image(texturename, img);

		texture = new Texture(flipped_up_down_view(const_view(img)));
	}
}

Environment::~Environment()
{
	delete texture;
}

void Environment::Draw()
{
	if (!texture)
		return;

	texture->bind();

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
