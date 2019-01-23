#pragma once
#include <irrlicht.h>
#include "RingBuffer.h"

using namespace irr;


class Graph : public scene::ISceneNode
{


	core::aabbox3d<f32> Box;
	video::S3DVertex Vertices[4];
	video::SMaterial Material;

	core::rect<s32> pos;
	video::SColor colorRect;

	wchar_t* caption;

	int bufSize, numBuffers;
	RingBuffer<core::vector2df>* buffers;
	int nextIdx = 0, numElements = 0;



public:
	Graph(wchar_t* caption, core::rect<s32> pos, int numBuffers, int bufSize, scene::ISceneManager* mgr, s32 id)
		: scene::ISceneNode(0, mgr, id)
	{
		this->pos = pos;
		this->bufSize = bufSize;
		this->numBuffers = numBuffers;
		buffers = (RingBuffer<core::vector2df>*) malloc(sizeof(RingBuffer<core::vector2df>) * numBuffers);
		for (int i = 0; i < numBuffers; ++i)
			buffers[i] = RingBuffer<core::vector2df>(bufSize);

		colorRect.set(50, 50, 50, 50);

		Box.reset(Vertices[0].Pos);
		for (s32 i = 1; i<4; ++i)
			Box.addInternalPoint(Vertices[i].Pos);
	}

	~Graph() {
		delete buffers;
	}

	void addVal(int buffer, core::vector2df val) {
		buffers[buffer].push(val);
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
