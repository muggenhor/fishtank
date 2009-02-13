#pragma once

#include <vector>
#include <string>
#include "math3.h"
#include "Main.h"

class Ground
{
	public:
		//de hoogtes in een vector
		std::vector<int> ground;
		//de dimenties
		int widthAmount, lengthAmount, maxHeight;
		//het texture id
		GLuint texture_id;
		std::string file;
	public:
		Ground(const std::string &filename, int maxHeight, const std::string &texturename="");/// use no texture name if want no texture
		~Ground(void);

		//let op: y telt voor lengte hier, niet hoogte, zoals in het aquarium
		//verkrijg de hoogte op een bepaalde positie
		double HeightAt(int x, int y);
		math3::Vec3d PosAt(int x, int y);
		// - afblijven -
		math3::Vec3d NormalAt(int x, int y);

		//teken de vloer
		void Draw();
		//genereer de hoogtes, gebazeerd op een hoogtemap, gegeven als path
		void GenerateGroundFromImage(const std::string &filename);
};
