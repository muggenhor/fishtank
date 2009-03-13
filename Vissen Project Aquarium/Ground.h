#ifndef __INCLUDED_GROUND_H__
#define __INCLUDED_GROUND_H__

#include "JPEG.h"
#include "vertexarrays/trianglearray.hpp"

class Ground
{
	public:
		Ground(const char* filename, int maxHeight, const char* texturename = 0);/// use no texture name if want no texture

		//let op: y telt voor lengte hier, niet hoogte, zoals in het aquarium
		//verkrijg de hoogte op een bepaalde positie
		int HeightAt(unsigned int x, unsigned int y) const;

		//teken de vloer
		void Draw();

		inline unsigned int width() const { return heightmap.width; }
		inline unsigned int depth() const { return heightmap.height; }

		int maxHeight;

		void updateRenderData();

	private:
		Image heightmap;
		Texture texture;
		TriangleArray<unsigned int, float, int, float> triangles;
};

#endif // __INCLUDED_GROUND_H__
