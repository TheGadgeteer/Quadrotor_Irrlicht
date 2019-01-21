#pragma once
#include <irrlicht.h>

using namespace irr;

class Quadcopter : public scene::ISceneNode
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

	Quadcopter(float size,  float weight,
		float maxRPM, float gravity, scene::ISceneNode* parent, scene::ISceneManager* smgr, s32 id);

	virtual void OnRegisterSceneNode()
	{
		if (IsVisible)
			SceneManager->registerNodeForRendering(this);

		ISceneNode::OnRegisterSceneNode();
	}

	// Every element in speed is between -1 and 1
	void setMotorSpeed(float speed[]);

	virtual void render()
	{
		/*video::IVideoDriver* driver = SceneManager->getVideoDriver();

		driver->setMaterial(Material);
		driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
		driver->drawVertexPrimitiveList(&Vertices[0], 4, indices, 2, video::EVT_STANDARD, scene::EPT_TRIANGLES, video::EIT_16BIT);
		*/
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
