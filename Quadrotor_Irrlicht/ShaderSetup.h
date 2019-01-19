#pragma once
#include <irrlicht.h>

using namespace irr;

class MyShaderCallBack : public video::IShaderConstantSetCallBack
{
private:
	bool UseHighLevelShaders = false;
	IrrlichtDevice* device;
public:

	virtual void OnSetConstants(video::IMaterialRendererServices* services,
		s32 userData)
	{
		video::IVideoDriver* driver = services->getVideoDriver();

		// set inverted world matrix
		// if we are using highlevel shaders (the user can select this when
		// starting the program), we must set the constants by name.

		core::matrix4 invWorld = driver->getTransform(video::ETS_WORLD);
		invWorld.makeInverse();

		if (UseHighLevelShaders)
			services->setVertexShaderConstant("mInvWorld", invWorld.pointer(), 16);
		else
			services->setVertexShaderConstant(invWorld.pointer(), 0, 4);

		// set clip matrix

		core::matrix4 worldViewProj;
		worldViewProj = driver->getTransform(video::ETS_PROJECTION);
		worldViewProj *= driver->getTransform(video::ETS_VIEW);
		worldViewProj *= driver->getTransform(video::ETS_WORLD);

		if (UseHighLevelShaders)
			services->setVertexShaderConstant("mWorldViewProj", worldViewProj.pointer(), 16);
		else
			services->setVertexShaderConstant(worldViewProj.pointer(), 4, 4);

		// set camera position

		core::vector3df pos = device->getSceneManager()->
			getActiveCamera()->getAbsolutePosition();

		if (UseHighLevelShaders)
			services->setVertexShaderConstant("mLightPos", reinterpret_cast<f32*>(&pos), 3);
		else
			services->setVertexShaderConstant(reinterpret_cast<f32*>(&pos), 8, 1);

		// set light color

		video::SColorf col(0.0f, 1.0f, 1.0f, 0.0f);

		if (UseHighLevelShaders)
			services->setVertexShaderConstant("mLightColor",
				reinterpret_cast<f32*>(&col), 4);
		else
			services->setVertexShaderConstant(reinterpret_cast<f32*>(&col), 9, 1);

		// set transposed world matrix

		core::matrix4 world = driver->getTransform(video::ETS_WORLD);
		world = world.getTransposed();

		if (UseHighLevelShaders)
		{
			services->setVertexShaderConstant("mTransWorld", world.pointer(), 16);

			// set texture, for textures you can use both an int and a float setPixelShaderConstant interfaces (You need it only for an OpenGL driver).
			s32 TextureLayerID = 0;
			if (UseHighLevelShaders)
				services->setPixelShaderConstant("myTexture", &TextureLayerID, 1);
		}
		else
			services->setVertexShaderConstant(world.pointer(), 10, 4);
	}
};

void setupShader(IrrlichtDevice* device, bool UseHighLevelShaders, 
	video::IVideoDriver* driver, video::E_DRIVER_TYPE driverType,
	io::path& psFileName, io::path& vsFileName) {

	switch (driverType)
	{
	case video::EDT_DIRECT3D8:
		psFileName = "../media/d3d8.psh";
		vsFileName = "../media/d3d8.vsh";
		break;
	case video::EDT_DIRECT3D9:
		if (UseHighLevelShaders)
		{
			// Cg can also handle this syntax
			psFileName = "../media/d3d9.hlsl";
			vsFileName = psFileName; // both shaders are in the same file
		}
		else
		{
			psFileName = "../media/d3d9.psh";
			vsFileName = "../media/d3d9.vsh";
		}
		break;

	case video::EDT_OPENGL:
		if (UseHighLevelShaders)
		{
			psFileName = "../media/opengl.frag";
			vsFileName = "../media/opengl.vert";

		}
		else
		{
			psFileName = "../media/opengl.psh";
			vsFileName = "../media/opengl.vsh";
		}
		break;
	}

	if (!driver->queryFeature(video::EVDF_PIXEL_SHADER_1_1) &&
		!driver->queryFeature(video::EVDF_ARB_FRAGMENT_PROGRAM_1))
	{
		device->getLogger()->log("WARNING: Pixel shaders disabled "\
			"because of missing driver/hardware support.");
		psFileName = "";
	}

	if (!driver->queryFeature(video::EVDF_VERTEX_SHADER_1_1) &&
		!driver->queryFeature(video::EVDF_ARB_VERTEX_PROGRAM_1))
	{
		device->getLogger()->log("WARNING: Vertex shaders disabled "\
			"because of missing driver/hardware support.");
		vsFileName = "";
	}
}