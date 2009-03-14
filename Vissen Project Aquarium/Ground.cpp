#include <boost/array.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/foreach.hpp>
#include <Eigen/Core>
#include "Ground.h"
#include <iostream>
#include "AquariumController.h"
#include "JPEG.h"
#include "math-helpers.hpp"
#include "main.hpp"

#define foreach BOOST_FOREACH

Ground::Ground(const char* const filename, int maxHeight, const char* const texturename) :
	maxHeight(maxHeight),
	heightmap(Image::LoadJPG(filename)),
	texture(texturename ? Texture(Image::LoadJPG(texturename, FLIP_Y)) : Texture())
{
	triangles.UseVBOs(use_vbos);
	updateRenderData();

#ifdef DEBUG
	std::cerr << "Ground has:\n"
	          << triangles.uniqueVertices() << " vertices\n"
	          << triangles.drawnVertices() << " indices\n\n";
#endif
}

int Ground::HeightAt(unsigned int x, unsigned int y) const
{
	static const Eigen::Vector3f NTSC_grayscale_weights(0.299f, 0.587f, 0.114f);

	x = clip(x, 0U, width() - 1);
	y = clip(y, 0U, depth() - 1);

	// Convert RGB value to grayscale by using the NTSC grayscale weights
	const float height_value = (heightmap.data[x][y].cast<float>() / static_cast<float>(UCHAR_MAX)).dot(NTSC_grayscale_weights);

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

	if (texture.is_null_texture())
	{
		glColor3f(1,0.8,0.1);
	}
	else
	{
		texture.bind();
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glScalef(1.f / static_cast<float>(width()), 1.f / static_cast<float>(depth()), 1.f);
		glMatrixMode(GL_MODELVIEW);
		glColor3f(1,1,1);
		//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_MODULATE);
	}

	triangles.draw();

	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_COLOR_MATERIAL) ;

	if (!texture.is_null_texture())
	{
		glDisable(GL_TEXTURE_2D);
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
	}

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
