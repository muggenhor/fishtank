#include <boost/foreach.hpp>
#include <cmath>
#include "AquariumController.h"
#include <framework/debug.hpp>
#include "main.hpp"

#define foreach BOOST_FOREACH

using namespace std;

//staan in header uitgelegd
Eigen::Vector3d aquariumSize(440, 250, 220), swimArea(200, 90, 200);
int balkSize = 0; //de grote van de lege ruimte die gemaakt word door textures kleiner te maken -ongebruikt ivm misteffect-
int balkSize2 = 5; //de grote van de balken tussen de hoeken
Eigen::Vector2d faceRange(.5, .5);
//de radius die gebruikt wordt voor de circel wanneer de vissen naar het scherm toe zwemmen
static const double circleDistance = 15.0;

float my_random()
{
	return rand() / float(RAND_MAX);
}

AquariumController::AquariumController(void):
		//voeg grond toe
		ground((datadir + "/heightmap.jpg").c_str(), 30, (datadir + "/ground.jpg").c_str()),
		//voeg de muren toe
		wall1(Eigen::Vector3d(-0.5*aquariumSize.x(), -0.5*aquariumSize.y() + balkSize, -0.5*aquariumSize.z() + balkSize),
					Eigen::Vector3d(-0.5*aquariumSize.x(), 0.5*aquariumSize.y() - balkSize, -0.5*aquariumSize.z() + balkSize),
					Eigen::Vector3d(-0.5*aquariumSize.x(), 0.5*aquariumSize.y() - balkSize, 0.5*aquariumSize.z() - balkSize),
					Eigen::Vector3d(-0.5*aquariumSize.x(), -0.5*aquariumSize.y() + balkSize, 0.5*aquariumSize.z() - balkSize), (datadir + "/wall1.jpg").c_str()),
		wall2(Eigen::Vector3d(-0.5*aquariumSize.x() + balkSize, -0.5*aquariumSize.y() + balkSize, -0.5*aquariumSize.z()),
					Eigen::Vector3d(-0.5*aquariumSize.x() + balkSize, 0.5*aquariumSize.y() - balkSize, -0.5*aquariumSize.z()),
					Eigen::Vector3d(0.5*aquariumSize.x() - balkSize, 0.5*aquariumSize.y() - balkSize, -0.5*aquariumSize.z()),
					Eigen::Vector3d(0.5*aquariumSize.x() - balkSize, -0.5*aquariumSize.y() + balkSize, -0.5*aquariumSize.z()), (datadir + "/wall2.jpg").c_str()),
		/*ceiling(Eigen::Vector3d(-0.5*aquariumSize.x() + balkSize, 0.5*aquariumSize.y(), 0.5*aquariumSize.z() - balkSize),
					Eigen::Vector3d(-0.5*aquariumSize.x() + balkSize, 0.5*aquariumSize.y(), -0.5*aquariumSize.z() + balkSize),
					Eigen::Vector3d(0.5*aquariumSize.x() - balkSize, 0.5*aquariumSize.y(), -0.5*aquariumSize.z() + balkSize),
					Eigen::Vector3d(0.5*aquariumSize.x() - balkSize, 0.5*aquariumSize.y(), 0.5*aquariumSize.z() - balkSize), (datadir + "/ceiling.jpg").c_str())*/
		//voeg het plafond toe
		ceiling(Eigen::Vector3d(-0.5*aquariumSize.x(), 0.5*aquariumSize.y(), 0.5*aquariumSize.z()),
					Eigen::Vector3d(-0.5*aquariumSize.x(), 0.5*aquariumSize.y(), -0.5*aquariumSize.z()),
					Eigen::Vector3d(0.5*aquariumSize.x(), 0.5*aquariumSize.y(), -0.5*aquariumSize.z()),
					Eigen::Vector3d(0.5*aquariumSize.x(), 0.5*aquariumSize.y(), 0.5*aquariumSize.z()), (datadir + "/ceiling.jpg").c_str()),
		facePosition(.5, .5)
{
}

void AquariumController::AddFish(boost::shared_ptr<Model> model, const string &propertiesFile)
{
	fishes.push_back(Vis(model, propertiesFile, ground.maxHeight));
}

void AquariumController::AddObject(boost::shared_ptr<Model> model, const string &propertiesFile, const Eigen::Vector3d &position)
{
	objects.push_back(Object(model, propertiesFile, position));
}

void AquariumController::AddBubbleSpot(const Eigen::Vector3f& position)
{
	bubbleSpots.push_back(position);
}

void AquariumController::Update(double dt)
{
	foreach (Vis& fish, fishes)
		fish.Update(dt);
	foreach (Object& object, objects)
		object.Update(dt);
	//voeg op willekeurige momenten bubbels toe
	foreach (const Eigen::Vector3f& bubbleSpot, bubbleSpots)
	{
		if (my_random() < dt * 9.5)
		{
			bubbles.push_back(Bubble(bubbleSpot, 1 + my_random() * 2, (my_random() < dt * 2)));
		}
	}
	//update de bubbels en kijk of ze weggegooit mogen worden
	foreach (Bubble& bubble, bubbles)
	{
		bubble.Update(dt);
		if (bubble.pop < 0)
		{
			bubble = bubbles.back();
			bubbles.pop_back();
		}
	}
	//kijk of vissen aan het botsen zijn, en onderneem eventueel actie
	AvoidFishBounce();
}

void AquariumController::GoToScreen(const Eigen::Vector2d &position)
{
	const Eigen::Vector2d size(aquariumSize.x(), aquariumSize.y());

	//bereken de positie relatief in het aquarium, gebruikmakende van de factoren
	const Eigen::Vector2d pos(size.cwise() * position - size * .5);

	//laat alle vissen nu naar het punt toe zwemmen
	for (unsigned int i = 0; i < fishes.size(); i++)
	{
		//geef de vissen een iets andere positie zodat ze niet door elkaar heen willen gaan (in dit geval een circel)
		const double circular_position = 2. * M_PI / fishes.size() * i;
		const Eigen::Vector2d fishGoalPos(pos.cwise() * Eigen::Vector2d(sin(circular_position), cos(circular_position)) * circleDistance);
		//laat de vissen naar de positie zwemmen
		fishes[i].setGoal(Eigen::Vector3f(fishGoalPos.x(), fishGoalPos.y(), aquariumSize.y()));
		//fishes[i].pos = Eigen::Vector3d(fishGoalPos.x(), fishGoalPos.y(), aquariumSize.y());
		debug(LOG_NEVER) << "Goto " << fishGoalPos;
	}
}

void AquariumController::AvoidFishBounce()
{
	// FIXME: O(n^2) behaviour
	foreach (Vis& fish, fishes)
	{
		foreach (const Vis& collidable, fishes)
		{
			// No collision detection with self
			if (&collidable == &fish)
				continue;

			//needs goalcheck in this if aswell
			if (fish.Colliding(collidable.pos, collidable.sphere)
			 && fish.IsGoingTowards(collidable.pos))
			{
				//debug(LOG_NEVER) << "Fish-fish collision " << i << ":" << j;
				fish.Avade();
			}
		}

		foreach (const Object& collidable, objects)
		{
			//needs goalcheck in this if aswell
			Eigen::Vector3f object_center(0.5f * (collidable.model->bb_h + collidable.model->bb_l));
			if (fish.Colliding(object_center, collidable.sphere) && fish.IsGoingTowards(object_center))
			{
				fish.Avade();
			}
		}
	}
}

void AquariumController::Draw()
{
	//teken alle muren die niet webcams zijn
	ground.Draw();
	wall1.Draw();
	wall2.Draw();
	ceiling.Draw();

	foreach (const Vis& fish, fishes)
		fish.Draw();
	foreach (const Object& object, objects)
		object.Draw();
	// Draw bubbles as last to get proper alpha blending
	foreach (const Bubble& bubble, bubbles)
		bubble.Draw();
}
