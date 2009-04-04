#include <fstream>
#include "main.hpp"
#include <stdexcept>
#include <string>
#include "wiggle.hpp"

#define foreach BOOST_FOREACH

WiggleTransformation::WiggleTransformation()
{
	const std::string shaderFile = datadir + "/wiggle.glsl";

	std::ifstream source(shaderFile.c_str());
	if (!source.is_open())
		throw std::runtime_error("Couldn't find file " + shaderFile);

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

void WiggleTransformation::update(const Eigen::Vector3f& wiggle_freq,
                                  const Eigen::Vector3f& wiggle_dir,
                                  const float wiggle_phase,
                                  const float turn)
{
	OpenGL::use_scoped_program use_shader(*this);

	uniform(_wiggle_freq, wiggle_freq);
	uniform(_wiggle_dir, wiggle_dir);
	uniform(_wiggle_phase, wiggle_phase);
	uniform(_turn, turn);
}