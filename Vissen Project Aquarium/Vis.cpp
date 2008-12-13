#include "AquariumController.h"
#include "Vis.h"
#include <cstdlib>
#include <ctime>

using namespace math3;

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

const double forward_acceleration=5;
const double vertical_acceleration=1;
//const double max_vertical_speed=10;

const double turn_speed=0.7;///radians per second

const double wiggle_factor=2;/// amount of wiggle (controls amplitude vs speed)
const double wiggle_freq=0.15;///larger means more wiggle waves on fish.
const double wiggle_speed_factor=3.0;

Vis::Vis(Model *model, double scale) //defines model
{
	this->model = model;
	this->scale = scale;
	pos = RandomPos();
	velocity=Vec3d(0,0,0);
	speed=0;

	//goalPos = RandomPos();
	//desired_speed=min_speed+my_random()*(max_speed-min_speed);
	newGoal();


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
		/**/
		/*glColor3d(1,1,1);
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
		glEnd();*/
		/**/

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
	desired_speed = min_speed+my_random()*(max_speed-min_speed);
}

void Vis::Update(double dt)
{
	pos += velocity*dt;

	Vec3d delta=goalPos-pos;
	double dist=Length(delta);
	if (dist<desired_speed)/// if we're about to pass goal in less than second, change the goal
	{
		newGoal();
		return;
	}
	/*
	//Vec3d desiredVelocity=delta*(speed/dist);
	if(desiredVelocity.y>velocity.y){/// vertical movement.
		velocity.y+=dt*acceleration;
	}else{
		velocity.y-=dt*acceleration;
	}

	*/
	/// Horizontal movement
	Vec3d fishForward(cos(swimDirAngle),0,sin(swimDirAngle));

	double dfs=dt*forward_acceleration;
	if(speed<desired_speed-dfs){
		speed+=dfs;
	}else
	if(speed>desired_speed+dfs)
	{
		speed-=dfs;
	}else{
		speed=desired_speed;
	}
	velocity.x=fishForward.x*speed;
	velocity.z=fishForward.z*speed;
	/// Vertical movement
	double desired_vertical_speed=delta.y*speed/dist;// /sqrt(delta.x*delta.x+delta.z*delta.z)
	double dvs=vertical_acceleration*dt;
	if(velocity.y<desired_vertical_speed-dvs){
		velocity.y+=dvs;
	}else
	if(velocity.y>desired_vertical_speed+dvs)
	{
		velocity.y-=dvs;
	}else{
		velocity.y=desired_vertical_speed;
	}
	velocity.y=desired_vertical_speed;

	//double forwardFactor=DotProd(fishForward,Normalized(desiredVelocity));

	//velocity+=dt*fishForward*acceleration;// *forwardFactor

/*
	double current_speed=Length(velocity);
	if(current_speed>speed){/// moving too fast - slow down to exactly speed. TODO: do it smootly for less robotic turns.
		velocity*=speed/current_speed;
	}
	*/

	/// turns
	double goalHeading=atan2(delta.z,delta.x);

	double angleDelta=swimDirAngle-goalHeading;
	/// make the closest turn angle (-pi to +pi)
	if(angleDelta>pi) angleDelta-=2*pi;
	if(angleDelta<-pi)angleDelta+=2*pi;

	double turn=dt*turn_speed;
	if(angleDelta>turn){
		swimDirAngle-=turn;
	}else
	if(angleDelta<-turn){
		swimDirAngle+=turn;
	}


	if(swimDirAngle>pi) swimDirAngle-=2*pi;
	if(swimDirAngle<-pi)swimDirAngle+=2*pi;

	wiggle_amplitude=wiggle_factor-wiggle_factor/(speed+1.0);/// as speed changes from zero to infinity, wiggle amplitude changes from 0 to wiggle_factor

	wiggle_phase+=wiggle_freq*dt*speed*wiggle_speed_factor;/// magic constant.

	/// wraparound not to lose precision over time.
	if(wiggle_phase>2*pi)wiggle_phase-=2*pi;


	myWaitTime -= dt;
	if (myWaitTime < 0)
	{
		myWaitTime = ((int)20+20*my_random());
		newGoal();
	}
}
