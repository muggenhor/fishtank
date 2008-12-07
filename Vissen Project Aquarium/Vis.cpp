#include "Vis.h"
#include <cstdlib>
#include <ctime>

float my_random()
{
	return rand() / float(RAND_MAX);
}

using namespace math3;

const Vec3d aquariumSize(200,200,200);
const double max_speed=10.0;
const double min_speed=5.0;
const double acceleration=10;

const double turn_speed=1.0;///

Vec3d RandomPos()
{
	Vec3d result;
	result.x=(my_random()-0.5)*aquariumSize.x;
	result.y=(my_random()-0.5)*aquariumSize.y;
	result.z=(my_random()-0.5)*aquariumSize.z;
	return result;
}

Vis::Vis(Model *model) //defines model
{
	this->model = model;
	pos = RandomPos();
	goalPos = RandomPos();
	velocity=Vec3d(0,0,0);
	speed=min_speed+my_random()*(max_speed-min_speed);
	swimDirAngle=0;
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

		glPushMatrix();
		glTranslatef(pos.x,pos.y,pos.z);
		glRotatef(-swimDirAngle*180.0/pi, 0,1,0);

		model->render();

		glPopMatrix();
	}
}

void Vis::Update(double dt)
{
	pos += velocity*dt;

	Vec3d delta=goalPos-pos;
	double dist=Length(delta);
	if (dist<speed)/// if we're about to pass goal in less than second, change the goal
	{
		goalPos = RandomPos();
		speed=min_speed+my_random()*(max_speed-min_speed);
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
	if(angleDelta>pi) angleDelta=angleDelta - 2*pi;
	if(angleDelta<-pi)angleDelta=angleDelta + 2*pi;

	swimDirAngle+=dt*angleDelta*turn_speed;

}
