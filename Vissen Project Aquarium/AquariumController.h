#pragma once

#include <vector>
#include "Vis.h"
#include "Object.h"
#include "Bubble.h"
#include "Ground.h"
#include "Environment.h"
#include "math3.h"
#include "JPEG.h"

//de grootte van het aquarium
extern math3::Vec3d aquariumSize;
//de grootte van de ruimte waar de vissen mogen zwemmen
extern math3::Vec3d swimArea;
//de grootte van de zwarten balken (word geladen in main)
extern int balkSize;
extern int balkSize2;
//de range in extra % de gezichten mogen herkend worden
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
	//de linker en achtermuur en het plafond
	Environment wall1, wall2, ceiling;
	//de positie van een persoon die voor het scherm staat
	math3::Vec2d facePosition;

	AquariumController(void);
	~AquariumController(void);

	//update alle posities van vissen en laat ze nadenken (laat ze even een stapje leven)
	void Update(double dt);
	//teken alle vissen, muren, objecten en bubbels
	void Draw();

	//deze functie kijkt voor elke vis of ze botsen, zo ja, dan nemen ze een actie in om te ontwijken
	void AvoidFishBounce();
	//deze functie zorgt ervoor dat de vissen naar het scherm toe zwemmen
	void GoToScreen(const math3::Vec2d &position);

	//voeg een vis toe in het aquarium
	void AddFish(Model *model, const std::string &propertiesFile);
	//voeg een object toe in het aquarium
	void AddObject(Model *model, const std::string &propertiesFile, const math3::Vec3d &position);
	//voeg een bubbel maker toe in het aquarium
	void AddBubbleSpot(const math3::Vec3d &position);

	// -afblijven- dit laad alle vitale componenten van het programma, wat samenwerkt met de modelloader en de sockets -afblijven-
	static void InitialiseComponents(tImageJPG *img);
};

//een random tussen 0 en 1
float my_random();
