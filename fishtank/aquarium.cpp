#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <cmath>
#include "aquarium.hpp"
#include <framework/debug.hpp>
#include "main.hpp"

#define foreach BOOST_FOREACH

using namespace boost;
using namespace boost::lambda;
using namespace std;

//staan in header uitgelegd
Eigen::Vector3d swimArea(200, 90, 200);
int balkSize = 0; //de grote van de lege ruimte die gemaakt word door textures kleiner te maken -ongebruikt ivm misteffect-
int balkSize2 = 1; //de grote van de balken tussen de hoeken
Eigen::Vector2d faceRange(.5, .5);
//de radius die gebruikt wordt voor de circel wanneer de vissen naar het scherm toe zwemmen
static const double circleDistance = 15.0;

float my_random()
{
	return rand() / float(RAND_MAX);
}

Aquarium::Aquarium(const Eigen::Vector3d& size_) :
	_size(size_),
	//voeg grond toe
	ground(datadir / "heightmap.jpg", 30, this, datadir / "ground.jpg"),
	facePosition(.5, .5)
{
}

void Aquarium::addFish(boost::shared_ptr<Vis> fish)
{
	if (fish)
		fishes.push_back(fish);
}

void Aquarium::addObject(boost::shared_ptr<Object> object)
{
	if (object)
		objects.push_back(object);
}

void Aquarium::AddBubbleSpot(const Eigen::Vector3f& position)
{
	bubbleSpots.push_back(position);
}

void Aquarium::update(double dt)
{
	foreach (const weak_ptr<Vis>& fish, fishes)
	{
		const shared_ptr<Vis> ptr(fish.lock());
		if (!ptr)
			continue;
		ptr->update(dt);
	}

	foreach (const weak_ptr<Object>& object, objects)
	{
		const shared_ptr<Object> ptr(object.lock());
		if (!ptr)
			continue;
		ptr->update(dt);
	}

	//voeg op willekeurige momenten bubbels toe
	foreach (const Eigen::Vector3f& bubbleSpot, bubbleSpots)
	{
		if (my_random() < dt * 9.5)
		{
			bubbles.push_back(shared_ptr<Bubble>(new Bubble(*this, bubbleSpot, 1.f + my_random(), (my_random() < dt * 2))));
		}
	}

	// Update all bubbles and remove those who expired passed their "pop"-time
	bubbles.erase(std::remove_if(bubbles.begin(), bubbles.end(),
	    (
	      bind(&Bubble::update, *_1, dt),
	      bind(&Bubble::pop, *_1) <= 0.
	    )
	  ),
	  bubbles.end());

	//kijk of vissen aan het botsen zijn, en onderneem eventueel actie
	AvoidFishBounce();

	objects.erase(std::remove_if(objects.begin(), objects.end(),
	    bind(&weak_ptr<Object>::expired, _1)
	  ),
	  objects.end());
}

void Aquarium::GoToScreen(const Eigen::Vector2d &position)
{
	const Eigen::Vector2d size(this->size().x(), this->size().y());

	//bereken de positie relatief in het aquarium, gebruikmakende van de factoren
	const Eigen::Vector2d pos(size.cwise() * position - size * .5);

	//laat alle vissen nu naar het punt toe zwemmen
	for (unsigned int i = 0; i < fishes.size(); i++)
	{
		const shared_ptr<Vis> ptr(fishes[i].lock());
		if (!ptr)
			continue;

		//geef de vissen een iets andere positie zodat ze niet door elkaar heen willen gaan (in dit geval een circel)
		const double circular_position = 2. * M_PI / fishes.size() * i;
		const Eigen::Vector2d fishGoalPos(pos.cwise() * Eigen::Vector2d(sin(circular_position), cos(circular_position)) * circleDistance);
		//laat de vissen naar de positie zwemmen
		ptr->setGoal(Eigen::Vector3f(fishGoalPos.x(), fishGoalPos.y(), this->size().y()));
		//ptr->pos = Eigen::Vector3d(fishGoalPos.x(), fishGoalPos.y(), this->size().y());
		debug(LOG_NEVER) << "Goto " << fishGoalPos;
	}
}

void Aquarium::AvoidFishBounce()
{
	// FIXME: O(n^2) behaviour
	foreach (const weak_ptr<Vis>& fishp, fishes)
	{
		const shared_ptr<Vis> fish(fishp.lock());
		if (!fish)
			continue;

		foreach (const weak_ptr<Vis>& fishp, fishes)
		{
			const shared_ptr<Vis> collidable(fishp.lock());
			if (!collidable)
				continue;

			// No collision detection with self
			if (&*collidable == &*fish)
				continue;

			//needs goalcheck in this if aswell
			if (fish->collidingWith(*collidable)
			 && fish->IsGoingTowards(collidable->pos))
			{
				//debug(LOG_NEVER) << "Fish-fish collision " << i << ":" << j;
				fish->Avade();
			}
		}

		foreach (const weak_ptr<Object>& object, objects)
		{
			const shared_ptr<Object> collidable(object.lock());
			if (!collidable || !collidable->model)
				continue;

			//needs goalcheck in this if aswell
			Eigen::Vector3f object_center(0.5f * (collidable->model->bb_h + collidable->model->bb_l));
			if (fish->collidingWith(*collidable)
			 && fish->IsGoingTowards(object_center))
			{
				fish->Avade();
			}
		}
	}
}

void Aquarium::draw()
{
	//teken alle muren die niet webcams zijn
	ground.Draw();

	foreach (const weak_ptr<Vis>& fish, fishes)
	{
		const shared_ptr<Vis> ptr(fish.lock());
		if (!ptr)
			continue;
		ptr->draw();
	}
	foreach (const weak_ptr<Object>& object, objects)
	{
		const shared_ptr<Object> ptr(object.lock());
		if (!ptr)
			continue;
		ptr->draw();
	}

	// Draw transparent objects last to get proper alpha blending
	foreach (const shared_ptr<Bubble>& bubble, bubbles)
		bubble->draw();
	if (drawCollisionSpheres)
	{
		foreach (const weak_ptr<Object>& object, objects)
		{
			const shared_ptr<Object> ptr(object.lock());
			if (!ptr)
				continue;
			ptr->drawCollisionSphere();
		}
		foreach (const weak_ptr<Vis>& fish, fishes)
		{
			const shared_ptr<Vis> ptr(fish.lock());
			if (!ptr)
				continue;
			ptr->drawCollisionSphere();
		}
	}
}
