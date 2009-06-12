#include <boost/array.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/gil/algorithm.hpp>
#include <Eigen/Core>
#include "Ground.h"
#include <iostream>
#include "AquariumController.h"
#include "math-helpers.hpp"
#include "main.hpp"
#include "image.hpp"

#define foreach BOOST_FOREACH

using namespace boost;
using namespace boost::gil;
using namespace std;

static const char CAUSTICSNAME[] = "./Data/caustics/caust";

static const int NUMCAUSTICS = 32; // Number of caustXX.jpg textures (eg. 00-31)
static const int SPEEDCAUSTICS = 4; // Higher number = slower movement
static const float SCALECAUSTICS = 24; // Higher number = bigger texture
static const float CAUSTICOPACITY = 0.25f; // Opacity of the caustics (eg. 0.25f for 25%)

Ground::Ground(const char* const filename, int maxHeight, const char* const texturename) :
	maxHeight(maxHeight)
{
	// Load heightmap
	rgb8_image_t img;
	read_image(filename, img);

	// Convert heightmap to grayscale (so it's usable)
	heightmap.recreate(img.dimensions());
	copy_and_convert_pixels(const_view(img), view(heightmap));

	if (texturename)
	{
		read_image(texturename, img);
		texture = flipped_up_down_view(const_view(img));
	}

	// Loop until we have loaded in all the desired JPEGs
	for (int i = 0; i < NUMCAUSTICS; ++i)
	{
		const string causticsfilename = (format("%s%02d.jpg") % CAUSTICSNAME % i).str();

		read_image(causticsfilename, img);
		caustics.push_back(const_view(img));
	}

	triangles.UseVBOs(use_vbos);
	updateRenderData();

#ifdef DEBUG
	std::cerr << "Ground has:\n"
	          << triangles.uniqueVertices() << " vertices\n"
	          << triangles.drawnVertices() << " indices\n\n";
#endif
}

Texture& Ground::getCausticTexture()
{
	// We add a counter here so we can slow down or speed up the caustics animation
	static int num = 0;
	static int currentindex = 0;

	if (++num == SPEEDCAUSTICS)
	{
		currentindex = (currentindex + 1) % caustics.size();
		num = 0;
	}

	return caustics[currentindex];
}

int Ground::HeightAt(unsigned int x, unsigned int y) const
{
	static const Eigen::Vector3f NTSC_grayscale_weights(0.299f, 0.587f, 0.114f);

	x = clip(x, 0U, width() - 1);
	y = clip(y, 0U, depth() - 1);

	// Convert RGB value to grayscale by using the NTSC grayscale weights
	const float height_value = const_view(heightmap)(x, y)[0];

	return height_value * maxHeight - aquariumSize.y() * 0.5f;
}

void Ground::Draw()
{
	glDisable(GL_LIGHT1);
	glDisable(GL_LIGHT2);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	static const float lightpos[]={0,200,0,1};

	static const float lightcol[]={1,1,1,1};
	static const float black[]={0,0,0,1};

	glLightfv(GL_LIGHT0,GL_POSITION, lightpos);
	glLightfv(GL_LIGHT0,GL_DIFFUSE, lightcol);
	glLightfv(GL_LIGHT0,GL_AMBIENT, black);
	glLightfv(GL_LIGHT0,GL_SPECULAR, black);

	glLightf(GL_LIGHT0,GL_CONSTANT_ATTENUATION,1.0);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0);

	glEnable ( GL_COLOR_MATERIAL ) ;
	glMaterialfv ( GL_FRONT_AND_BACK, GL_EMISSION, black ) ;
	glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;

	/**
	 * Enable blending and alpha for the ground texture and caustics
	 * Multitexture would've been easier, but since we use TriangleArray::draw,
	 * we can't manipulate glMultiTexCoords
	 */

	glEnable(GL_BLEND);
	glEnable(GL_ALPHA);

	if (texture.empty())
	{
		glColor3f(1,0.8,0.1);
	}
	else
	{
		glEnable(GL_TEXTURE_2D);

		texture.bind();
		glMatrixMode(GL_TEXTURE);
			glPushMatrix();
			glLoadIdentity();
			glScalef(1.f / static_cast<float>(width()), 1.f / static_cast<float>(depth()), 1.f);
		glMatrixMode(GL_MODELVIEW);
		glBlendFunc(GL_ONE,GL_ZERO);
 		glColor4f(1.f,1.f,1.f,1.f);
	}
	triangles.draw();

	// Disable depth testing for the second pass
	if (glIsEnabled(GL_DEPTH_TEST))
		glDisable(GL_DEPTH_TEST);

	// Take alpha into account for blending
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.f,1.f,1.f,CAUSTICOPACITY);

	// Bind the current caustics texture
	getCausticTexture().bind();
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

	glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glScalef(1.f / SCALECAUSTICS, 1.f / SCALECAUSTICS, 1.f);
	glMatrixMode(GL_MODELVIEW);

	triangles.draw();

	glMatrixMode(GL_TEXTURE);
		glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_TEXTURE_2D);

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA);
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_COLOR_MATERIAL) ;

	glEnable(GL_DEPTH_TEST);
	glColor3f(1,1,1);
}

static Eigen::Vector3f PosAt(const Ground& ground, int x, int y)
{
	const float relativeX = x * aquariumSize.x() / float(ground.width() - 1) - 0.5 * aquariumSize.x();
	const float relativeY = y * aquariumSize.z() / float(ground.depth() - 1) - 0.5 * aquariumSize.z();

	return Eigen::Vector3f(relativeX, ground.HeightAt(x, y), relativeY);
}

static void AddTriangleStripPoint(const Ground& ground,
                                  boost::circular_buffer<Eigen::Vector2i>& vertices,
                                  bool& flip_vertices,
                                  TriangleArray<unsigned int, float, int, float>& triangles,
                                  const Eigen::Vector2i& point)
{
	vertices.push_back(point);
	// Keep collecting vertices until we've got enough to draw a triangle
	assert(vertices.capacity() == 3);
	if (vertices.size() < 3)
		return;

	boost::array<unsigned int, 3> indexes;
	if ((flip_vertices = !flip_vertices))
	{
		indexes[0] = 0;
		indexes[1] = 1;
		indexes[2] = 2;
	}
	else
	{
		indexes[0] = 1;
		indexes[1] = 0;
		indexes[2] = 2;
	}

	foreach(unsigned int i, indexes)
	{
		const Eigen::Vector2i& point = vertices[i];

		// Comput the normal for this position
		const Eigen::Vector3f normal(-(PosAt(ground, point.x() + 1, point.y()) - PosAt(ground, point.x() - 1, point.y())).cross(PosAt(ground, point.x(), point.y() + 1) - PosAt(ground, point.x(), point.y() - 1)).normalized());

		triangles.AddPoint(PosAt(ground, point.x(), point.y()), point, normal);
	}
}

void Ground::updateRenderData()
{
	triangles.clear();

	for (int y = 1; y < static_cast<int>(depth()); ++y)
	{
		// Triangles have 3 vertices
		boost::circular_buffer<Eigen::Vector2i> vertices(3);
		bool flip_vertices = false;

		for (int x = 0; x < static_cast<int>(width()); ++x)
		{
			AddTriangleStripPoint(*this, vertices, flip_vertices, triangles, Eigen::Vector2i(x, y));
			AddTriangleStripPoint(*this, vertices, flip_vertices, triangles, Eigen::Vector2i(x, y - 1));
		}
	}
}
