#pragma once

#include <vector>
#include "Vis.h"
#include "Bubble.h"
#include "Ground.h"
#include "math3.h"

static const math3::Vec3d aquariumSize(220, 110, 220), swimArea(200, 90, 200);

class AquariumController
{
private:
	std::vector<Vis> fishes;
	std::vector<Bubble> bubbles;
	std::vector<math3::Vec3d> bubbleSpots;

	Ground ground;
public:
	AquariumController(void);
	~AquariumController(void);

	void Update(double dt);
	void Draw();

	void AddFish(Model *model);
	void AddBubbleSpot();
};


float my_random();