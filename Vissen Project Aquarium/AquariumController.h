#pragma once

#include <vector>
#include "Vis.h"
#include "Bubble.h"
#include "math3.h"

static const math3::Vec3d aquariumSize(200,100,200);

class AquariumController
{
private:
	std::vector<Vis> fishes;
	std::vector<Bubble> bubbles;
	std::vector<math3::Vec3d> bubbleSpots;
public:
	AquariumController(void);
	~AquariumController(void);

	void Update(double dt);
	void Draw();

	void AddFish(Model *model);
	void AddBubbleSpot();
};


float my_random();