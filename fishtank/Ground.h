#ifndef __INCLUDED_GROUND_H__
#define __INCLUDED_GROUND_H__

#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>
#include "textures.hpp"
#include "vertexarrays/trianglearray.hpp"

#define CAUSTICSNAME "./Data/caustics/caust"
#define NUMCAUSTICS 32  // Number of caustXX.jpg textures (eg. 00-31)
#define SPEEDCAUSTICS 4 // Higher number = slower movement
#define SCALECAUSTICS 24 // Higher number = bigger texture
#define CAUSTICOPACITY 0.25f // Opacity of the caustics (eg. 0.25f for 25%)

class Ground
{
	public:
		Ground(const char* filename, int maxHeight, const char* texturename = 0);/// use no texture name if want no texture
		~Ground();

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
		Texture* getCausticTexture();

		boost::gil::gray32f_image_t heightmap;
		Texture* texture;
		Texture* caustics[NUMCAUSTICS];
		TriangleArray<unsigned int, float, int, float> triangles;
};

#endif // __INCLUDED_GROUND_H__
