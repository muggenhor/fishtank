#include "Vis.h"
#include <cstdlib>
#include <ctime>

float my_random()
{
	return rand() / float(RAND_MAX);
}

using namespace math3;

const Vec3d aquariumSize(200,100,200);

void TestDrawAquarium(){

	glColor3d(1,1,1);
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINES);

	glVertex3d(-0.5*aquariumSize.x,-0.5*aquariumSize.y,-0.5*aquariumSize.z);
	glVertex3d(0.5*aquariumSize.x,-0.5*aquariumSize.y,-0.5*aquariumSize.z);

	glVertex3d(-0.5*aquariumSize.x,-0.5*aquariumSize.y,-0.5*aquariumSize.z);
	glVertex3d(-0.5*aquariumSize.x,0.5*aquariumSize.y,-0.5*aquariumSize.z);

	glVertex3d(-0.5*aquariumSize.x,-0.5*aquariumSize.y,-0.5*aquariumSize.z);
	glVertex3d(-0.5*aquariumSize.x,-0.5*aquariumSize.y,0.5*aquariumSize.z);



	glVertex3d(-0.5*aquariumSize.x,-0.5*aquariumSize.y,0.5*aquariumSize.z);
	glVertex3d(0.5*aquariumSize.x,-0.5*aquariumSize.y,0.5*aquariumSize.z);

	glVertex3d(-0.5*aquariumSize.x,-0.5*aquariumSize.y,0.5*aquariumSize.z);
	glVertex3d(-0.5*aquariumSize.x,0.5*aquariumSize.y,0.5*aquariumSize.z);


	glVertex3d(-0.5*aquariumSize.x,0.5*aquariumSize.y,-0.5*aquariumSize.z);
	glVertex3d(0.5*aquariumSize.x,0.5*aquariumSize.y,-0.5*aquariumSize.z);

	glVertex3d(-0.5*aquariumSize.x,0.5*aquariumSize.y,-0.5*aquariumSize.z);
	glVertex3d(-0.5*aquariumSize.x,0.5*aquariumSize.y,0.5*aquariumSize.z);


  glVertex3d(0.5*aquariumSize.x,-0.5*aquariumSize.y,-0.5*aquariumSize.z);
	glVertex3d(0.5*aquariumSize.x,0.5*aquariumSize.y,-0.5*aquariumSize.z);

	glVertex3d(0.5*aquariumSize.x,-0.5*aquariumSize.y,-0.5*aquariumSize.z);
	glVertex3d(0.5*aquariumSize.x,-0.5*aquariumSize.y,0.5*aquariumSize.z);



	glVertex3d(0.5*aquariumSize.x,0.5*aquariumSize.y,0.5*aquariumSize.z);
	glVertex3d(-0.5*aquariumSize.x,0.5*aquariumSize.y,0.5*aquariumSize.z);

	glVertex3d(0.5*aquariumSize.x,0.5*aquariumSize.y,0.5*aquariumSize.z);
	glVertex3d(0.5*aquariumSize.x,-0.5*aquariumSize.y,0.5*aquariumSize.z);

	glVertex3d(0.5*aquariumSize.x,0.5*aquariumSize.y,0.5*aquariumSize.z);
	glVertex3d(0.5*aquariumSize.x,0.5*aquariumSize.y,-0.5*aquariumSize.z);

	glEnd();

}

Vec3d RandomPos()
{
	Vec3d result;
	result.x=(my_random()-0.5)*aquariumSize.x;
	result.y=(my_random()-0.5)*aquariumSize.y;
	result.z=(my_random()-0.5)*aquariumSize.z;
	return result;
}

const double max_speed=20.0;
const double min_speed=15.0;
const double acceleration=40;

const double turn_speed=1.0;///

const double wiggle_factor=2;/// amount of wiggle (controls amplitude vs speed)
const double wiggle_freq=0.15;///larger means more wiggle waves on fish.

Vis::Vis(Model *model, double scale) //defines model
{
	this->model = model;
	this->scale = scale;
	pos = RandomPos();
	goalPos = RandomPos();
	velocity=Vec3d(0,0,0);
	speed=min_speed+my_random()*(max_speed-min_speed);
	swimDirAngle=0;
	wiggle_phase=0;
	wiggle_amplitude=0;
	myWaitTime = 20+20*my_random();
}

Vis::~Vis(void)
{
}

void Vis::Draw()
{
	if(model)
	{
		/// debugging: draw some lines to show where dude's floating
		/// 1 line would be impossible to see 3d perspective on, so draw 8
		/*
		glColor3d(1,1,1);
		glDisable(GL_TEXTURE_2D);
		glBegin(GL_LINES);
		for(int i=0;i<2;++i){
			for(int j=0;j<2;++j){
				for(int k=0;k<2;++k){
					glVertex3f(pos.x+i*4,pos.y+j*4,pos.z+k*4);
					glVertex3f(goalPos.x+i*4,goalPos.y+j*4,goalPos.z+k*4);
				}
			}
		}
		glEnd();
		*/

		glPushMatrix();
		glTranslatef(pos.x,pos.y,pos.z);
		glRotatef(-swimDirAngle*180.0/pi, 0,1,0);

		glScalef(scale,scale,scale);
		glEnable(GL_NORMALIZE);
		model->render(Vec3f(wiggle_freq*scale,0,0), Vec3f(0,0,wiggle_amplitude/scale),wiggle_phase);

		glPopMatrix();
	}
}

void Vis::newGoal()
{
	goalPos = RandomPos();
	speed = min_speed+my_random()*(max_speed-min_speed);
}

void Vis::Update(double dt)
{
	pos += velocity*dt;

	Vec3d delta=goalPos-pos;
	double dist=Length(delta);
	if (dist<speed)/// if we're about to pass goal in less than second, change the goal
	{
		newGoal();
		return;
	}

	Vec3d desiredVelocity=delta*(speed/dist);
	if(desiredVelocity.y>velocity.y){/// vertical movement.
		velocity.y+=dt*acceleration;
	}else{
		velocity.y-=dt*acceleration;
	}
	/// horizontal movement
	Vec3d fishForward(cos(swimDirAngle),0,sin(swimDirAngle));

	double forwardFactor=DotProd(fishForward,Normalized(desiredVelocity));
	velocity+=dt*fishForward*acceleration*forwardFactor;


	double current_speed=Length(velocity);
	if(current_speed>speed){/// moving too fast - slow down to exactly speed. TODO: do it smootly for less robotic turns.
		velocity*=speed/current_speed;
	}
	/// turns
	double goalHeading=atan2(delta.z,delta.x);

	double angleDelta=goalHeading-swimDirAngle;
	/// make the closest turn angle (-pi to +pi)
	if(angleDelta>pi) angleDelta-=2*pi;
	if(angleDelta<-pi)angleDelta+=2*pi;

	swimDirAngle+=dt*angleDelta*turn_speed;

	if(swimDirAngle>pi) swimDirAngle-=2*pi;
	if(swimDirAngle<-pi)swimDirAngle+=2*pi;

	wiggle_amplitude=wiggle_factor-wiggle_factor/(current_speed+1.0);/// as speed changes from zero to infinity, wiggle amplitude changes from 0 to wiggle_factor

	wiggle_phase+=wiggle_freq*dt*current_speed*2.5;/// magic constant.


	myWaitTime -= dt;
	if (myWaitTime < 0)
	{
		myWaitTime = ((int)20+20*my_random());
		newGoal();
	}
}
