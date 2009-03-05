#pragma once

#include <vector>
#include <string>
#include "math3.h"
#include "JPEG.h"

//een plaatje dat getekend word, kunnen we gebruiken als muur
class Environment
{
	public:
		Environment(const math3::Vec3d &pos1, const math3::Vec3d &pos2, const math3::Vec3d &pos3, const math3::Vec3d &pos4, const std::string &texturename="");/// use no texture name if want no texture

		//teken de muur
		void Draw();

	private:
		Texture texture;
		//de 4 hoeken van het plaatje
		math3::Vec3d pos1, pos2, pos3, pos4;
};
