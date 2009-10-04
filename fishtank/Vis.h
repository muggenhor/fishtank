#ifndef __INCLUDED_VIS_H__
#define __INCLUDED_VIS_H__

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <Eigen/Core>
#include "object.hpp"
#include "wiggle.hpp"

class Model;

class Vis : public Object
{
	public:
		//goalPos = positie waar de vis naartoe wil zwemmen (deze is finalgoalpos, tenzij de vis iets moet ontwijken, en met een omweg zwemt)
		//finalgoalpos = de positie waar de vis wil belanden
		Eigen::Vector3f goalPos, finalGoalPos, velocity;
		//true als goalpos gebruikt wordt als tijdelijk doel, om te ontwijken
		bool usingTempGoal;

		//informatie voor het gedrag van de vis
		double swimDirAngle;
		double desired_speed;
		double speed;
		double turn_speed;
		double bending;
		double pitch;

		//instellingen
		double max_speed;
		double min_speed;

		//graden per seconde
		double max_turn_speed;
		double turn_acceleration;

		//groter, meer vibratie
		double wiggle_factor;
		double wiggle_freq;
		//instellingen end

		//de (willekeurige) tijd voor de vis om te besluiten een nieuw punt te zoeken, als de vis zijn punt niet kan bereiken (bijvoorbeeld omdat deze te dichtbij is en er rondjes omheen gaat zwemmen)
		double myWaitTime;

		double wiggle_phase, wiggle_amplitude;

		//de vloerhoogte, om ervoor te zorgen dat de vissen niet lager dan dit gaan zwemmen
		int maxFloorHeight;

		Vis(boost::shared_ptr<const Model> model, const std::string& propertiesFile, int maxFloorHeight); //hihi
		//draag de vis op om een andere positie te pakken (iets te ontwijken)
		void Avade();
		//geeft true als de vis botst met een ander object, hier gegeven in een positie en een botsarea
		virtual bool collidingWith(const Object& object) const;
		//geeft true als de vis richting het gegeven punt aan het zwemmen is
		bool IsGoingTowards(const Eigen::Vector3f& object);

		//geeft een willekeurige posite binnen de zwemarea, rekening houdende met de maxFloorHeight
		Eigen::Vector3f RandomPos() const;
		// - afblijven - de update van de vis, deze houd het bewegen van de vis bij en voert een stap uit - afblijven -
		virtual void update(double dt);
		//teken de vis
		virtual void draw() const;
		virtual void drawCollisionSphere() const;
		//haalt info uit de file, gegeven als path
		void LoadProperties(const std::string &propertiesFile);
		//positiebeheer
		void newGoal();
		void setGoal(const Eigen::Vector3f& final_goal);/// use that to set goal
		void setTemporaryGoal(const Eigen::Vector3f& temp_goal);/// use that to set temporary goal when avoiding collision or something.

	private:
		mutable float collided;
		static boost::shared_ptr<WiggleTransformation> _wiggle;
		static const Eigen::Vector4f uncollided_colour, collision_colour;
};

//teken het aquarium, gebruikmakende van witte lijnen
void TestDrawAquarium();

#endif // __INCLUDED_VIS_H__
