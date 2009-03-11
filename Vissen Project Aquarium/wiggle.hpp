#ifndef __INCLUDED_WIGGLE_HPP__
#define __INCLUDED_WIGGLE_HPP__

#include <Eigen/Core>
#include "shaders.hpp"

class WiggleTransformation : public OpenGL::Program
{
	public:
		WiggleTransformation();

		/**
		 * @param wiggle_freq internally, "it" (whatever that may be)
		 *                    will be subtly incorrect when wiggle_freq
		 *                    isn't the X axis
		 * @param turn        is inverse radius
		 */
		void update(const Eigen::Vector3f& wiggle_freq = Eigen::Vector3f::UnitX(),
		            const Eigen::Vector3f& wiggle_dir = Eigen::Vector3f::Zero(),
		            const float wiggle_phase = 0,
		            const float turn = 0);

	private:
		OpenGL::VertexShader _shader;
		GLint _wiggle_freq, _wiggle_dir, _wiggle_phase, _turn;
};

#endif // __INCLUDED_WIGGLE_HPP__
