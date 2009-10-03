#ifndef __INCLUDED_BUBBLE_H__
#define __INCLUDED_BUBBLE_H__

#include <Eigen/Core>
#include "MS3D_ASCII.h"

class Bubble
{
	private:
		//de grootte van de bubbel
		float radius;
		//een random tussen 0 en 100 die er voor zorgt dat de sinusen random lijken
		double wiggleStartX, wiggleStartZ;
		//het type beweging dat de bubbel maakt, true is een sinus, false een exponentiele beweging omhoog
		bool wiggle;
		//snelheid
		Eigen::Vector3f velocity;

	public:
		Bubble(const Eigen::Vector3f& startpos, float radius, bool wiggle);

		//de huidige positie van de bubbel
		Eigen::Vector3f pos;
		//dit houd bij wanneer een bubbel op het oppervlak kapot moet gaan
		double pop;

		//deze functie berekend een stap van de bubbel
		void Update(double dt);
		//deze funtie tekend de bubbel
		void Draw() const;

	private:
		boost::shared_ptr<const Model> model;
};

#endif // __INCLUDED_BUBBLE_H__
