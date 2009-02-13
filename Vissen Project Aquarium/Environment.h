#pragma once

#include <vector>
#include <string>
#include "math3.h"
#include "Main.h"

//een plaatje dat getekend word, kunnen we gebruiken als muur
class Environment
{
	public:
		//het texture id
		GLuint texture_id;
		//de 4 hoeken van het plaatje
		math3::Vec3d pos1, pos2, pos3, pos4;
	public:
		Environment(const math3::Vec3d &pos1, const math3::Vec3d &pos2, const math3::Vec3d &pos3, const math3::Vec3d &pos4, const std::string &texturename="");/// use no texture name if want no texture
		~Environment(void);

		//teken de muur
		void Draw();
};
