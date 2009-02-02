#pragma once

#include <vector>
#include "Vis.h"
#include "Object.h"
#include "Bubble.h"
#include "Ground.h"
#include "math3.h"

static const math3::Vec3d aquariumSize(220, 110, 220), swimArea(200, 90, 200);
static const double circleDistance = 15.0;
static const double PI=3.14159265358979323846;

class AquariumController
{
private:
	std::vector<Object> objects;
	std::vector<Vis> fishes;
	std::vector<Bubble> bubbles;
	std::vector<math3::Vec3d> bubbleSpots;
public:
	Ground ground;
	AquariumController(void);
	~AquariumController(void);

	void Update(double dt);
	void Draw();

	void AvoidFishBounce();
	void GoToScreen(const math3::Vec2d &position);

	void AddFish(Model *model, const std::string &propertiesFile);
	void AddObject(Model *model, const std::string &propertiesFile, const math3::Vec3d &position);
	void AddBubbleSpot();
};


float my_random();