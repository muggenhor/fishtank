#ifndef __INCLUDED_BUBBLE_H__
#define __INCLUDED_BUBBLE_H__

#include <Eigen/Core>
#include "object.hpp"

class Bubble : public Object
{
	private:
		//een random tussen 0 en 100 die er voor zorgt dat de sinusen random lijken
		const float wiggleStartX, wiggleStartZ;
		//het type beweging dat de bubbel maakt, true is een sinus, false een exponentiele beweging omhoog
		const bool wiggle;
		//snelheid
		Eigen::Vector3f velocity;

	public:
		Bubble(const Eigen::Vector3f& startpos, float radius, bool wiggle);

		//dit houd bij wanneer een bubbel op het oppervlak kapot moet gaan
		double pop;

		virtual void update(double dt);
		virtual void draw() const;
};

#endif // __INCLUDED_BUBBLE_H__
