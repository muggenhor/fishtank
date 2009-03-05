#ifndef __INCLUDED_ENVIRONMENT_H__
#define __INCLUDED_ENVIRONMENT_H__

#include <Eigen/Core>
#include <vector>
#include <string>
#include "JPEG.h"

//een plaatje dat getekend word, kunnen we gebruiken als muur
class Environment
{
	public:
		Environment(const Eigen::Vector3d& pos1, const Eigen::Vector3d& pos2, const Eigen::Vector3d& pos3, const Eigen::Vector3d& pos4, const std::string &texturename="");/// use no texture name if want no texture

		//teken de muur
		void Draw();

	private:
		Texture texture;
		//de 4 hoeken van het plaatje
		Eigen::Vector3d pos1, pos2, pos3, pos4;
};

#endif // __INCLUDED_ENVIRONMENT_H__
