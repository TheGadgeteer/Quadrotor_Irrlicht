#include "Quadrotor.h"
#include <cmath>

#define _METER *100

#define FORCE_FACTOR (2 * 9.81f _METER / 4 * weight / maxRPS) // half power: floating)
#define DRAG_PER_SPEED  14.24f // A 80kg person will have static speed at 200 km/h (55 m/s)
#define PI 3.14159265f
#define YAW_FACTOR 1.f
#define INERTIA (2*weight*WEIGHT_OUTER_FACTOR * size/2*size/2) // Two propellers opposite, distance to mid size/2, weight weight*WEIGHT_OUTER_FACTOR

#define WEIGHT_INNER_FACTOR 0.5f
#define WEIGHT_OUTER_FACTOR 0.125f

Quadrotor::Quadrotor(float size, float weight,
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

	video::SMaterial& weightNodeMaterial = weightNode->getMaterial(0);
	weightNodeMaterial.Lighting = true;
	weightNodeMaterial.ColorMaterial = video::ECM_AMBIENT;
	//weightNodeMaterial.EmissiveColor = video::SColor(255, 0, 200, 0);
	weightNodeMaterial.AmbientColor = video::SColor(255, 250, 250, 10);
	weightNodeMaterial.DiffuseColor = video::SColor(255, 70, 70, 70);

	ISceneNode* rodNodes[2];
	for (int i = 0; i < 2; ++i) {
		rodNodes[i] = smgr->addCubeSceneNode(size * 2 * sqrt(2.), this, -1, core::vector3df(0, size*(0.5f - rodSizeFactor) - 1, 0),
			core::vector3df(0, ((i == 0) ? -45 : 45), 0), core::vector3df(1.f, rodSizeFactor, rodSizeFactor));
		rodNodes[i]->getMaterial(0).EmissiveColor = video::SColor(255, 40, 40, 40);
	}


	for (int i = 0; i < 4; ++i) {
		rotor[i] = smgr->addMeshSceneNode(smgr->getMesh("../media/Propeller.obj"), this);
		int x = i / 2, y = i % 2;
		rotor[i]->setPosition(core::vector3df(size - 2 * x*size, size / 2 + size*0.05, size - 2 * y*size));
		rotor[i]->setRotation(core::vector3df(90, 0, 180));
		rotor[i]->setScale(core::vector3df(size/1.7f, size/1.7f, size/1.7f));
		rotor[i]->getMaterial(0).EmissiveColor = video::SColor(255, 120 + i * 30,  100 + i*30, 80 + i*30);
		rotor[i]->getMaterial(0).Lighting = true;
		rotor[i]->getMaterial(0).ColorMaterial = video::ECM_DIFFUSE_AND_AMBIENT;
	}

	smgr->addCubeSceneNode(size / 2, weightNode, -1, core::vector3df(size / 4 + 0.2, 0, 0))->getMaterial(0).DiffuseColor= video::SColor(255, 10, 10, 10);
	
	Box.reset(Vertices[0].Pos);
	for (s32 i = 1; i<4; ++i)
		Box.addInternalPoint(Vertices[i].Pos);

	this->reset();
}


void Quadrotor::setMotorSpeed(float speed[]) {
	for (int i = 0; i < 4; ++i) {
		if (speed[i] < -1)
			speed[i] = -1;
		else if (speed[i] > 1)
			speed[i] = 1;
		wantedMotorSpeed[i] = speed[i] * maxRPS;
	}
}

void Quadrotor::update(f64 elapsedTime) {
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

	core::vector3df rot = this->getRotation();
	//core::vector3df normal(sinf(rot.X * 2 *PI / 360), cosf(rot.Y *2 * PI / 360), 0);
	//printf("Normal: %.3f %.3f %.3f\n",plane.Normal.X, plane.Normal.Y, plane.Normal.Z);


	force += plane.Normal * forceSum;

	//aerodynamic drag
	force -= speed * DRAG_PER_SPEED;


	speed += force / weight * elapsedTime;
	pos += speed * elapsedTime;


	//printf("speed: %.3f %.3f %.3f\n", speed.X, speed.Y, speed.Z);

	//printf("motorSpeed: %.3f, force: %.3f %.3f %.3f\n", forceSum, force.X, force.Y, force.Z);
	
	//printf("Position: %.3f %.3f %.3f\n", pos.X, pos.Y, pos.Z);
	this->setPosition(pos);

	// Calculate Angular Forces and update Rotation
	rot = this->getRotation();
	
	core::vector3df angularForce;
	angularForce.X = size/2  * FORCE_FACTOR * 
		(-motorSpeed[0]  - motorSpeed[2] + motorSpeed[1] + motorSpeed[3]);
	angularForce.Z = - size/2 * FORCE_FACTOR *
		(-motorSpeed[0] - motorSpeed[1] + motorSpeed[2] + motorSpeed[3]);
	angularForce.Y = (motorSpeed[0] + motorSpeed[3]
		- motorSpeed[1] - motorSpeed[2]) * YAW_FACTOR;
	//printf("angular Force: %.3f %.3f %.3f\n",angularForce.X, angularForce.Y, angularForce.Z);

	// Approximation for aerodynamic drag
	angularForce -= angularSpeed *2*PI/360 * size/2 * DRAG_PER_SPEED / 4;

	angularSpeed += angularForce * 1.f / INERTIA * elapsedTime * 360 / 2 / PI; // in degrees
	//printf("angular Speed: %.3f %.3f %.3f, elapsedTime: %f\n", angularSpeed.X, angularSpeed.Y, angularSpeed.Z, elapsedTime);

	//rot += core::vector3df(0, 90, 0) * elapsedTime;
	rot += angularSpeed * elapsedTime;

	this->setRotation(rot);

	// Restrict Height to > 0
	if (pos.Y < 0) {
		//printf("restricted height");
		pos.Y = 0;
		speed.Y = 0;
		speed *= 0.2f;
		this->setPosition(pos);
		this->setRotation(core::vector3df(0, 0, 0));
	}
}