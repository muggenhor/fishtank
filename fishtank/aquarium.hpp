#ifndef __INCLUDED_AQUARIUM_HPP__
#define __INCLUDED_AQUARIUM_HPP__

#include <boost/shared_ptr.hpp>
#include <Eigen/Core>
#include <vector>
#include "Vis.h"
#include "Bubble.h"
#include "Ground.h"

//de grootte van de ruimte waar de vissen mogen zwemmen
extern Eigen::Vector3d swimArea;
//de grootte van de zwarten balken (word geladen in main)
extern int balkSize;
extern int balkSize2;

/**
 * Represents the relative portion of the display in which face recognition is
 * allowed to have an effect on the display's perspective.
 *
 * NOTE: The contained scalars represent a fraction for each dimension.
 */
extern Eigen::Vector2d faceRange;

class Aquarium : boost::noncopyable
{
	private:
		Eigen::Vector3d _size;
		std::vector< boost::weak_ptr<Object> > objects;
	std::vector< boost::shared_ptr<Vis> > fishes;
	std::vector< boost::shared_ptr<Bubble> > bubbles;
	std::vector<Eigen::Vector3f> bubbleSpots;
	public:
		const Eigen::Vector3d& size() const { return _size; }
		void size(const Eigen::Vector3d& v) { _size = v; }

	Ground ground;
	//de positie van een persoon die voor het scherm staat
	Eigen::Vector2d facePosition;
		float eye_distance;

		Aquarium(const Eigen::Vector3d& size_ = Eigen::Vector3d::Zero());

	//update alle posities van vissen en laat ze nadenken (laat ze even een stapje leven)
	void update(double dt);
	//teken alle vissen, muren, objecten en bubbels
	void draw();

	//deze functie kijkt voor elke vis of ze botsen, zo ja, dan nemen ze een actie in om te ontwijken
	void AvoidFishBounce();
	//deze functie zorgt ervoor dat de vissen naar het scherm toe zwemmen
	void GoToScreen(const Eigen::Vector2d &position);

	//voeg een vis toe in het aquarium
	void AddFish(boost::shared_ptr<const Model> model, const std::string& propertiesFile);
	//voeg een object toe in het aquarium
	void addObject(boost::shared_ptr<Object> object);
	//voeg een bubbel maker toe in het aquarium
	void AddBubbleSpot(const Eigen::Vector3f& position);
};

//een random tussen 0 en 1
float my_random();

#endif // __INCLUDED_AQUARIUM_HPP__
