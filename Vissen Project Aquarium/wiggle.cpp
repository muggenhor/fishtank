#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <fstream>
#include <iostream>
#include "wiggle.hpp"
#include "GL/GLee.h"
#include "glexcept.hpp"

#define foreach BOOST_FOREACH

boost::shared_ptr<WiggleTransformation::WiggleShaderProgram> WiggleTransformation::shader;

WiggleTransformation::WiggleTransformation()
{
	try
	{
		// Create the shader if it hasn't been created yet
		if (!shader)
			shader.reset(new WiggleShaderProgram);
	}
	catch (const OpenGL::missing_capabilities& e)
	{
		std::cerr << "Failed to create WiggleShaderProgram due to missing OpenGL capabilities: " << e.what() << "\n";
	}
	catch (const OpenGL::shader_source_error& e)
	{
		std::cerr << "Failed to compile, link or validate the WiggleShaderProgram: " << e.what() << "\n"
		          << "Info log contains:\n"
		          << e.infoLog() << "\n";
	}
}

void WiggleTransformation::update(const Eigen::Vector3f& wiggle_freq,
                                  const Eigen::Vector3f& wiggle_dir,
                                  const float wiggle_phase,
                                  const float turn)
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

	_i_turn = 1.f / _turn;
	a = 2.f * _wiggle_dir.norm() + 1E-20; /// prevent divisions by zero
	b = _wiggle_freq.norm() + 1E-20;
	c = 1.f / sqrt(a * a * b * b + 1.f);
	q = 0.5f * (1.f - c);
	p = 0.5f * (c + 1.f);
	s_a = 0.5f * q / b;

	if (shader)
	{
		OpenGL::use_scoped_program use_shader(*shader);

		shader->wiggle_freq(_wiggle_freq);
		shader->wiggle_dir(_wiggle_dir);
		shader->wiggle_phase(_wiggle_phase);
		shader->turn(_turn);
	}
}

static void shader_lock(boost::shared_ptr<OpenGL::use_scoped_program> /* shader_lock */)
{
}

boost::function<void ()> WiggleTransformation::operator()(const std::vector<Eigen::Vector3f>& vertices,
                                                          const std::vector<Eigen::Vector2f>& /* texcoords */,
                                                          const std::vector<Eigen::Vector3f>& /* normals */,
                                                          const std::vector<unsigned int>& /* indices */) const
{
	// Use the shader if it's available
	if (shader)
		return boost::bind(&shader_lock, boost::shared_ptr<OpenGL::use_scoped_program>(new OpenGL::use_scoped_program(*shader)));

	wiggledVertices.clear();
	wiggledVertices.reserve(vertices.size());

	foreach (const Eigen::Vector3f& vertex, vertices)
	{
		// Wiggle position
		const float alpha = vertex.dot(_wiggle_freq);
		Eigen::Vector3f wiggled_pos = vertex + _wiggle_dir * float(sin(alpha + _wiggle_phase));

		// approximate the elliptic integral.
		wiggled_pos.x() += p * sin(2.f * alpha + _wiggle_phase) * s_a;

		/// approximate the elliptic integral, weirder but better looking (?).
		//wiggled_pos += wiggle_freq * float(sin(2 * alpha + wiggle_phase) * s_a / b);

		// turns:
		if (fabs(_turn) > 1E-5)
		{
			float turn_a = wiggled_pos.x() * _turn;

			if (_turn > 0.f)
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

	return boost::function<void ()>();
}

WiggleTransformation::WiggleShaderProgram::WiggleShaderProgram()
{
	static const char shaderFile[] = "./Data/wiggle.glsl";

	std::ifstream source(shaderFile);
	if (!source.is_open())
		throw std::runtime_error("Couldn't find file " + std::string(shaderFile));

	_shader.loadSource(source);
	_shader.compile();

	attachShader(_shader);
	link();
	validate();
	_wiggle_freq = getUniformLocation("wiggle_freq");
	_wiggle_dir = getUniformLocation("wiggle_dir");
	_wiggle_phase = getUniformLocation("wiggle_phase");
	_turn = getUniformLocation("turn");
}

void WiggleTransformation::WiggleShaderProgram::wiggle_freq(const Eigen::Vector3f& v)
{
	uniform(_wiggle_freq, v);
}

void WiggleTransformation::WiggleShaderProgram::wiggle_dir(const Eigen::Vector3f& v)
{
	uniform(_wiggle_dir, v);
}

void WiggleTransformation::WiggleShaderProgram::wiggle_phase(float v)
{
	uniform(_wiggle_phase, v);
}

void WiggleTransformation::WiggleShaderProgram::turn(float v)
{
	uniform(_turn, v);
}
