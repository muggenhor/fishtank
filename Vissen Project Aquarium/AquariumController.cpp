#include "AquariumController.h"


using namespace math3;
using namespace std;

float my_random()
{
	return rand() / float(RAND_MAX);
}

AquariumController::AquariumController(void):
ground("./data/heightmap.jpg", 30, "./data/ground.jpg")
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

void AquariumController::AddFish(Model *model, const string &propertiesFile)
{
	fishes.push_back(Vis(model, propertiesFile, ground.maxHeight));
}

void AquariumController::AddObject(Model *model, const string &propertiesFile, const math3::Vec3d &position)
{
	objects.push_back(Object(model, propertiesFile, position));
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
	for(int i = 0; i < objects.size(); i++)
	{
		objects[i].Update(dt);
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
	AvoidFishBounce();
}

void AquariumController::AvoidFishBounce()
{
	for(int i = 0; i < fishes.size(); i++)
	{
		for(int j = 0; j < fishes.size(); j++)
		{
			if (j != i)
			{
				//needs goalcheck in this if aswell
				if (fishes[i].Colliding(fishes[j].pos, fishes[j].sphere) && fishes[i].IsGoingTowards(fishes[j].pos)  )
				{
					//std::cout<<"Fish-fish collision "<<i<<":"<<j<<std::endl;
					fishes[i].Avade();
				}
			}
		}
		for(int j = 0; j < objects.size(); j++)
		{
			//needs goalcheck in this if aswell
			Vec3d object_center=0.5*(objects[j].model->bb_h + objects[j].model->bb_l);
			if(fishes[i].Colliding(object_center, objects[j].sphere) && fishes[i].IsGoingTowards(object_center))
			{
				fishes[i].Avade();
			}
		}
	}
}

void AquariumController::Draw()
{
	for(int i = 0; i < fishes.size(); i++)
	{
		fishes[i].Draw();
	}
	for(int i = 0; i < objects.size(); i++)
	{
		objects[i].Draw();
	}
	for(int i = 0; i < bubbles.size(); i++)
	{
		bubbles[i].Draw();
	}
	ground.Draw();
}
