#ifndef __INCLUDED_GROUND_H__
#define __INCLUDED_GROUND_H__

#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>
#include "textures.hpp"
#include <vector>
#include "vertexarrays/trianglearray.hpp"

// Forward declaration to allow references
class Aquarium;

class Ground
{
	public:
		Ground(const char* filename, int maxHeight, const Aquarium* aquarium, const char* texturename = 0);/// use no texture name if want no texture

		//let op: y telt voor lengte hier, niet hoogte, zoals in het aquarium
		//verkrijg de hoogte op een bepaalde positie
		int HeightAt(unsigned int x, unsigned int y) const;

		//teken de vloer
		void Draw();

		inline unsigned int width() const { return heightmap.width(); }
		inline unsigned int depth() const { return heightmap.height(); }
		int maxHeight;

		void updateRenderData();

	private:
		Texture& getCausticTexture();

	private:
		const Aquarium* aquarium;
		boost::gil::gray32f_image_t heightmap;
		Texture texture;
		std::vector<Texture> caustics;
		TriangleArray<unsigned int, float, int, float, 2> triangles;
};

#endif // __INCLUDED_GROUND_H__
