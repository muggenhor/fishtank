#include "AquariumController.h"
#include "Vis.h"
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>
using namespace math3;
using namespace std;

void TestDrawAquarium()
{
#if 0
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	
	glBegin(GL_QUADS);

	glTextCoord2f(0, 0);
	glVertex3f(-0.5*aquariumSize.x, -0.5*aquariumSize.y, -0.5*aquariumSize.z);

	glTextCoord2f(1, 0);
	glVertex3f(-0.5*aquariumSize.x, 0.5*aquariumSize.y, -0.5*aquariumSize.z);

	glTextCoord2f(1, 1);
	glVertex3f(-0.5*aquariumSize.x, 0.5*aquariumSize.y, 0.5*aquariumSize.z);

	glTextCoord2f(0, 1);
	glVertex3f(-0.5*aquariumSize.x, -0.5*aquariumSize.y, 0.5*aquariumSize.z);

	glEnd();
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
#endif
}

const double forward_acceleration=7.5;
const double vertical_acceleration=5;

const double pitch_factor=0.5;/// how much fish tilts when swimming up and down

const double max_pitch=10*(pi/180.0);///max angle when swimming up
const double min_pitch=-max_pitch;///ditto for swimming down.
const double pitch_change_speed=5*(pi/180.0);/// speed of change of pitch
//const double max_vertical_speed=10;

const double wiggle_speed_factor=2.5;/// larger = fish has to wiggle more to move.
const double bending_factor=0.75;/// larger = fish has to bend more to move. (note: long fish can entirely coil up)


Vis::Vis(Model *model, const std::string &propertiesFile, int maxFloorHeight) //defines model
{
	this->maxFloorHeight = maxFloorHeight;
	usingTempGoal=false;
	this->model = model;

	this->scale = 200;

	pos = RandomPos();
	finalGoalPos=Vec3d(0,0,0);
	velocity=Vec3d(0,0,0);
	speed=0;
	turn_speed=0;
	bending=0;
	pitch=0;

	//goalPos = RandomPos();
	//desired_speed=min_speed+my_random()*(max_speed-min_speed);



	swimDirAngle=0;
	wiggle_phase=0;
	wiggle_amplitude=0;
	myWaitTime = 20+20*my_random();

	//just in case
	max_speed = 20.0;
	min_speed = 15.0;

	max_turn_speed = 2.0;///radians per second
	turn_acceleration = 0.5;///radians per second per second

	wiggle_factor=2;/// amount of wiggle displacement (controls amplitude vs speed), larger, fish bends more for wiggling
	wiggle_freq=0.15;///larger = means more wiggle waves on fish.

	LoadProperties(propertiesFile);

	/// need to set goal after we did load all the properties,
	/// because setting of goal needs some of properties (min and max speed)
	newGoal();

	sphere = 20;
}

Vec3d Vis::RandomPos()
{
	Vec3d result;
	result.x=(my_random()-0.5)*swimArea.x;
	int noswim = maxFloorHeight - model->bb_l.y * scale;
	result.y=(my_random()-0.5)*(swimArea.y - noswim) + (noswim / 2);
	result.z=(my_random()-0.5)*swimArea.z;
	return result;
}

void Vis::LoadProperties(const string &propertiesFile)
{
	string s;
	ifstream input_file(("./Data/Vissen/" + propertiesFile + ".oif").c_str());

	//scaling
	getline(input_file, s);
	double fish_length = atof(s.c_str());
	//randomize
	getline(input_file, s);
	float random_length = atof(s.c_str());
	fish_length = fish_length + my_random() * random_length;

	if(model){
		scale = fish_length / ( model->bb_h.x - model->bb_l.x );
	}else{
		scale = fish_length;
		std::cerr<<"Error: trying to load properties for fish that has no model, cant determine scaling"<<std::endl;
	}

	//speeds
	getline(input_file, s);
	min_speed = atof(s.c_str());

	getline(input_file, s);
	max_speed = atof(s.c_str());

	//sphere
	getline(input_file, s);
	sphere = Length(model->bb_h - model->bb_l) * scale * 0.5 + atoi(s.c_str());

	//wiggle
	getline(input_file, s);
	wiggle_freq = (40.0/fish_length) * atof(s.c_str()) / 100;

	getline(input_file, s);
	wiggle_factor = (fish_length/40.0) * atof(s.c_str()) / 10;

	//turning
	getline(input_file, s);
	max_turn_speed = atof(s.c_str()) / 10;

	getline(input_file, s);
	turn_acceleration = atof(s.c_str()) / 100;
}

bool Vis::Colliding(const math3::Vec3d &object, int otherSphere)
{
	double distance = Length(object - pos);
	distance -= otherSphere;
	distance -= sphere;
	return (distance < 0);
}

bool Vis::IsGoingTowards(const math3::Vec3d &object){
	return DotProd(goalPos-pos, object-pos)>0;
}

void Vis::Draw() const
{
	if(model)
	{
#if 0
		//om te debuggen kun je deze comment uitzetten, het tekent lijnen per vis om te kunnen weten waar ze naartoe willen gaan
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
#endif

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

void Vis::Avade()
{
	setTemporaryGoal(RandomPos());
}

void Vis::newGoal()
{
	//goalPos = RandomPos();
	desired_speed = min_speed+my_random()*(max_speed-min_speed);
	if(usingTempGoal)
	{
		goalPos=finalGoalPos;
		usingTempGoal=false;
	}
	else
	{
		goalPos=RandomPos();
	}
}

void Vis::setGoal(const math3::Vec3d &final_goal){
	if(usingTempGoal)
	{
		finalGoalPos=final_goal;
	}
	else
	{
		goalPos=final_goal;
	}
}
void Vis::setTemporaryGoal(const math3::Vec3d &temp_goal){
	//zonder tempgoal, wordt het een finalgoal
	if(!usingTempGoal)
	{
		finalGoalPos = goalPos;
		usingTempGoal = true;
	}
	goalPos = temp_goal;
}

double TowardsGoalBy(double a, double goal, double by){
	if(a < goal - by)
	{
		return a + by;
	}
	if(a > goal + by)
	{
		return a - by;
	}
	return goal;
}

// - afblijven - de update van de vis, deze houd het bewegen van de vis bij en voert een stap uit - afblijven -
void Vis::Update(double dt)
{
	pos += velocity*dt;

	Vec3d delta = goalPos-pos;
	double dist = Length(delta);
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
	//horisontale beweging
	Vec3d fishForward(cos(swimDirAngle),0,sin(swimDirAngle));

	double dfs=dt*forward_acceleration;
	speed=TowardsGoalBy(speed, desired_speed, dfs);

	velocity.x=fishForward.x*speed;
	velocity.z=fishForward.z*speed;
	//vertikale beweging
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

	//omdraaien
	double goalHeading=atan2(delta.z, delta.x);

	double angleDelta=goalHeading-swimDirAngle;
	//maak de kleinst mogelijke draai (-pi to +pi)
	if(angleDelta>pi)
	{
		angleDelta -= 2 * pi;
	}
	if(angleDelta<-pi)
	{
		angleDelta += 2 * pi;
	}


	if(angleDelta>0.5*turn_speed*turn_speed/turn_acceleration)
	{
		turn_speed=TowardsGoalBy(turn_speed, max_turn_speed, dt*turn_acceleration);
	}
	else if(angleDelta<-0.5*turn_speed*turn_speed/turn_acceleration)
	{
		turn_speed=TowardsGoalBy(turn_speed, -max_turn_speed, dt*turn_acceleration);
	}
	else
	{
		turn_speed=TowardsGoalBy(turn_speed, 0, dt*turn_acceleration);
	}
	swimDirAngle+=dt*turn_speed;


	if(swimDirAngle>pi)
	{
		swimDirAngle-=2*pi;
	}
	if(swimDirAngle<-pi)
	{
		swimDirAngle+=2*pi;
	}

	//vibreren

	//snelheid van 0 tot oneindig = vibratie van 0 tot wiggle_factor
	wiggle_amplitude=wiggle_factor-wiggle_factor/(speed+1.0);

	/// magische constante
	wiggle_phase+=wiggle_freq*dt*speed*wiggle_speed_factor;

	//buigen
	//buig radius is snelheid / buig snelheid
	if(fabs(speed)>1E-2)
	{
		bending=-bending_factor*(turn_speed/speed);
	}

	//pitch
	double desired_pitch=atan2(pitch_factor*velocity.y,sqrt(velocity.x*velocity.x+velocity.z*velocity.z));
	if(desired_pitch < min_pitch)
	{
		desired_pitch = min_pitch;
	}
	if(desired_pitch > max_pitch)
	{
		desired_pitch = max_pitch;
	}
	pitch = TowardsGoalBy(pitch, desired_pitch, pitch_change_speed*dt);


	/// stabielheid dingen:

	//even zorgen dat de wiggle_phase niet te hoog wordt
	if(wiggle_phase>2*pi)
	{
		wiggle_phase-=2*pi;
	}


	myWaitTime -= dt;
	if (myWaitTime < 0)
	{
		myWaitTime = ((int)20+20*my_random());
		newGoal();
	}
}

