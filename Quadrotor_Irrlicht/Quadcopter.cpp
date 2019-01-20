#include "Quadcopter.h"
#include <cmath>

Quadcopter::Quadcopter(float size, float weight,
	float maxRPM, float gravity, scene::ISceneNode* parent, scene::ISceneManager* smgr, s32 id)
	: scene::ISceneNode(parent, smgr, id), weight(weight), gravity(gravity), maxRPM(maxRPM)
{
	Material.Wireframe = false;
	Material.Lighting = false;

	this->size = size;
	for (int i = 0; i < 4; ++i) {
		this->motorSpeed[i] = 0.f;
		this->wantedMotorSpeed[i] = 0.f;
	}

	ISceneNode* weightNode = smgr->addCubeSceneNode(size, this, -1, core::vector3df(0, size/4, 0));
	weightNode->setScale(core::vector3df(1, 0.5f, 0.8f));
	weightNode->getMaterial(0).EmissiveColor = video::SColor(255, 30, 60, 30);

	ISceneNode* rodNodes[2];
	for (int i = 0; i < 2; ++i) {
		rodNodes[i] = smgr->addCubeSceneNode(size * 2 * sqrt(2.), this, -1, core::vector3df(0, size*(0.5f - rodSizeFactor) - 2, 0),
			core::vector3df(0, ((i == 0) ? -45 : 45), 0), core::vector3df(1, rodSizeFactor, rodSizeFactor));
		rodNodes[i]->getMaterial(0).EmissiveColor = video::SColor(255, 40, 40, 40);
	}


	for (int i = 0; i < 4; ++i) {
		rotor[i] = smgr->addMeshSceneNode(smgr->getMesh("../media/Propeller.obj"));
		int x = i / 2, y = i % 2;
		rotor[i]->setPosition(core::vector3df(-size + 2 * x*size, size / 2 + size*0.05, -size + 2 * y*size));
		rotor[i]->setRotation(core::vector3df(90, 0, 180));
		rotor[i]->setScale(core::vector3df(size/1.7f, size/1.7f, size/1.7f));
		rotor[i]->getMaterial(0).EmissiveColor = video::SColor(255, 60, 42, 42);
	}

	//Box.reset(Vertices[0].Pos);
	for (s32 i = 1; i<4; ++i)
		Box.addInternalPoint(Vertices[i].Pos);
}

void Quadcopter::update(f64 elapsedTime) {
	// Update speed of Rotors
	for (int i = 0; i < 4; ++i) {
		motorSpeed[i] += (wantedMotorSpeed[i] - motorSpeed[i]) * (1 - std::exp(-elapsedTime / rotorTimeConstant));
		core::vector3df rot = rotor[i]->getRotation();
		rot.Y += (i == 0 || i == 3 ? 1 : -1) * motorSpeed[i] * 360 / 60 * elapsedTime; // Speed is given in RPM; convert into radians/sec
		rotor[i]->setRotation(rot);
	}

	// Calculate Forces and update Position
	core::vector3df force(0, -gravity, 0);
	float forceSum = 0.f;
	for (int i = 0; i < 4; ++i)
		forceSum += motorSpeed[i] * 05; //TODO: MAGIC_NUMBER

	force += this->getRotation() * forceSum;

	speed += force * elapsedTime;

	core::vector3df newPos = this->getPosition();
	newPos += speed * elapsedTime;

	// Restrict Height to > 0
	if (newPos.Y < 0) {
		newPos.Y = 0;
		speed.Y = 0;
	}
	this->setPosition(newPos);

	// Calculate Angular Forces and update Rotation
	core::vector3df angularForce(0, 0, 0);
	//TODO

	core::vector3df newRot = this->getRotation();
	newRot += angularSpeed * elapsedTime;

	this->setRotation(newRot);

}