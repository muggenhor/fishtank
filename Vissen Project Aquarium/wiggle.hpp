#ifndef __INCLUDED_WIGGLE_HPP__
#define __INCLUDED_WIGGLE_HPP__

#include <boost/function.hpp>
#include <Eigen/Core>
#include <vector>

class WiggleTransformation
{
	public:
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
		mutable std::vector<Eigen::Vector3f> wiggledVertices;
		Eigen::Vector3f _wiggle_freq;
		Eigen::Vector3f _wiggle_dir;
		float           _wiggle_phase;
		float           _turn;
		float           _i_turn;
		float           a, b, c, p, q, s_a;
};

#endif // __INCLUDED_WIGGLE_HPP__
