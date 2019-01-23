#pragma once
#pragma once
#include <irrlicht.h>
#include "RingBuffer.h"

using namespace irr;


class FuzzyGraph : public scene::ISceneNode
{


	core::aabbox3d<f32> Box;
	video::S3DVertex Vertices[4];
	video::SMaterial Material;

	core::rect<s32> pos;
	video::SColor colorRect;

	wchar_t* caption;

	int numVals;
	core::vector2df *vals;

public:
	FuzzyGraph(const wchar_t* caption, core::rect<s32> pos, int numVals,  scene::ISceneManager* mgr, s32 id)
		: scene::ISceneNode(0, mgr, id)
	{

		this->numVals = numVals;
		vals = new core::vector2df[numVals];
		colorRect.set(50, 50, 50, 50);

		Box.reset(Vertices[0].Pos);
		for (s32 i = 1; i<4; ++i)
			Box.addInternalPoint(Vertices[i].Pos);
	}

	~FuzzyGraph() {
		delete vals;
	}

	void nextVal(int buffer, core::vector2df val) {
		vals[buffer] = val;
	}

	virtual void OnRegisterSceneNode()
	{
		if (IsVisible)
			SceneManager->registerNodeForRendering(this);

		ISceneNode::OnRegisterSceneNode();
	}


	virtual void render()
	{
		video::IVideoDriver* driver = this->SceneManager->getVideoDriver();
		driver->enableMaterial2D();
		driver->draw2DRectangle(colorRect, pos);
	}

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
