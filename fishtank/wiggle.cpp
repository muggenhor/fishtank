#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/path.hpp>
#include "main.hpp"
#include <stdexcept>
#include <string>
#include "wiggle.hpp"

namespace fs = boost::filesystem;

#define foreach BOOST_FOREACH

WiggleTransformation::WiggleTransformation()
{
	const fs::path shaderFile(datadir / "wiggle.glsl");

	fs::ifstream source(shaderFile);
	if (!source.is_open())
		throw std::runtime_error("Couldn't find file: " + shaderFile.file_string());

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
