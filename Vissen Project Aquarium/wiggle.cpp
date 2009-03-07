#include "wiggle.hpp"
#include <boost/foreach.hpp>
#include "GL/GLee.h"

#define foreach BOOST_FOREACH

void WiggleTransformation::update(const Eigen::Vector3f& wiggle_freq,
                                  const Eigen::Vector3f& wiggle_dir,
                                  const double wiggle_phase,
                                  const double turn)
{
	// Copy parameters
	_wiggle_freq  = wiggle_freq;
	_wiggle_dir   = wiggle_dir;
	_wiggle_phase = wiggle_phase;
	_turn         = turn;

/*
// approximate elliptic integral with sine... just an approximation, not correct formula

a*sin(l*b) ' = a*b*cos(l*b)

dl=sqrt((a*b*cos(x*b))^2 + 1)*dt

dl min =dx
dl max = sqrt((a*b)^2 + 1)*dx

dx/dl max = 1  (x*b=pi/2)
dx/dl min = 1/sqrt((a*b)^2 + 1)  (l*b=0)
q=(1-1/sqrt((a*b)^2 + 1)))/2
p=(1/sqrt((a*b)^2 + 1)+1)/2
dx/dl=q*cos(2*l*b)+p
x=q*sin(2*b*l)/(2*b) + p*l
*/

	_i_turn = 1. / _turn;
	a = 2.0 * _wiggle_dir.norm() + 1E-20; /// prevent divisions by zero
	b = _wiggle_freq.norm() + 1E-20;
	c = 1.0 / sqrt(a * a * b * b + 1);
	q = 0.5 * (1 - c);
	p = 0.5 * (c + 1);
	s_a = 0.5 * q / b;

}

void WiggleTransformation::operator()(const std::vector<Eigen::Vector3f>& vertices,
                                      const std::vector<Eigen::Vector2f>& /* texcoords */,
                                      const std::vector<Eigen::Vector3f>& /* normals */,
                                      const std::vector<unsigned int>& /* indices */) const
{
	wiggledVertices.clear();
	wiggledVertices.reserve(vertices.size());

	foreach (const Eigen::Vector3f& vertex, vertices)
	{
		// Wiggle position
		const float alpha = vertex.dot(_wiggle_freq);
		Eigen::Vector3f wiggled_pos = vertex + _wiggle_dir * float(sin(alpha + _wiggle_phase));

		// approximate the elliptic integral.
		wiggled_pos.x() += p * sin(2 * alpha + _wiggle_phase) * s_a;

		/// approximate the elliptic integral, weirder but better looking (?).
		//wiggled_pos += wiggle_freq * float(sin(2 * alpha + wiggle_phase) * s_a / b);

		// turns:
		if (fabs(_turn) > 1E-5)
		{
			double turn_a = wiggled_pos.x() * _turn;

			if (_turn > 0)
			{
				wiggled_pos.x() = sin(turn_a) * (_i_turn + wiggled_pos.z());
				wiggled_pos.z() = cos(turn_a) * (_i_turn + wiggled_pos.z()) - _i_turn;
			}
			else
			{
				wiggled_pos.x() = sin(-turn_a) * (-_i_turn - wiggled_pos.z());
				wiggled_pos.z() = -_i_turn - cos(-turn_a) * (-_i_turn - wiggled_pos.z());
			}
		}

		wiggledVertices.push_back(wiggled_pos);
	}

	glVertexPointer(3, GL_FLOAT, 0, wiggledVertices[0].data());
	/// todo: also apply wiggle to normals? (thats much harder.)
}
