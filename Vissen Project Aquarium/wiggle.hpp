#ifndef __INCLUDED_WIGGLE_HPP__
#define __INCLUDED_WIGGLE_HPP__

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <Eigen/Core>
#include <vector>
#include "shaders.hpp"

class WiggleTransformation
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

		boost::function<void ()> operator()(const std::vector<Eigen::Vector3f>& vertices,
		                                    const std::vector<Eigen::Vector2f>& texcoords,
		                                    const std::vector<Eigen::Vector3f>& normals,
		                                    const std::vector<unsigned int>& indices) const;

	private:
		class WiggleShaderProgram : public OpenGL::Program
		{
			public:
				WiggleShaderProgram();

				void wiggle_freq(const Eigen::Vector3f& v);
				void wiggle_dir(const Eigen::Vector3f& v);
				void wiggle_phase(float v);
				void turn(float v);

			private:
				OpenGL::VertexShader _shader;
				GLint _wiggle_freq, _wiggle_dir, _wiggle_phase, _turn;
		};

	private:
		mutable std::vector<Eigen::Vector3f> wiggledVertices;
		Eigen::Vector3f _wiggle_freq;
		Eigen::Vector3f _wiggle_dir;
		float           _wiggle_phase;
		float           _turn;
		float           _i_turn;
		float           a, b, c, p, q, s_a;

		static boost::shared_ptr<WiggleShaderProgram> shader;
};

#endif // __INCLUDED_WIGGLE_HPP__
