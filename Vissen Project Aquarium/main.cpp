#define GLFW_BUILD_DLL
#include <GL/glfw.h>
#include <GL/gl.h>
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

const double PI=3.14159265358979323846;

int win_width=640,win_height=480;
const float near_clip_plane=10;
const float far_clip_plane=1000;
const float horizontal_FOV=60*PI/180;/// horizontal field of view in radians

map<string, Model> models;

void LoadModels(AquariumController *aquariumController)
{
	string s;
	ifstream input_file("./Settings/aquaConfig.txt");

	safe_getline(input_file, s);/// not using >> because it is problematic if used together with readline
	aquariumController->ground.maxHeight = atoi(s.c_str());

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

ImageReceiver image_receiver(7779);

void DrawBackground(){
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, image_receiver.TextureID());
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	glColor3f(1,1,1);/// just in case

	glBegin(GL_QUADS);

	glTexCoord2f(0, 0);
	glVertex3f(-0.5*aquariumSize.x, -0.5*aquariumSize.y, -0.5*aquariumSize.z);

	glTexCoord2f(1, 0);
	glVertex3f(-0.5*aquariumSize.x, 0.5*aquariumSize.y, -0.5*aquariumSize.z);

	glTexCoord2f(1, 1);
	glVertex3f(-0.5*aquariumSize.x, 0.5*aquariumSize.y, 0.5*aquariumSize.z);

	glTexCoord2f(0, 1);
	glVertex3f(-0.5*aquariumSize.x, -0.5*aquariumSize.y, 0.5*aquariumSize.z);

	glEnd();
	glDisable(GL_TEXTURE_2D);
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
	glfwSetWindowTitle("OpenGL rox");



	AquariumController aquariumController;

	LoadModels(&aquariumController);
	//for(int i = 0; i < 5; i++)
	//{
		//aquariumController.AddFish(&model);
	//}
	aquariumController.AddBubbleSpot();
	//Vis testVis(&model,100);
	//Vis testVis2(&model,100);


	double curTime;
	double oldTime = 0;

	while(glfwGetWindowParam( GLFW_OPENED ))
	{
		glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);

		glfwGetWindowSize(&win_width,&win_height);/// get window size
		glViewport(0, 0, win_width, win_height);/// Set render area


		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//glOrtho(0,win_width,win_height,0, -1000,1000);/// set up orthographic view for 2d drawing.

		/// Set up 3d view.

		double kx=tan(0.5*horizontal_FOV)*near_clip_plane;
		double ky=((double)win_height/(double)win_width)*kx;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum( -kx, kx, -ky, ky, near_clip_plane, far_clip_plane );
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();


		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();


		curTime = glfwGetTime();

		double dt = curTime - oldTime;
		if(dt > 0.1)
		{
			dt=0.1;
		}
		aquariumController.Update(dt);

		image_receiver.Update();

		oldTime = curTime;


		glTranslatef(0,0,-300);

		aquariumController.Draw();

		//testVis.Draw();//Vis::visModel::test);
		//testVis2.Draw();//Vis::visModel::test);

		TestDrawAquarium();

		DrawBackground();

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
