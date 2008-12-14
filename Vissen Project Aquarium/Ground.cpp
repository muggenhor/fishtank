#include "Ground.h"
#include "AquariumController.h"

using namespace math3;

Ground::Ground(int widthAmount, int lengthAmount):
ground(widthAmount*lengthAmount,-aquariumSize.y / 2)
{
	this->widthAmount = widthAmount;
	this->lengthAmount = lengthAmount;
}

Ground::~Ground(void)
{
}

void Ground::Draw()
{
	glColor3f(1,0.8,0.1);

	for(int i=1; i<widthAmount; ++i)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for(int j=0; j<lengthAmount; ++j)
		{
			//Vec2d p=pos(i,j);
			double y=ground[j+i*lengthAmount];

			glTexCoord2i(i,j);
			glVertex3f(i*aquariumSize.x/float(widthAmount - 1)-0.5*aquariumSize.x, y, j*aquariumSize.z/float(lengthAmount - 1)-0.5*aquariumSize.z);

			y=ground[j+(i-1)*lengthAmount];
			glTexCoord2i(i-1,j);
			glVertex3f((i-1)*aquariumSize.x/float(widthAmount - 1)-0.5*aquariumSize.x, y, j*aquariumSize.z/float(lengthAmount - 1)-0.5*aquariumSize.z);
		}
		glEnd();
	}
}