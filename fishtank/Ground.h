#ifndef __INCLUDED_GROUND_H__
#define __INCLUDED_GROUND_H__

#include <boost/filesystem/path.hpp>
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
		Ground(const boost::filesystem::path& filename, int maxHeight, const Aquarium* aquarium, const boost::filesystem::path& texturename = boost::filesystem::path());/// use no texture name if want no texture

		//let op: y telt voor lengte hier, niet hoogte, zoals in het aquarium
		//verkrijg de hoogte op een bepaalde positie
		int HeightAt(unsigned int x, unsigned int y) const;

		//teken de vloer
		void Draw();

		inline unsigned int width() const { return heightmap.width(); }
		inline unsigned int depth() const { return heightmap.height(); }
		inline int  maxHeight() const     { return _maxHeight;   }
		inline void maxHeight(int height) { _maxHeight = height; updateRenderData(); }

	private:
		void updateRenderData();
		Texture& getCausticTexture();

	private:
		const Aquarium* aquarium;
		int _maxHeight;
		boost::gil::gray32f_image_t heightmap;
		Texture texture;
		std::vector<Texture> caustics;
		TriangleArray<unsigned int, float, int, float, 2> triangles;
};

#endif // __INCLUDED_GROUND_H__
