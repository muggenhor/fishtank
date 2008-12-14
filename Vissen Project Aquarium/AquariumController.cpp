#include "AquariumController.h"


using namespace math3;

float my_random()
{
	return rand() / float(RAND_MAX);
}

AquariumController::AquariumController(void):
ground(2, 2)
{
	
}

AquariumController::~AquariumController(void)
{
}

Vec3d RandomBubblePos()
{
	Vec3d result;
	result.x=(my_random()-0.5)*aquariumSize.x;
	result.y= -aquariumSize.y / 2;
	result.z=(my_random()-0.5)*aquariumSize.z;
	return result;
}

void AquariumController::AddFish(Model *model)
{
	fishes.push_back(Vis(model, 100 + my_random() * 50));
}

void AquariumController::AddBubbleSpot()
{
	bubbleSpots.push_back(RandomBubblePos());
}

void AquariumController::Update(double dt)
{
	for(int i = 0; i < fishes.size(); i++)
	{
		fishes[i].Update(dt);
	}
	for(int i = 0; i < bubbleSpots.size(); i++)
	{
		if (my_random() < dt * 9.5)
		{
			bubbles.push_back(Bubble(bubbleSpots[i], 1 + my_random() * 2, (my_random() < dt * 2)));
		}
	}
	for(int i = 0; i < bubbles.size(); i++)
	{
		bubbles[i].Update(dt);
		if (bubbles[i].pop < 0)
		{
			bubbles[i] = bubbles.back();
			bubbles.pop_back();
		}
	}
}

void AquariumController::Draw()
{
	for(int i = 0; i < fishes.size(); i++)
	{
		fishes[i].Draw();
	}
	for(int i = 0; i < bubbles.size(); i++)
	{
		bubbles[i].Draw();
	}
	ground.Draw();
}