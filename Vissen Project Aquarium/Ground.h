#ifndef __INCLUDED_GROUND_H__
#define __INCLUDED_GROUND_H__

#include <Eigen/Core>
#include <vector>
#include <string>
#include "JPEG.h"

class Ground
{
	public:
		Ground(const std::string &filename, int maxHeight, const std::string &texturename="");/// use no texture name if want no texture

		//let op: y telt voor lengte hier, niet hoogte, zoals in het aquarium
		//verkrijg de hoogte op een bepaalde positie
		int HeightAt(int x, int y) const;

		//teken de vloer
		void Draw();
		//genereer de hoogtes, gebazeerd op een hoogtemap, gegeven als path
		void GenerateGroundFromImage(const std::string &filename);

		int widthAmount, lengthAmount, maxHeight;
		std::string file;

	private:
		Eigen::Vector3f PosAt(int x, int y);
		const Eigen::Vector3f& NormalAt(int x, int y) const;

	private:
		//de hoogtes in een vector
		std::vector<int> ground;
		std::vector<Eigen::Vector3f> normals;
		//de dimenties
		//het texture id
		Texture texture;
};

#endif // __INCLUDED_GROUND_H__
