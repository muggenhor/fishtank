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


int win_width=0, win_height=0;
int win_move_x = 5, win_move_y = 30;

float eye_distance=300;/// eye distance from aquarium, in aquarium units.

map<string, Model> models;

void LoadSettings(std::istream &input_file)
{
	string s;

	safe_getline(input_file, s);/// not using >> because it is problematic if used together with readline
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
	balkSize = atoi(s.c_str());

	safe_getline(input_file, s);
	eye_distance = atof(s.c_str());

	safe_getline(input_file, s);
	range_x = atoi(s.c_str());
	safe_getline(input_file, s);
	range_y = atoi(s.c_str());
}

void LoadModels(std::istream &input_file, AquariumController *aquariumController)
{
	string s;

	safe_getline(input_file, s);
	aquariumController->ground.maxHeight = atoi(s.c_str());
	aquariumController->ground.GenerateGroundFromImage(aquariumController->ground.file);

	safe_getline(input_file, s);
	int n=atoi(s.c_str());

	for (int i = 0; i < n; i++)
	{
		bool exsists = false;

		string model_name;
		safe_getline(input_file, model_name);
		map<string, Model>::iterator model_iterator=models.find(model_name);
		if(model_iterator==models.end())
		{// model does not exist
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
		bool exsists = false;

		string model_name;
		safe_getline(input_file, model_name);
		map<string, Model>::iterator model_iterator=models.find(model_name);
		if(model_iterator==models.end())
		{// model does not exist
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
}

ImageReceiver image_receiver(7778);
ImageReceiver image_receiver2(7779);

PositionReceiver position_receiver(0, 7780);
PositionReceiver faceposition_receiver(1, 7781);

unsigned int background_id;

void DrawBackground(bool cam1){
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

	glColor3f(1,1,1);/// just in case

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

	if (!cam1)
	{
		glColor3f(0,0,0);

		glBegin(GL_QUADS);

		glVertex3f(-balkSize, -0.5 * aquariumSize.y + balkSize, 0.5 * aquariumSize.z - 1);
		glVertex3f(-balkSize, 0.5 * aquariumSize.y - balkSize, 0.5 * aquariumSize.z - 1);
		glVertex3f(balkSize, 0.5 * aquariumSize.y - balkSize, 0.5 * aquariumSize.z - 1);
		glVertex3f(balkSize, -0.5 * aquariumSize.y + balkSize, 0.5 * aquariumSize.z - 1);

		glEnd();

		glColor3f(1,1,1);
	}
}

int main(int argc, char **argv)
{
	srand(time(NULL));/// make random numbers sequence depend to program start time.

  cout << "Hello world!" << endl;

  glfwInit();

	if( !glfwOpenWindow( win_width, win_height,  0,0,0,0,  16, 	 0, GLFW_WINDOW )){/// width, height, rgba bits (4 params), depth bits, stencil bits, mode.
    cout << "Bye world! Open window failed" << endl;
    return 1;
  }
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
	glfwSetWindowTitle("");


	ifstream input_file("./Settings/aquaConfig.txt");

	LoadSettings(input_file);

	AquariumController aquariumController;

	LoadModels(input_file, &aquariumController);


	glfwSetWindowSize(win_width * 3, win_height);
	glfwSetWindowPos(win_move_x, win_move_y);

	//for(int i = 0; i < 5; i++)
	//{
		//aquariumController.AddFish(&model);
	//}
	aquariumController.AddBubbleSpot();
	//Vis testVis(&model,100);
	//Vis testVis2(&model,100);


	double curTime;
	double oldTime = 0;

	GLfloat fogColor[4]= {0.3f, 0.4f, 0.7f, 1.0f};
	//glClearColor(fogColor[0], fogColor[1], fogColor[2], fogColor[3]); // make it clear to fog color?

	glFogi(GL_FOG_MODE, GL_EXP);		// Fog Mode
	//glFogi(GL_FOG_COORD_SRC, GL_FRAGMENT_DEPTH);
	glFogfv(GL_FOG_COLOR, fogColor);			// Set Fog Color
	glFogf(GL_FOG_DENSITY, 1.0f/(eye_distance+aquariumSize.z+aquariumSize.x));				// Fog density, tweak this for less dense fog
	/// unnecessary
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

		/// set up and draw first viewport
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
		aquariumController.Draw();
		DrawBackground(true);

		/// set up and draw second viewport
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
		aquariumController.Draw();
		DrawBackground(false);

		//testVis.Draw();//Vis::visModel::test);
		//testVis2.Draw();//Vis::visModel::test);

		//TestDrawAquarium();



		//cout << glfwGetTime() << endl;
/* */
		//glTranslatef(50*sin(glfwGetTime()*PI),0,50*cos(glfwGetTime()*PI));
		//glTranslatef(0,6*sin(glfwGetTime()*PI * 3),0);

		//glRotatef(glfwGetTime()*180,0,1,0);

		//glScalef(200,200,200);
		//glEnable(GL_NORMALIZE);

		//model.render();
	/*	*/

		glfwSwapBuffers();/// display what we rendered
	}


	return 0;
}