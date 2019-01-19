#include <irrlicht.h>
#include <iostream>
#include "driverChoice.h"
#include "ShaderSetup.h"

using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif


IrrlichtDevice* device = 0;
bool UseHighLevelShaders = false;



int main()
{
	// ask user for driver
	video::E_DRIVER_TYPE driverType = driverChoiceConsole();
	if (driverType == video::EDT_COUNT)
		return 1;

	UseHighLevelShaders = true;

	// create device
	device = createDevice(driverType, core::dimension2d<u32>(640, 480));

	if (device == 0)
		return 1; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();
	gui::IGUIEnvironment* gui = device->getGUIEnvironment();

	io::path vsFileName; // filename for the vertex shader
	io::path psFileName; // filename for the pixel shader

	setupShader(device, UseHighLevelShaders, driver, driverType,
		psFileName, vsFileName);

	// add a nice skybox

	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);

	smgr->addSkyBoxSceneNode(
		driver->getTexture("../media/Yokohama2/posy.jpg"),
		driver->getTexture("../media/Yokohama2/negy.jpg"),
		driver->getTexture("../media/Yokohama2/negz.jpg"),
		driver->getTexture("../media/Yokohama2/posz.jpg"),
		driver->getTexture("../media/Yokohama2/negx.jpg"),
		driver->getTexture("../media/Yokohama2/posx.jpg"));

	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, true);

	// add a camera and disable the mouse cursor

	scene::ICameraSceneNode* cam = smgr->addCameraSceneNodeFPS();
	cam->setPosition(core::vector3df(0, 0, 0));
	cam->setTarget(core::vector3df(1, 0, 0));
	//device->getCursorControl()->setVisible(false);


	int lastFPS = -1;
	u32 now, then;
	then = device->getTimer()->getTime();

	while (device->run())
		if (device->isWindowActive())
		{
			now = device->getTimer()->getTime();
			f64 timeElapsed = (now - then) / 1000.;
			then = now;
			driver->beginScene(true, true, video::SColor(255, 0, 0, 0));
			smgr->drawAll();
			driver->endScene();

			int fps = driver->getFPS();

			if (lastFPS != fps)
			{
				core::stringw str = L"Irrlicht Engine - Vertex and pixel shader example [";
				str += driver->getName();
				str += "] FPS:";
				str += fps;

				device->setWindowCaption(str.c_str());
				lastFPS = fps;
			}
		}

	device->drop();

	return 0;
}
