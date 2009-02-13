#pragma once

#include "math3.h"
#include "MS3D_ASCII.h"


class Vis
{
	public:
		//pos = huidige positie
		//goalPos = positie waar de vis naartoe wil zwemmen (deze is finalgoalpos, tenzij de vis iets moet ontwijken, en met een omweg zwemt)
		//finalgoalpos = de positie waar de vis wil belanden
		math3::Vec3d pos, goalPos, finalGoalPos, velocity;
		//true als goalpos gebruikt wordt als tijdelijk doel, om te ontwijken
		bool usingTempGoal;

		//het model
		Model *model;

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

		//de lengte van de vis
		double scale;

		//de (willekeurige) tijd voor de vis om te besluiten een nieuw punt te zoeken, als de vis zijn punt niet kan bereiken (bijvoorbeeld omdat deze te dichtbij is en er rondjes omheen gaat zwemmen)
		double myWaitTime;

		double wiggle_phase, wiggle_amplitude;

		//de botsarea
		int sphere;
		//de vloerhoogte, om ervoor te zorgen dat de vissen niet lager dan dit gaan zwemmen
		int maxFloorHeight;

		Vis(Model *model, const std::string &propertiesFile, int maxFloorHeight); //hihi
		~Vis(void);

		//draag de vis op om een andere positie te pakken (iets te ontwijken)
		void Avade();
		//geeft true als de vis botst met een ander object, hier gegeven in een positie en een botsarea
		bool Colliding(const math3::Vec3d &object, int sphere);
		//geeft true als de vis richting het gegeven punt aan het zwemmen is
		bool IsGoingTowards(const math3::Vec3d &object);

		//geeft een willekeurige posite binnen de zwemarea, rekening houdende met de maxFloorHeight
		math3::Vec3d Vis::RandomPos();
		// - afblijven - de update van de vis, deze houd het bewegen van de vis bij en voert een stap uit - afblijven -
		void Update(double dt);
		//teken de vis
		void Draw();
		//haalt info uit de file, gegeven als path
		void LoadProperties(const std::string &propertiesFile);
		//positiebeheer
		void newGoal();
		void setGoal(const math3::Vec3d &final_goal);/// use that to set goal
		void setTemporaryGoal(const math3::Vec3d &temp_goal);/// use that to set temporary goal when avoiding collision or something.
};

//teken het aquarium, gebruikmakende van witte lijnen
void TestDrawAquarium();
