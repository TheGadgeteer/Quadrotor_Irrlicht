#pragma once
#include <irrlicht.h>

using namespace irr;


class PlatformNode : public scene::ISceneNode
{

	/*
	First, we declare some member variables:
	The bounding box, 4 vertices, and the material of the tetraeder.
	*/
	core::aabbox3d<f32> Box;
	video::S3DVertex Vertices[4];
	video::SMaterial Material;

	/*
	The parameters of the constructor specify the parent of the scene node,
	a pointer to the scene manager, and an id of the scene node.
	In the constructor we call the parent class' constructor,
	set some properties of the material, and
	create the 4 vertices of the tetraeder we will draw later.
	*/

public:

	PlatformNode(float xLength, float yLength, video::ITexture* texture,
		scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id)
		: scene::ISceneNode(parent, mgr, id)
	{
		Material.Wireframe = false;
		Material.Lighting = false;
		this->setMaterialTexture(0, texture);
		Material.BackfaceCulling = false;

		Vertices[0] = video::S3DVertex(-xLength/2, 0, -yLength/2, 1, 0, 0,
			video::SColor(255, 255, 255, 255), 0, 0);
		Vertices[1] = video::S3DVertex(-xLength / 2, 0, yLength/2, 1, 0, 0,
			video::SColor(255, 255, 255, 255), 0, 1);
		Vertices[2] = video::S3DVertex(xLength / 2, 0, -yLength/2, 1, 0, 0,
			video::SColor(255, 255, 255, 255), 1, 0);
		Vertices[3] = video::S3DVertex(xLength/2, 0, yLength/2, 1, 0, 0,
			video::SColor(255, 255, 255, 255), 1, 1);

		Box.reset(Vertices[0].Pos);
		for (s32 i = 1; i<4; ++i)
			Box.addInternalPoint(Vertices[i].Pos);
	}

	virtual void OnRegisterSceneNode()
	{
		if (IsVisible)
			SceneManager->registerNodeForRendering(this);

		ISceneNode::OnRegisterSceneNode();
	}

	/*
	In the render() method most of the interesting stuff happens: The
	Scene node renders itself. We override this method and draw the
	tetraeder.
	*/
	virtual void render()
	{
		//u16 indices[] = { 0,2,3, 2,1,3, 1,0,3, 2,0,1 };
		u16 indices[] = { 0,1,2, 3,2,1};
		video::IVideoDriver* driver = SceneManager->getVideoDriver();

		driver->setMaterial(Material);
		driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
		driver->drawVertexPrimitiveList(&Vertices[0], 4, indices, 2, video::EVT_STANDARD, scene::EPT_TRIANGLES, video::EIT_16BIT);
	}

	/*
	And finally we create three small additional methods.
	irr::scene::ISceneNode::getBoundingBox() returns the bounding box of
	this scene node, irr::scene::ISceneNode::getMaterialCount() returns the
	amount of materials in this scene node (our tetraeder only has one
	material), and irr::scene::ISceneNode::getMaterial() returns the
	material at an index. Because we have only one material here, we can
	return the only one material, assuming that no one ever calls
	getMaterial() with an index greater than 0.
	*/
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
