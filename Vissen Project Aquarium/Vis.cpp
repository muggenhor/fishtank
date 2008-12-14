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
	result.x=(my_random()-0.5)*swimArea.x;
	result.y=(my_random()-0.5)*swimArea.y;
	result.z=(my_random()-0.5)*swimArea.z;
	return result;
}

const double max_speed=20.0;
const double min_speed=15.0;

const double forward_acceleration=7.5;
const double vertical_acceleration=5;

const double pitch_factor=0.5;/// how much fish tilts when swimming up and down

const double max_pitch=10*(pi/180.0);///max angle when swimming up
const double min_pitch=-max_pitch;///ditto for swimming down.
const double pitch_change_speed=5*(pi/180.0);/// speed of change of pitch
//const double max_vertical_speed=10;

const double max_turn_speed=2.0;///radians per second
const double turn_acceleration=0.5;///radians per second per second

const double wiggle_factor=2;/// amount of wiggle displacement (controls amplitude vs speed), larger, fish bends more for wiggling
const double wiggle_freq=0.15;///larger = means more wiggle waves on fish.
const double wiggle_speed_factor=2.5;/// larger = fish has to wiggle more to move.
const double bending_factor=0.75;/// larger = fish has to bend more to move. (note: long fish can entirely coil up)


Vis::Vis(Model *model, double scale) //defines model
{
	this->model = model;
	this->scale = scale;
	pos = RandomPos();
	velocity=Vec3d(0,0,0);
	speed=0;
	turn_speed=0;
	bending=0;
	pitch=0;

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
		/**
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
		/**/

		glPushMatrix();
		glTranslatef(pos.x,pos.y,pos.z);
		glRotatef(-swimDirAngle*180.0/pi, 0,1,0);

		glRotatef(pitch*180.0/pi, 0,0,1);

		glScalef(scale,scale,scale);
		glEnable(GL_NORMALIZE);
		model->render(Vec3f(wiggle_freq*scale,0,0), Vec3f(0,0,wiggle_amplitude/scale),wiggle_phase, bending*scale);

		glPopMatrix();
	}
}

void Vis::newGoal()
{
	goalPos = RandomPos();
	desired_speed = min_speed+my_random()*(max_speed-min_speed);
}

double TowardsGoalBy(double a, double goal, double by){
	if(a<goal-by){
		return a+by;
	}
	if(a>goal+by){
		return a-by;
	}
	return goal;
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
	speed=TowardsGoalBy(speed, desired_speed, dfs);

	velocity.x=fishForward.x*speed;
	velocity.z=fishForward.z*speed;
	/// Vertical movement
	double desired_vertical_speed=delta.y*speed/dist;// /sqrt(delta.x*delta.x+delta.z*delta.z)
	double dvs=vertical_acceleration*dt;

	velocity.y=TowardsGoalBy(velocity.y, desired_vertical_speed, dvs);

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

	double angleDelta=goalHeading-swimDirAngle;
	/// make the closest turn angle (-pi to +pi)
	if(angleDelta>pi) angleDelta-=2*pi;
	if(angleDelta<-pi)angleDelta+=2*pi;


	if(angleDelta>0.5*turn_speed*turn_speed/turn_acceleration){/// steering behavior as for homing rocket ;-)
		turn_speed=TowardsGoalBy(turn_speed, max_turn_speed, dt*turn_acceleration);
	}else
	if(angleDelta<-0.5*turn_speed*turn_speed/turn_acceleration){
		turn_speed=TowardsGoalBy(turn_speed, -max_turn_speed, dt*turn_acceleration);
	}else{
		turn_speed=TowardsGoalBy(turn_speed, 0, dt*turn_acceleration);
	}
	swimDirAngle+=dt*turn_speed;


	if(swimDirAngle>pi) swimDirAngle-=2*pi;
	if(swimDirAngle<-pi)swimDirAngle+=2*pi;

	/// wiggle

	wiggle_amplitude=wiggle_factor-wiggle_factor/(speed+1.0);/// as speed changes from zero to infinity, wiggle amplitude changes from 0 to wiggle_factor

	wiggle_phase+=wiggle_freq*dt*speed*wiggle_speed_factor;/// magic constant.

	/// bend
	if(fabs(speed)>1E-2){/// bending radius is speed / turn speed
		bending=-bending_factor*(turn_speed/speed);
	}

	/// pitch

	double desired_pitch=atan2(pitch_factor*velocity.y,sqrt(velocity.x*velocity.x+velocity.z*velocity.z));
	if(desired_pitch<min_pitch)desired_pitch=min_pitch;
	if(desired_pitch>max_pitch)desired_pitch=max_pitch;
	pitch=TowardsGoalBy(pitch, desired_pitch, pitch_change_speed*dt);


	/// stability things:

	/// wraparound not to lose precision over time.
	if(wiggle_phase>2*pi)wiggle_phase-=2*pi;


	myWaitTime -= dt;
	if (myWaitTime < 0)
	{
		myWaitTime = ((int)20+20*my_random());
		newGoal();
	}
}
