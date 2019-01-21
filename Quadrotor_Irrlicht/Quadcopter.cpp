#include "Quadcopter.h"
#include <cmath>

#define FORCE_FACTOR 2 * 9.81f / 4 * weight / maxRPS // half power: floating)
#define DRAG_PER_SPEED 14.24f // A 80kg person will have static speed at 200 km/h
#define PI 3.14159265f
#define YAW_FACTOR 0.00001f

Quadcopter::Quadcopter(float size, float weight,
	float maxRPS, float gravity, scene::ISceneNode* parent, scene::ISceneManager* smgr, s32 id)
	: scene::ISceneNode(parent, smgr, id), weight(weight), gravity(gravity), maxRPS(maxRPS)

{
	Material.Wireframe = false;
	Material.Lighting = false;

	this->size = size;
	for (int i = 0; i < 4; ++i) {
		this->motorSpeed[i] = 0.f;
		this->wantedMotorSpeed[i] = 0.f;
	}

	ISceneNode* weightNode = smgr->addCubeSceneNode(size, this, -1, core::vector3df(0, size/4, 0));
	weightNode->setScale(core::vector3df(1, 0.5f, 1.f));
	weightNode->getMaterial(0).EmissiveColor = video::SColor(255, 30, 60, 30);

	ISceneNode* rodNodes[2];
	for (int i = 0; i < 2; ++i) {
		rodNodes[i] = smgr->addCubeSceneNode(size * 2 * sqrt(2.), this, -1, core::vector3df(0, size*(0.5f - rodSizeFactor) - 0.5f, 0),
			core::vector3df(0, ((i == 0) ? -45 : 45), 0), core::vector3df(1, rodSizeFactor, rodSizeFactor));
		rodNodes[i]->getMaterial(0).EmissiveColor = video::SColor(255, 40, 40, 40);
	}


	for (int i = 0; i < 4; ++i) {
		rotor[i] = smgr->addMeshSceneNode(smgr->getMesh("../media/Propeller.obj"), this);
		int x = i / 2, y = i % 2;
		rotor[i]->setPosition(core::vector3df(-size + 2 * x*size, size / 2 + size*0.05, -size + 2 * y*size));
		rotor[i]->setRotation(core::vector3df(90, 0, 180));
		rotor[i]->setScale(core::vector3df(size/1.7f, size/1.7f, size/1.7f));
		rotor[i]->getMaterial(0).EmissiveColor = video::SColor(255, 60 + i * 40, 42 + i*40, 42);
	}

	smgr->addCubeSceneNode(size / 2, weightNode, -1, core::vector3df(size / 4 + 0.2, 0, 0));
	
	Box.reset(Vertices[0].Pos);
	for (s32 i = 1; i<4; ++i)
		Box.addInternalPoint(Vertices[i].Pos);
}


void Quadcopter::setMotorSpeed(float speed[]) {
	for (int i = 0; i < 4; ++i) {
		if (speed[i] < -1)
			speed[i] = -1;
		else if (speed[i] > 1)
			speed[i] = 1;
		wantedMotorSpeed[i] = speed[i] * maxRPS;
	}
}

void Quadcopter::update(f64 elapsedTime) {
	// Update speed of Rotors
	for (int i = 0; i < 4; ++i) {
		motorSpeed[i] += (wantedMotorSpeed[i] - motorSpeed[i]) * (1 - std::exp(-elapsedTime / rotorTimeConstant));
		core::vector3df rot = rotor[i]->getRotation();
		rot.Y += (i == 0 || i == 3 ? 1 : -1) * motorSpeed[i] * 360 * elapsedTime; // rotation in degree, not radian
		rotor[i]->setRotation(rot);
	}

	// Calculate Forces and update Position
	core::vector3df  pos = this->getPosition();
	core::vector3df force(0, -gravity * weight, 0);
	float forceSum = 0.f;
	for (int i = 0; i < 4; ++i)
		forceSum += motorSpeed[i] * FORCE_FACTOR;
	// Construct a plane out of the rotor positions; its normal is the direction of the force
	core::plane3df plane(rotor[0]->getAbsolutePosition(), rotor[1]->getAbsolutePosition(),
		rotor[2]->getAbsolutePosition());

	force += plane.Normal * forceSum;

	//aerodynamic drag
	force -= speed * DRAG_PER_SPEED;


	speed += force / weight * elapsedTime;
	pos += speed * elapsedTime;


	//printf("speed: %.3f %.3f %.3f\n", speed.X, speed.Y, speed.Z);
	//printf("normal: %.3f %.3f %.3f\n", normal.X, normal.Y, normal.Z);
	//printf("motorSpeed: %.3f, force: %.3f %.3f %.3f\n", forceSum, force.X, force.Y, force.Z);


	// Restrict Height to > 0
	if (pos.Y < 0) {
		//printf("restricted height");
		pos.Y = 0;
		speed.Y = 0;
		speed *= 0.2;

	}
	printf("Position: %.3f %.3f %.3f\n", pos.X, pos.Y, pos.Z);
	//this->setPosition(pos);

	return;

	// Calculate Angular Forces and update Rotation
	core::vector3df rot = this->getRotation();

	core::vector3df angularForce;
	angularForce.X = size/2 * sqrt(2) * FORCE_FACTOR * 
		(motorSpeed[0]  + motorSpeed[2] - motorSpeed[1] - motorSpeed[3]);
	angularForce.Z = size/2 * sqrt(2) * FORCE_FACTOR *
		(motorSpeed[0] + motorSpeed[1] - motorSpeed[2] - motorSpeed[3]);

	angularForce.Y = (motorSpeed[0] + motorSpeed[3]
		- motorSpeed[1] - motorSpeed[2]) * YAW_FACTOR;

	//angularForce -= angularSpeed * size/2 *  DRAG_PER_SPEED;

	angularSpeed += angularForce / (weight/4)* elapsedTime * 360 / 2 / PI;
	//rot += angularSpeed * elapsedTime;

	this->setRotation(rot);

}