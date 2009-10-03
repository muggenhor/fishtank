#include "AquariumController.h"
#include "Vis.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <framework/debug.hpp>
#include <fstream>
#include "glexcept.hpp"
#include <iostream>
#include "main.hpp"
#include "math-helpers.hpp"
#include <string>

using namespace std;

boost::shared_ptr<WiggleTransformation> Vis::_wiggle;

const Eigen::Vector4f Vis::uncollided_colour(1.f, 1.f, 1.f, .5f);
const Eigen::Vector4f Vis::collision_colour (1.f, 0.f, 0.f, .5f);

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

const float pitch_factor = 0.5;/// how much fish tilts when swimming up and down

const float max_pitch = 10.f * (M_PI / 180.f);///max angle when swimming up
const float min_pitch = -max_pitch;///ditto for swimming down.
const double pitch_change_speed = 5. * (M_PI / 180.0);/// speed of change of pitch
//const double max_vertical_speed=10;

const double wiggle_speed_factor=2.5;/// larger = fish has to wiggle more to move.
const double bending_factor=0.75;/// larger = fish has to bend more to move. (note: long fish can entirely coil up)


Vis::Vis(boost::shared_ptr<Model> model, const std::string& propertiesFile, int maxFloorHeight) :
	usingTempGoal(false),
	model(model),
	scale(200),
	radius(20),
	maxFloorHeight(maxFloorHeight),
	collided(0.f)
{
	try
	{
		// Create the shader if it hasn't been created yet
		if (!_wiggle)
			_wiggle.reset(new WiggleTransformation);
	}
	catch (const OpenGL::missing_capabilities& e)
	{
		debug(LOG_ERROR) << "Failed to create WiggleShaderProgram due to missing OpenGL capabilities: " << e.what() << "\n";
	}
	catch (const OpenGL::shader_source_error& e)
	{
		debug(LOG_ERROR) << "Failed to compile, link or validate the WiggleShaderProgram: " << e.what() << "\n"
		                 << "Info log contains:\n"
		                 << e.infoLog();
	}

	pos = RandomPos();
	finalGoalPos = Eigen::Vector3f(0., 0., 0.);
	velocity=Eigen::Vector3f(0., 0., 0.);
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
}

Eigen::Vector3f Vis::RandomPos() const
{
	Eigen::Vector3f result;
	result.x() = (my_random() - 0.5) * swimArea.x();
	int noswim = maxFloorHeight - model->bb_l.y() * scale;
	result.y() = (my_random() - 0.5) * (swimArea.y() - noswim) + (noswim / 2);
	result.z() = (my_random() - 0.5) * swimArea.z();
	return result;
}

void Vis::LoadProperties(const string &propertiesFile)
{
	string s;
	ifstream input_file((datadir + "/Vissen/" + propertiesFile + ".oif").c_str());

	//scaling
	getline(input_file, s);
	double fish_length = atof(s.c_str());
	//randomize
	getline(input_file, s);
	float random_length = atof(s.c_str());
	fish_length = fish_length + my_random() * random_length;

	if (model)
	{
		scale = fish_length / (model->bb_h.x() - model->bb_l.x());
	}
	else
	{
		scale = fish_length;
		debug(LOG_ERROR) << "Trying to load properties for fish that has no model, cant determine scaling";
	}

	//speeds
	getline(input_file, s);
	min_speed = atof(s.c_str());

	getline(input_file, s);
	max_speed = atof(s.c_str());

	//sphere
	getline(input_file, s);
	radius = (model->bb_h - model->bb_l).norm() * scale * 0.5 + atoi(s.c_str());

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

bool Vis::Colliding(const Eigen::Vector3f& object, float otherRadius)
{
	Eigen::Vector3f const swimDirection(goalPos - pos),
	                      objectDirection(object - pos);
	float const swimDistance   = swimDirection.norm(),
	            objectDistance = objectDirection.norm();

	if (objectDistance < (radius + otherRadius))
	{
		/*
		 * We're using this theorem from the dot product, u and v are
		 * vectors, T is the smallest angle between those vectors.
		 *
		 * u · v = |u| |v| cos T
		 *
		 * Thus:
		 *           u · v
		 * cos T = ---------
		 *          |u| |v|
		 *
		 * Additionally, given that T is always the smallest angle,
		 * thus T is always <= π. Also cos x / dx for 0 <= x <= π is
		 * always <= 0. Thus for S,T in [0,π] S < T, S > T and S = T
		 * imply cos S < cos T, cos S > T and cos S = T respectively.
		 *
		 * Thus while we cannot use cos T as T, we can use it to
		 * compare the angle T with another angle S. Hence we don't
		 * need to find T by obtaining arccos cos T, which we would be
		 * quite computationally expensive.
		 */
		float const cos_angle = swimDirection.dot(objectDirection)
		                      / (swimDistance * objectDistance);

		/*
		 * Only act on our collision if *this* is the object crashing
		 * right into it, i.e. our angle with that object is less than
		 * 90˚. If our angle is greater than 90˚ then we are in fact
		 * already traveling away from the colliding object. For angles
		 * x from 0˚ to 90˚ cos(x) ranges from 1 to 0, i.e. remains
		 * positive.
		 */
		if (cos_angle >= 0.f)
		{
			collided = 1.f;
			return true;
		}
	}

	return false;
}

bool Vis::IsGoingTowards(const Eigen::Vector3f& object)
{
	return (goalPos - pos).dot(object - pos) > 0;
}

static GLUquadric* TheQuadric()
{
	static GLUquadric* result = NULL;
	if (!result)
	{
		result = gluNewQuadric();
	}

	return result;
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
		glTranslatef(pos.x(), pos.y(), pos.z());
		glRotatef(-swimDirAngle * 180. / M_PI, 0,1,0);

		glRotatef(pitch * 180. / M_PI, 0,0,1);

		glScalef(scale,scale,scale);
		glEnable(GL_NORMALIZE);

		if (_wiggle)
		{
			OpenGL::use_scoped_program use_shader(*_wiggle);
			_wiggle->update(Eigen::Vector3f(wiggle_freq * scale, 0, 0),
				       Eigen::Vector3f(0, 0, wiggle_amplitude / scale),
				       wiggle_phase,
				       bending * scale);

			model->render();
		}
		else
		{
			model->render();
		}

		glPopMatrix();
	}
}

void Vis::DrawCollisionSphere() const
{
	if (!model)
		return;

	glPushMatrix();
	glTranslatef(pos.x(), pos.y(), pos.z());
	glRotatef(-swimDirAngle * 180. / M_PI, 0,1,0);

	glRotatef(pitch * 180. / M_PI, 0,0,1);

	glEnable(GL_NORMALIZE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/*
	 * Interpolate from uncollided_colour to collision_colour using
	 * collided, which ranges from 0 to 1.
	 *
	 * Use this equation, for linear interpolation between two
	 * vectors, w is the interpolated vector, u and v are the
	 * vectors to interpolate between according to the
	 * interpolation factor f which ranges from 0 (w=u) to 1 (w=1):
	 *
	 *   w = u - f(u - v)
	 */
	Eigen::Vector4f colour(uncollided_colour - collided * (uncollided_colour - collision_colour));
	glColor4fv(colour.data());
	gluSphere(TheQuadric(), radius, 21, 21);

	glColor3f(1.f, 1.f, 1.f);
	glDisable(GL_BLEND);
	glPopMatrix();

	if (collided >= 0.f)
	{
		collided -= 1.f / 30.f;
		if (collided < 0.f)
			collided = 0.f;
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

void Vis::setGoal(const Eigen::Vector3f& final_goal)
{
	if(usingTempGoal)
	{
		finalGoalPos = final_goal;
	}
	else
	{
		goalPos=final_goal;
	}
}

void Vis::setTemporaryGoal(const Eigen::Vector3f& temp_goal)
{
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
	pos += velocity * dt;

	Eigen::Vector3f delta = goalPos - pos;
	double dist = delta.norm();
	if (dist<desired_speed)/// if we're about to pass goal in less than second, change the goal
	{
		newGoal();
		return;
	}
	/*
	//Eigen::Vector3d desiredVelocity=delta*(speed/dist);
	if(desiredVelocity.y>velocity.y){/// vertical movement.
		velocity.y+=dt*acceleration;
	}else{
		velocity.y-=dt*acceleration;
	}

	*/
	//horisontale beweging
	Eigen::Vector3d fishForward(cos(swimDirAngle), 0, sin(swimDirAngle));

	double dfs=dt*forward_acceleration;
	speed=TowardsGoalBy(speed, desired_speed, dfs);

	velocity.x() = fishForward.x() * speed;
	velocity.z() = fishForward.z() * speed;
	//vertikale beweging
	double desired_vertical_speed = delta.y() * speed/ dist;// /sqrt(delta.x*delta.x+delta.z*delta.z)
	double dvs = vertical_acceleration * dt;

	velocity.y() = TowardsGoalBy(velocity.y(), desired_vertical_speed, dvs);

	//double forwardFactor = fishForward.dot(desiredVelocity.normalized());

	//velocity+=dt*fishForward*acceleration;// *forwardFactor

/*
	double current_speed = velocity.norm();
	if(current_speed>speed){/// moving too fast - slow down to exactly speed. TODO: do it smootly for less robotic turns.
		velocity*=speed/current_speed;
	}
	*/

	//omdraaien
	double goalHeading = atan2(delta.z(), delta.x());

	double angleDelta=goalHeading-swimDirAngle;
	//maak de kleinst mogelijke draai (-M_PI to +M_PI)
	if(angleDelta > M_PI)
	{
		angleDelta -= 2 * M_PI;
	}
	if(angleDelta < -M_PI)
	{
		angleDelta += 2 * M_PI;
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


	if(swimDirAngle > M_PI)
	{
		swimDirAngle -= 2 * M_PI;
	}
	if(swimDirAngle < -M_PI)
	{
		swimDirAngle += 2 * M_PI;
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
	const float desired_pitch = clip(atan2f(pitch_factor * velocity.y(), sqrt(velocity.x() * velocity.x() + velocity.z() * velocity.z())),
	                                 min_pitch,
	                                 max_pitch);
	pitch = TowardsGoalBy(pitch, desired_pitch, pitch_change_speed*dt);


	/// stabielheid dingen:

	//even zorgen dat de wiggle_phase niet te hoog wordt
	if(wiggle_phase > 2 * M_PI)
	{
		wiggle_phase -= 2 * M_PI;
	}

	myWaitTime -= dt;
	if (myWaitTime < 0)
	{
		myWaitTime = ((int)20+20*my_random());
		newGoal();
	}
}

