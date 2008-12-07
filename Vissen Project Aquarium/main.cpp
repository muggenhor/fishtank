#define GLFW_BUILD_DLL
#include <GL/glfw.h>
#include <GL/gl.h>
#include "Vis.h"
#include "MS3D_ASCII.h"

#include <cmath>
#include <cstdlib>
#include <ctime>

#include <iostream>

using namespace std;

const double PI=3.14159265358979323846;

int win_width=640,win_height=480;
const float near_clip_plane=10;
const float far_clip_plane=1000;
const float horizontal_FOV=60*PI/180;/// horizontal field of view in radians


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

	Model model;
	model.loadFromMs3dAsciiFile("data/model/character.txt");

	Vis testVis(&model);
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
		testVis.Update(curTime - oldTime);
		oldTime = curTime;


		glTranslatef(0,0,-300);

		testVis.Draw();//Vis::visModel::test);

		//cout << glfwGetTime() << endl;
/*
		glTranslatef(50*sin(glfwGetTime()*PI),0,50*cos(glfwGetTime()*PI));
		glTranslatef(0,6*sin(glfwGetTime()*PI * 3),0);

		glRotatef(glfwGetTime()*180,0,1,0);

		model.render();
		*/

		glfwSwapBuffers();/// display what we rendered
	}


	return 0;
}
