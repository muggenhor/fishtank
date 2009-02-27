#include "GL/GLee.h"
#define GLFW_BUILD_DLL
#include <GL/glfw.h>
#include <GL/gl.h>
#include "math3.h"
#include "Vis.h"
#include "AquariumController.h"
#include "MS3D_ASCII.h"
#include <fstream>
#include <string>

#include <cmath>
#include <cstdlib>
#include <ctime>

#include <iostream>
#include <vector>

#include <map>
#include "safe_getline.h"

#include "imagereceiver.h"

using namespace std;
using namespace math3;

//scherm resolutie
static int win_width=0, win_height=0;
//de schermpositie
static int win_move_x = 5, win_move_y = 30;

//oogafstand van het aquarium
static float eye_distance=300;

static map<string, Model> models;

//laad de settings uit het opgegeven bestand
static void LoadSettings(std::istream &input_file)
{
	string s;

	safe_getline(input_file, s);
	win_width = atoi(s.c_str());
	safe_getline(input_file, s);
	win_height = atoi(s.c_str());

	safe_getline(input_file, s);
	win_move_x = atoi(s.c_str());
	safe_getline(input_file, s);
	win_move_y = atoi(s.c_str());

	safe_getline(input_file, s);
	int xx = atoi(s.c_str());
	safe_getline(input_file, s);
	int yy = atoi(s.c_str());
	safe_getline(input_file, s);
	int zz = atoi(s.c_str());
	aquariumSize = math3::Vec3d(xx, yy, zz);

	safe_getline(input_file, s);
	xx = atoi(s.c_str());
	safe_getline(input_file, s);
	yy = atoi(s.c_str());
	safe_getline(input_file, s);
	zz = atoi(s.c_str());
	swimArea = math3::Vec3d(xx, yy, zz);

	safe_getline(input_file, s);
	balkSize2 = atoi(s.c_str());
	if (balkSize2 >= 1)
	{
		balkSize = balkSize2 - 1;
	}
	else
	{
		balkSize = balkSize2;
	}

	safe_getline(input_file, s);
	eye_distance = atof(s.c_str());

	safe_getline(input_file, s);
	range_x = atoi(s.c_str());
	safe_getline(input_file, s);
	range_y = atoi(s.c_str());
}

//laad de modelen uit het opgegeven bestand
static void LoadModels(std::istream &input_file, AquariumController *aquariumController)
{
	string s;

	safe_getline(input_file, s);
	aquariumController->ground.maxHeight = atoi(s.c_str());
	aquariumController->ground.GenerateGroundFromImage(aquariumController->ground.file);

	safe_getline(input_file, s);
	int n=atoi(s.c_str());

	for (int i = 0; i < n; i++)
	{
		string model_name;
		safe_getline(input_file, model_name);
		map<string, Model>::iterator model_iterator=models.find(model_name);
		//model bestaat niet
		if(model_iterator==models.end())
		{
			models[model_name].loadFromMs3dAsciiFile(("./Data/Vissen/Modellen/" +model_name+ ".txt").c_str(), math3::Matrix4x4f(-1,0,0,0, 0,0,1,0, 0,1,0,0, 0,0,0,1));
		}

		string propertieFile;
		safe_getline(input_file, propertieFile);

		safe_getline(input_file, s);
		int m=atoi(s.c_str());
		for (int j = 0; j < m; j++)
		{
			aquariumController->AddFish(&models[model_name], propertieFile);
		}
	}


	safe_getline(input_file, s);
	n=atoi(s.c_str());
	for (int i = 0; i < n; i++)
	{
		string model_name;
		safe_getline(input_file, model_name);
		map<string, Model>::iterator model_iterator=models.find(model_name);
		//model bestaat niet
		if(model_iterator==models.end())
		{
			models[model_name].loadFromMs3dAsciiFile(("./Data/Objecten/Modellen/" +model_name+ ".txt").c_str(), math3::Matrix4x4f(-1,0,0,0, 0,0,1,0, 0,1,0,0, 0,0,0,1));
		}

		string propertieFile;
		safe_getline(input_file, propertieFile);

		safe_getline(input_file, s);
		int x = -(aquariumSize.x / 2) + atoi(s.c_str());
		safe_getline(input_file, s);
		int z = -(aquariumSize.z / 2) + atoi(s.c_str());
		int groundposx = (x + (aquariumSize.x / 2)) / aquariumSize.x * (aquariumController->ground.widthAmount);
		int groundposy = (z + (aquariumSize.z / 2)) / aquariumSize.z * (aquariumController->ground.lengthAmount);
		aquariumController->AddObject(&models[model_name], propertieFile, math3::Vec3d(x, aquariumController->ground.HeightAt(groundposx, groundposy), z));
	}

	safe_getline(input_file, s);
	n=atoi(s.c_str());
	for (int i = 0; i < n; i++)
	{
		safe_getline(input_file, s);
		int x = -(aquariumSize.x / 2) + atoi(s.c_str());
		safe_getline(input_file, s);
		int z = -(aquariumSize.z / 2) + atoi(s.c_str());
		int groundposx = (x + (aquariumSize.x / 2)) / aquariumSize.x * (aquariumController->ground.widthAmount);
		int groundposy = (z + (aquariumSize.z / 2)) / aquariumSize.z * (aquariumController->ground.lengthAmount);
		aquariumController->AddBubbleSpot(math3::Vec3d(x, aquariumController->ground.HeightAt(groundposx, groundposy), z));
	}
}

static ImageReceiver image_receiver(7778);
static ImageReceiver image_receiver2(7779);

static PositionReceiver position_receiver(0, 7780);
static PositionReceiver faceposition_receiver(1, 7781);

//teken de de webcam schermen en balken, afhankelijk van de huidige campositie. cam1 == true: grote scherm. cam1 == false: kleine scherm
static void DrawBackground(bool cam1)
{
	glEnable(GL_TEXTURE_2D);
	if (cam1)
	{
		glBindTexture(GL_TEXTURE_2D, image_receiver.TextureID());
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, image_receiver2.TextureID());
	}
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	//zet kleur naar wit
	glColor3f(1,1,1);

	glBegin(GL_QUADS);

	if (cam1)
	{
		glTexCoord2f(0, 0);
		glVertex3f(0.5*aquariumSize.x, -0.5*aquariumSize.y + balkSize, -0.5*aquariumSize.z + balkSize);

		glTexCoord2f(0, 1);
		glVertex3f(0.5*aquariumSize.x, 0.5*aquariumSize.y - balkSize, -0.5*aquariumSize.z + balkSize);

		glTexCoord2f(1, 1);
		glVertex3f(0.5*aquariumSize.x, 0.5*aquariumSize.y - balkSize, 0.5*aquariumSize.z - balkSize);

		glTexCoord2f(1, 0);
		glVertex3f(0.5*aquariumSize.x, -0.5*aquariumSize.y + balkSize, 0.5*aquariumSize.z - balkSize);
	}
	else
	{
		glTexCoord2f(1, 0);
		glVertex3f(-0.5*aquariumSize.x + balkSize, -0.5*aquariumSize.y + balkSize, 0.5*aquariumSize.z);

		glTexCoord2f(1, 1);
		glVertex3f(-0.5*aquariumSize.x + balkSize, 0.5*aquariumSize.y - balkSize, 0.5*aquariumSize.z);

		glTexCoord2f(0, 1);
		glVertex3f(0.5*aquariumSize.x - balkSize, 0.5*aquariumSize.y - balkSize, 0.5*aquariumSize.z);

		glTexCoord2f(0, 0);
		glVertex3f(0.5*aquariumSize.x - balkSize, -0.5*aquariumSize.y + balkSize, 0.5*aquariumSize.z);
	}

	glEnd();
	glDisable(GL_TEXTURE_2D);

	glColor3f(0,0,0);

	glBegin(GL_QUADS);

	if (balkSize2 != 0)
	{
		if (cam1)
		{
			//onder bij klein scherm
			glVertex3f(0.5 * aquariumSize.x - 1, -0.5 * aquariumSize.y + balkSize2, -0.5 * aquariumSize.z);
			glVertex3f(0.5 * aquariumSize.x - 1, -0.5 * aquariumSize.y, -0.5 * aquariumSize.z);
			glVertex3f(0.5 * aquariumSize.x - 1, -0.5 * aquariumSize.y, 0.5 * aquariumSize.z);
			glVertex3f(0.5 * aquariumSize.x - 1, -0.5 * aquariumSize.y + balkSize2, 0.5 * aquariumSize.z);

			//boven bij klein scherm
			glVertex3f(0.5 * aquariumSize.x - 1, 0.5 * aquariumSize.y - balkSize2, -0.5 * aquariumSize.z);
			glVertex3f(0.5 * aquariumSize.x - 1, 0.5 * aquariumSize.y, -0.5 * aquariumSize.z);
			glVertex3f(0.5 * aquariumSize.x - 1, 0.5 * aquariumSize.y, 0.5 * aquariumSize.z);
			glVertex3f(0.5 * aquariumSize.x - 1, 0.5 * aquariumSize.y - balkSize2, 0.5 * aquariumSize.z);

			//rechts bij klein scherm
			glVertex3f(0.5 * aquariumSize.x - 1, -0.5 * aquariumSize.y, -0.5 * aquariumSize.z + balkSize2);
			glVertex3f(0.5 * aquariumSize.x - 1, -0.5 * aquariumSize.y, -0.5 * aquariumSize.z);
			glVertex3f(0.5 * aquariumSize.x - 1, 0.5 * aquariumSize.y, -0.5 * aquariumSize.z);
			glVertex3f(0.5 * aquariumSize.x - 1, 0.5 * aquariumSize.y, -0.5 * aquariumSize.z + balkSize2);

			//links bij klein scherm
			glVertex3f(0.5 * aquariumSize.x - 1, -0.5 * aquariumSize.y, 0.5 * aquariumSize.z - balkSize2);
			glVertex3f(0.5 * aquariumSize.x - 1, -0.5 * aquariumSize.y, 0.5 * aquariumSize.z);
			glVertex3f(0.5 * aquariumSize.x - 1, 0.5 * aquariumSize.y, 0.5 * aquariumSize.z);
			glVertex3f(0.5 * aquariumSize.x - 1, 0.5 * aquariumSize.y, 0.5 * aquariumSize.z - balkSize2);
		}
		else
		{
			//dwarsbalk bij groot scherm
			glVertex3f(-balkSize2, -0.5 * aquariumSize.y + balkSize2, 0.5 * aquariumSize.z - 1);
			glVertex3f(-balkSize2, 0.5 * aquariumSize.y - balkSize2, 0.5 * aquariumSize.z - 1);
			glVertex3f(balkSize2, 0.5 * aquariumSize.y - balkSize2, 0.5 * aquariumSize.z - 1);
			glVertex3f(balkSize2, -0.5 * aquariumSize.y + balkSize2, 0.5 * aquariumSize.z - 1);
			
			//onder bij groot scherm
			glVertex3f(-0.5 * aquariumSize.x, -0.5 * aquariumSize.y + balkSize2, 0.5 * aquariumSize.z - 1);
			glVertex3f(-0.5 * aquariumSize.x, -0.5 * aquariumSize.y, 0.5 * aquariumSize.z - 1);
			glVertex3f(0.5 * aquariumSize.x, -0.5 * aquariumSize.y, 0.5 * aquariumSize.z - 1);
			glVertex3f(0.5 * aquariumSize.x, -0.5 * aquariumSize.y + balkSize2, 0.5 * aquariumSize.z - 1);

			//boven bij groot scherm
			glVertex3f(-0.5 * aquariumSize.x, 0.5 * aquariumSize.y - balkSize2, 0.5 * aquariumSize.z - 1);
			glVertex3f(-0.5 * aquariumSize.x, 0.5 * aquariumSize.y, 0.5 * aquariumSize.z - 1);
			glVertex3f(0.5 * aquariumSize.x, 0.5 * aquariumSize.y, 0.5 * aquariumSize.z - 1);
			glVertex3f(0.5 * aquariumSize.x, 0.5 * aquariumSize.y - balkSize2, 0.5 * aquariumSize.z - 1);

			//rechts bij groot scherm
			glVertex3f(0.5 * aquariumSize.x - balkSize2, -0.5 * aquariumSize.y, 0.5 * aquariumSize.z - 1);
			glVertex3f(0.5 * aquariumSize.x, -0.5 * aquariumSize.y, 0.5 * aquariumSize.z - 1);
			glVertex3f(0.5 * aquariumSize.x, 0.5 * aquariumSize.y, 0.5 * aquariumSize.z - 1);
			glVertex3f(0.5 * aquariumSize.x - balkSize2, 0.5 * aquariumSize.y, 0.5 * aquariumSize.z - 1);

			//links bij groot scherm
			glVertex3f(-0.5 * aquariumSize.x + balkSize2, -0.5 * aquariumSize.y, 0.5 * aquariumSize.z - 1);
			glVertex3f(-0.5 * aquariumSize.x, -0.5 * aquariumSize.y, 0.5 * aquariumSize.z - 1);
			glVertex3f(-0.5 * aquariumSize.x, 0.5 * aquariumSize.y, 0.5 * aquariumSize.z - 1);
			glVertex3f(-0.5 * aquariumSize.x + balkSize2, 0.5 * aquariumSize.y, 0.5 * aquariumSize.z - 1);
		}

		//onder tegenover groot scherm
		glVertex3f(-0.5 * aquariumSize.x, -0.5 * aquariumSize.y + balkSize2, -0.5 * aquariumSize.z + 1);
		glVertex3f(-0.5 * aquariumSize.x, -0.5 * aquariumSize.y, -0.5 * aquariumSize.z + 1);
		glVertex3f(0.5 * aquariumSize.x, -0.5 * aquariumSize.y, -0.5 * aquariumSize.z + 1);
		glVertex3f(0.5 * aquariumSize.x, -0.5 * aquariumSize.y + balkSize2, -0.5 * aquariumSize.z + 1);

		//boven tegenover groot scherm
		glVertex3f(-0.5 * aquariumSize.x, 0.5 * aquariumSize.y - balkSize2, -0.5 * aquariumSize.z + 1);
		glVertex3f(-0.5 * aquariumSize.x, 0.5 * aquariumSize.y, -0.5 * aquariumSize.z + 1);
		glVertex3f(0.5 * aquariumSize.x, 0.5 * aquariumSize.y, -0.5 * aquariumSize.z + 1);
		glVertex3f(0.5 * aquariumSize.x, 0.5 * aquariumSize.y - balkSize2, -0.5 * aquariumSize.z + 1);

		//rechts tegenover groot scherm
		glVertex3f(0.5 * aquariumSize.x - balkSize2, -0.5 * aquariumSize.y, -0.5 * aquariumSize.z + 1);
		glVertex3f(0.5 * aquariumSize.x, -0.5 * aquariumSize.y, -0.5 * aquariumSize.z + 1);
		glVertex3f(0.5 * aquariumSize.x, 0.5 * aquariumSize.y, -0.5 * aquariumSize.z + 1);
		glVertex3f(0.5 * aquariumSize.x - balkSize2, 0.5 * aquariumSize.y, -0.5 * aquariumSize.z + 1);

		//links tegenover groot scherm
		glVertex3f(-0.5 * aquariumSize.x + balkSize2, -0.5 * aquariumSize.y, -0.5 * aquariumSize.z + 1);
		glVertex3f(-0.5 * aquariumSize.x, -0.5 * aquariumSize.y, -0.5 * aquariumSize.z + 1);
		glVertex3f(-0.5 * aquariumSize.x, 0.5 * aquariumSize.y, -0.5 * aquariumSize.z + 1);
		glVertex3f(-0.5 * aquariumSize.x + balkSize2, 0.5 * aquariumSize.y, -0.5 * aquariumSize.z + 1);

		//onder tegenover klein scherm
		glVertex3f(-0.5 * aquariumSize.x + 1, -0.5 * aquariumSize.y + balkSize2, -0.5 * aquariumSize.z);
		glVertex3f(-0.5 * aquariumSize.x + 1, -0.5 * aquariumSize.y, -0.5 * aquariumSize.z);
		glVertex3f(-0.5 * aquariumSize.x + 1, -0.5 * aquariumSize.y, 0.5 * aquariumSize.z);
		glVertex3f(-0.5 * aquariumSize.x + 1, -0.5 * aquariumSize.y + balkSize2, 0.5 * aquariumSize.z);

		//boven tegenover klein scherm
		glVertex3f(-0.5 * aquariumSize.x + 1, 0.5 * aquariumSize.y - balkSize2, -0.5 * aquariumSize.z);
		glVertex3f(-0.5 * aquariumSize.x + 1, 0.5 * aquariumSize.y, -0.5 * aquariumSize.z);
		glVertex3f(-0.5 * aquariumSize.x + 1, 0.5 * aquariumSize.y, 0.5 * aquariumSize.z);
		glVertex3f(-0.5 * aquariumSize.x + 1, 0.5 * aquariumSize.y - balkSize2, 0.5 * aquariumSize.z);

		//rechts tegenover klein scherm
		glVertex3f(-0.5 * aquariumSize.x + 1, -0.5 * aquariumSize.y, -0.5 * aquariumSize.z + balkSize2);
		glVertex3f(-0.5 * aquariumSize.x + 1, -0.5 * aquariumSize.y, -0.5 * aquariumSize.z);
		glVertex3f(-0.5 * aquariumSize.x + 1, 0.5 * aquariumSize.y, -0.5 * aquariumSize.z);
		glVertex3f(-0.5 * aquariumSize.x + 1, 0.5 * aquariumSize.y, -0.5 * aquariumSize.z + balkSize2);

		//links tegenover klein scherm
		glVertex3f(-0.5 * aquariumSize.x + 1, -0.5 * aquariumSize.y, 0.5 * aquariumSize.z - balkSize2);
		glVertex3f(-0.5 * aquariumSize.x + 1, -0.5 * aquariumSize.y, 0.5 * aquariumSize.z + 20);
		glVertex3f(-0.5 * aquariumSize.x + 1, 0.5 * aquariumSize.y, 0.5 * aquariumSize.z + 20);
		glVertex3f(-0.5 * aquariumSize.x + 1, 0.5 * aquariumSize.y, 0.5 * aquariumSize.z - balkSize2);
	}

	glEnd();

	glColor3f(1,1,1);
}

int main()
{
	srand(time(NULL));/// make random numbers sequence depend to program start time.

	cout << "Created by Jasper Lammers and Dmytry Lavrov" << endl;

	glfwInit();

	if( !glfwOpenWindow( win_width, win_height,  0,0,0,0,  16, 	 0, GLFW_WINDOW ))
	{/// width, height, rgba bits (4 params), depth bits, stencil bits, mode.
		cout << "Bye world! Open window failed" << endl;
		return 1;
	}
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glfwSetWindowTitle("");


	ifstream input_file("Settings/aquaConfig.txt");

	LoadSettings(input_file);

	AquariumController aquariumController;

	LoadModels(input_file, &aquariumController);


	glfwSetWindowSize(win_width * 3, win_height);
	glfwSetWindowPos(win_move_x, win_move_y);


	double curTime;
	double oldTime = 0;

	//gebruik mist voor het "water effect"
	GLfloat fogColor[4]= {0.3f, 0.4f, 0.7f, 1.0f};
	//glClearColor(fogColor[0], fogColor[1], fogColor[2], fogColor[3]); // make it clear to fog color?

	glFogi(GL_FOG_MODE, GL_EXP);
	//glFogi(GL_FOG_COORD_SRC, GL_FRAGMENT_DEPTH);
	//mist kleur
	glFogfv(GL_FOG_COLOR, fogColor);
	//mist dichtheid
	glFogf(GL_FOG_DENSITY, 1.0f/(eye_distance+aquariumSize.z+aquariumSize.x));
	/// niet nodig
	//glHint(GL_FOG_HINT, GL_NICEST);
	//glFogf(GL_FOG_START, eye_distance);
	//glFogf(GL_FOG_END, eye_distance+aquariumSize.z+aquariumSize.x);
	glEnable(GL_FOG);

	while(glfwGetWindowParam( GLFW_OPENED ))
	{
		//update
		curTime = glfwGetTime();
		double dt = curTime - oldTime;
		if(dt > 0.1)
		{
			dt=0.1;
		}
		aquariumController.Update(dt);

		image_receiver.Update();
		image_receiver2.Update();

		position_receiver.Update(&aquariumController);
		faceposition_receiver.Update(&aquariumController);


		oldTime = curTime;


		glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);

		glfwGetWindowSize(&win_width,&win_height);/// get window size

		double port1_width = win_width *2.0/3.0;
		double port2_width = win_width *1.0/3.0;

		//linker view
		glViewport(0, 0, port1_width, win_height);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		double kx=0.25*aquariumSize.x;
		double ky=((double)win_height/(double)port1_width)*kx;

		math3::Vec2d size = math3::Vec2d(aquariumSize.x * (range_x / 100.0), aquariumSize.y * (range_y / 100.0));
		math3::Vec2d pos = math3::Vec2d(size.x * aquariumController.facePosition.x / 100.0 - size.x / 2.0, size.y * aquariumController.facePosition.y / 100.0 - size.y / 2.0);

		glFrustum( -kx - 0.5*pos.x, kx - 0.5*pos.x, -ky - 0.5*pos.y, ky - 0.5*pos.y, 0.5*eye_distance, eye_distance*2+aquariumSize.z );
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glTranslatef(-pos.x, -pos.y, -(eye_distance+aquariumSize.z*0.5));

		DrawBackground(true);
		aquariumController.Draw();

		//rechter view
		glViewport(port1_width, 0, port2_width, win_height);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		kx=0.25*aquariumSize.z;
		ky=((double)win_height/(double)port2_width)*kx;
		glFrustum( -kx, kx, -ky, ky, 0.5*eye_distance, eye_distance*2+aquariumSize.x );
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();


		glTranslatef(0,0,-(eye_distance+aquariumSize.x*0.5));
		glRotatef(270,0,1,0);
		
		DrawBackground(false);
		aquariumController.Draw();

		//TestDrawAquarium();

		glfwSwapBuffers();
	}

	return 0;
}
