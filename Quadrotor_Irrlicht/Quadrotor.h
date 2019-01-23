#pragma once
#include <irrlicht.h>

using namespace irr;

class Quadrotor : public scene::ISceneNode
{
private:
	core::aabbox3d<f32> Box;
	video::S3DVertex Vertices[4];
	video::SMaterial Material;
	scene::IMeshSceneNode* rotor[4];

	float motorSpeed[4];
	float wantedMotorSpeed[4];
	core::vector3df angularSpeed = core::vector3df(0, 0, 0);
	core::vector3df speed = core::vector3df(0, 0, 0);

	const float rodSizeFactor = 0.03f;
	const float rotorTimeConstant = 1.f;

	float size;
	const float weight, maxRPS, gravity;

public:

	Quadrotor(float size,  float weight,
		float maxRPM, float gravity, scene::ISceneNode* parent, scene::ISceneManager* smgr, s32 id);

	virtual void OnRegisterSceneNode()
	{
		if (IsVisible)
			SceneManager->registerNodeForRendering(this);

		ISceneNode::OnRegisterSceneNode();
	}

	core::vector3df getSpeed() {
		return speed;
	}

	core::vector3df getAngularSpeed() {
		return speed;
	}
	// Every element in speed is between -1 and 1
	void setMotorSpeed(float speed[]);

	float getMotorSpeed(int motor) {
		return motorSpeed[motor] / maxRPS;
	}

	float getWantedMotorSpeed(int motor) {
		return wantedMotorSpeed[motor] / maxRPS;
	}

	virtual void render()
	{
		/*video::IVideoDriver* driver = SceneManager->getVideoDriver();

		driver->setMaterial(Material);
		driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
		driver->drawVertexPrimitiveList(&Vertices[0], 4, indices, 2, video::EVT_STANDARD, scene::EPT_TRIANGLES, video::EIT_16BIT);
		*/
	}

	void reset() {
		this->setPosition(core::vector3df(0, 0, 0));
		this->setRotation(core::vector3df(0, 0, 0));
		this->angularSpeed = core::vector3df(0, 0, 0);
		this->speed = core::vector3df(0, 0, 0);
		for (int i = 0; i < 4; ++i) {
			this->wantedMotorSpeed[i] = 0;
			this->motorSpeed[i] = 0;
		}
	}

	void update(f64 elapsedTime);


	virtual const core::aabbox3d<f32>& getBoundingBox() const
	{
		return Box;
	}

	virtual u32 getMaterialCount() const
	{
		return 1;
	}

	virtual video::SMaterial& getMaterial(u32 i)
	{
		return Material;
	}
};
