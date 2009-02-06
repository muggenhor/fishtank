#pragma once

#include <vector>
#include "Vis.h"
#include "Object.h"
#include "Bubble.h"
#include "Ground.h"
#include "Environment.h"
#include "math3.h"

extern math3::Vec3d aquariumSize;
extern math3::Vec3d swimArea;
extern int balkSize; //de grote van de zwarte balken tussen de hoeken
extern int range_x;
extern int range_y;

class AquariumController
{
private:
	std::vector<Object> objects;
	std::vector<Vis> fishes;
	std::vector<Bubble> bubbles;
	std::vector<math3::Vec3d> bubbleSpots;
public:
	Ground ground;
	Environment wall1, wall2, ceiling;
	math3::Vec2d facePosition;

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